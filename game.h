#pragma once

namespace Tmpl8
{
//forward declarations
class Tank;
class Rocket;
class Smoke;
class Particle_beam;

class Game
{
  public:
    void set_target(Surface* surface) { screen = surface; }
    void init();
    void shutdown();
    void update(float deltaTime);
    void draw();
    void tick(float deltaTime);
    void measure_performance();

    // Tank update
    Tank& find_closest_enemy(Tank& current_tank);
    void check_tank_collision(Tank& current_tank);
    void run_tank_collision(int start_index, int end_index, vector<int>& outside_cells, Sprite& rocket_red, Sprite& rocket_blue);
    vector<int> get_tank_indices(Cell& cell);
    void update_tank(int start_index, int end_index);
    Tank& set_tank_target(Tank& tank);

    // Rocket update
    void update_rocket(Sprite& explosion, Sprite& smoke, int start_index, int end_index);


    // Update particle beams
    void update_particle_beam(Particle_beam& particle_beam, Sprite& smoke);

    // Helper methods
    vector<Cell> find_surrounding_cells(int col, int row);

    void mouse_up(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void mouse_down(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void mouse_move(int x, int y)
    { /* implement if you want to detect mouse movement */
    }

    void key_up(int key)
    { /* implement if you want to handle keys */
    }

    void key_down(int key)
    { /* implement if you want to handle keys */
    }

  private:
    Surface* screen;

    vector<Tank> tanks;
    vector<Rocket> rockets;
    vector<Smoke> smokes;
    vector<Explosion> explosions;
    vector<Particle_beam> particle_beams;
    vector<Cell> cells;
    vector<vec2> tank_positions;
    vector<int> inactive_tank_indices;

    Terrain background_terrain;
    vector<vec2> forcefield_hull;

    Font* frame_count_font;
    long long frame_count = 0;

    bool rocket_destroyed_this_round = false;

    bool lock_update = false;

    // Mutex declarations
    mutex update_cell_mutex;
    mutex destroyed_rockets_mutex;
    mutex rocket_reload_mutex;
    mutex tank_positions_mutex;
    mutex tank_explosion_mutex;
    mutex rocket_explosion_mutex;
    mutex smokes_mutex;
    mutex inactive_tanks_mutex;
};

}; // namespace Tmpl8