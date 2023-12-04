#include "precomp.h" // include (only) this in every .cpp file
#include <ranges>

constexpr auto particle_beam_hit_value = 50;
constexpr auto tank_max_speed = 1.0;
constexpr auto max_frames = 2000;

// TODO'S
// Create global mutex variables in game_h.
// Mutex all places where in threads a vector is changed.
// Make the reload tank method more threadable without a lock around the whole block. Possibly only needed for the vector push_back.


// Global performance timer
// Start: 5.23m, speedup 0.4, 323866
// Grid collision (cells): 4.40m, speedup 1.2, 280300
// Tank health bar sort: 4:24m, speedup 1.1, 264179
// Convex hull: 4:24, speedup 1.0, 2649889
// Rocket collision (cells): 4:09, speedup: 1.0, 249488
// Added threads to tank: 3:12 speedup 1.3, 192187

// Tank update threaded, 300 frames: 61ms
// Tank update unthreaded, 300 frames: 90ms

// Rocket update threaded, 300 frames: 43ms
// Rocket update unthreaded, 300 frames: 51ms


constexpr auto REF_PERFORMANCE = 249488; //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

// Threadpool
BS::thread_pool pool(NUM_THREADS);

//Load sprite files and initialize sprites
static Surface* tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface* tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface* rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface* rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface* particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface* smoke_img = new Surface("assets/Smoke.png");
static Surface* explosion_img = new Surface("assets/Explosion.png");

static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(7, 9);
const static vec2 rocket_size(6, 6);

bool rocket_destroyed_this_round = false;

// -----------------------------------------------------------
// Initialize the simulation state
// This function does not count for the performance multiplier
// (Feel free to optimize anyway though ;) )
// -----------------------------------------------------------
void Game::init()
{

    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);

    uint max_rows = 24;

    float start_blue_x = tank_size.x + 40.0f;
    float start_blue_y = tank_size.y + 30.0f;

    float start_red_x = 1088.0f;
    float start_red_y = tank_size.y + 30.0f;

    float spacing = 7.5f;


    //Spawn blue tanks
    for (int i = 0; i < NUM_TANKS_BLUE; i++)
    {
        int tank_index = i;
        vec2 position{ start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing) };
        Cell* tank_cell = Cell::find_cell_for_tank(position.x, position.y, cells);
        tanks.push_back(Tank(tank_index, tank_cell->index, position.x, position.y, BLUE, &tank_blue, &smoke, 1100.f, position.y + 16, TANK_RADIUS, TANK_MAX_HEALTH, tank_max_speed));
        tank_cell->tank_indices.push_back(tank_index);
    }

    //Spawn red tanks
    for (int i = 0; i < NUM_TANKS_RED; i++)
    {
        int tank_index = NUM_TANKS_BLUE + i;
        vec2 position{ start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing) };
        Cell* tank_cell = Cell::find_cell_for_tank(position.x, position.y, cells);
        tanks.push_back(Tank(tank_index, tank_cell->index, position.x, position.y, RED, &tank_red, &smoke, 100.f, position.y + 16, TANK_RADIUS, TANK_MAX_HEALTH, tank_max_speed));
        tank_cell->tank_indices.push_back(tank_index);
    }

    particle_beams.push_back(Particle_beam(vec2(590, 327), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(64, 64), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
    particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, particle_beam_hit_value));
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::shutdown()
{
}

void Game::update(float deltaTime)
{
    //Calculate the route to the destination for each tank using BFS
    //Initializing routes here so it gets counted for performance..
    if (frame_count == 0)
    {
            for (Tank& t : tanks)
            {
                t.set_route(background_terrain.get_route(t, t.target));
            };
    }

    //Update tanks
    cout << "Update tanks ";
    Game::update_tanks(rocket_red, rocket_blue, pool);
    pool.wait_for_tasks();

    // Calculate convex hull for 'rocket barrier' with the tank_positions collected before.
    // Using the Graham scan algorithm.
    forcefield_hull.clear();
    forcefield_hull = convex_hull(tank_positions);
    
    // Update rockets
    cout << "Update rockets ";
    Game::update_rockets(smoke, explosion, pool);
    pool.wait_for_tasks();

    cout << "Update particle beams ";
    //Update particle beams
    Game::update_particle_beams(smoke);

    //Update smoke plumes
    for (Smoke& smoke : smokes) {
        {
            smoke.tick();
        }
    }

    //Update explosion sprites and remove when done with remove erase idiom
    for (Explosion& explosion : explosions)
    {
        explosion.tick();
    }

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());

    cout << "Remove rockets ";
    if (rocket_destroyed_this_round) {
        //Remove exploded rockets with remove erase idiom
        rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());
    }

    cout << "Remove inactive tanks ";
    // Remove inactive tanks from cell
    for (int tank_index : inactive_tank_indices) {
        Tank& tank = tanks[tank_index];
        Cell::remove_tank_from_cell(tank, cells);
    }

    cout << "End" << endl;
}

// -----------------------------------------------------------
// Draw all sprites to the screen
// (It is not recommended to multi-thread this function)
// -----------------------------------------------------------
void Game::draw()
{
    // clear the graphics window
    screen->clear(0);

    //Draw background
    background_terrain.draw(screen);

    //Draw convex hull or cells placeholder

    //Draw sprites
    for (int i = 0; i < NUM_TANKS_BLUE + NUM_TANKS_RED; i++)
    {
        tanks.at(i).draw(screen);

        vec2 tank_pos = tanks.at(i).get_position();
    }

    for (Rocket& rocket : rockets)
    {
        rocket.draw(screen);
    }

    for (Smoke& smoke : smokes)
    {
        smoke.draw(screen);
    }

    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.draw(screen);
    }

    for (Explosion& explosion : explosions)
    {
        explosion.draw(screen);
    }

    // Draw the health bars with two threads.
    for (int team = 0; team < 2; team++) {
        initialize_team_health_bar(tanks, team, screen);
    }
}


// -----------------------------------------------------------
// When we reach max_frames print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void Tmpl8::Game::measure_performance()
{
    char buffer[128];
    if (frame_count >= max_frames)
    {
        if (!lock_update)
        {
            duration = perf_timer.elapsed();
            cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
            lock_update = true;

            //int totalTime = 0;
            //for (int time : times) {
            //    totalTime = totalTime + time;
            //}

            //int average = totalTime / times.size();
            //cout << "Duration of timed window is on average: " << average << " milliseconds" << endl;

            //auto t1 = high_resolution_clock::now();
            //pool.wait_for_tasks();
            //auto t2 = high_resolution_clock::now();
            //auto ms_int = duration_cast<milliseconds>(t2 - t1);
            //times.push_back(ms_int.count());
        }

        frame_count--;
    }

    if (lock_update)
    {
        screen->bar(420 + HEALTHBAR_OFFSET, 170, 870 + HEALTHBAR_OFFSET, 430, 0x030000);
        int ms = (int)duration % 1000, sec = ((int)duration / 1000) % 60, min = ((int)duration / 60000);
        sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
        frame_count_font->centre(screen, buffer, 200);
        sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
        frame_count_font->centre(screen, buffer, 340);
    }
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::tick(float deltaTime)
{
    if (!lock_update)
    {
        update(deltaTime);
    }

    draw();

    measure_performance();

    //Print frame count
    frame_count++;
    string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->print(screen, frame_count_string.c_str(), 350, 580);
}
