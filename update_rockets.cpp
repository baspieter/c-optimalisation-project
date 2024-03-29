#include "precomp.h" // include (only) this in every .cpp file

void Game::update_rocket(Sprite& explosion, Sprite& smoke, int start_index, int end_index) {
    for (std::size_t j = start_index; j < end_index; ++j) {
        Rocket& rocket = rockets[j];

        rocket.tick();

        // Find surrounding cells.
        int rocket_col = rocket.position.x / CELL_WIDTH;
        int rocket_row = rocket.position.y / CELL_HEIGHT;
        // Using the same function as for tanks, but now given the rocket position.

        vector<Cell> surrounding_rocket_cells = Game::find_surrounding_cells(rocket_col, rocket_row);

        //Check if rocket collides with enemy tank, spawn explosion, and if tank is destroyed spawn a smoke plume

        for (const Cell& surrounding_rocket_cell : surrounding_rocket_cells) {
            for (int tank_index : surrounding_rocket_cell.tank_indices) {
                Tank& tank = tanks.at(tank_index);
                if (!tank.active) continue;

                if ((tank.allignment != rocket.allignment) && rocket.intersects(tank.position, tank.collision_radius))
                {
                    {
                        std::lock_guard<std::mutex> lock(tank_explosion_mutex);
                        explosions.push_back(Explosion(&explosion, tank.position));
                    }

                    if (tank.hit(ROCKET_HIT_VALUE))
                    {
                        std::lock_guard<std::mutex> lock(smokes_mutex);
                        smokes.push_back(Smoke(smoke, tank.position - vec2(7, 24)));
                    }

                    if (!tank.active) {
                        std::lock_guard<std::mutex> lock(inactive_tanks_mutex);
                        inactive_tank_indices.push_back(tank.index);
                    }
                    else {
                        for (size_t i = 0; i < forcefield_hull.size(); i++)
                        {
                            if (circle_segment_intersect(forcefield_hull.at(i), forcefield_hull.at((i + 1) % forcefield_hull.size()), rocket.position, rocket.collision_radius))
                            {
                                std::lock_guard<std::mutex> lock(rocket_explosion_mutex);
                                explosions.push_back(Explosion(&explosion, rocket.position));
                                rocket.active = false;
                            }
                        }
                    }

                    rocket.active = false;
                    {
                        std::lock_guard<std::mutex> lock(destroyed_rockets_mutex);
                        if (!rocket_destroyed_this_round) rocket_destroyed_this_round = true;
                    }
                    break;
                }
            }
        }
    }
}