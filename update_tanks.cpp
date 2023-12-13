#include "precomp.h" // include (only) this in every .cpp file

void Game::update_tank(int start_index, int end_index) {
    for (std::size_t j = start_index; j < end_index; ++j) {
        Tank& tank = tanks[j];
        if (!tank.active) continue;

        //Move tanks according to speed and nudges (see above) also reload
        tank.tick(background_terrain);
        {
            std::lock_guard<std::mutex> lock(update_cell_mutex);
            Cell::check_or_update_cell(tank, cells, tanks);
        }
    }
}

void Game::run_tank_collision(int start_index, int end_index, vector<int>& outside_cells, Sprite& rocket_red, Sprite& rocket_blue) {
    for (std::size_t j = start_index; j < end_index; ++j) {
        Tank& tank = tanks[j];
        if (!tank.active) continue;

        check_tank_collision(tank);

        //Shoot at closest target if reloaded
        if (tank.rocket_reloaded())
        {
            Tank& target = Game::find_closest_enemy(tank);
            tank.reload_rocket();
            {
                std::lock_guard<std::mutex> lock(rocket_reload_mutex);
                rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, ROCKET_RADIUS, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));
            }
        }

        // See if the current tank is positioned in one of the outside_cells. If so, add their position to tank_positions.
        // This is to prepare the convex_hull calculation.
        int cell_index = tank.cell_index;
        vector<int>::iterator outside_cell_iterator;
        outside_cell_iterator = std::find_if(outside_cells.begin(), outside_cells.end(), [cell_index](int outside_cell_index) { return cell_index == outside_cell_index; });
        if (outside_cell_iterator != outside_cells.end()) {
            std::lock_guard<std::mutex> lock(tank_positions_mutex);
            tank_positions.push_back(tanks.at(tank.index).position);
        }
    }
}

// -----------------------------------------------------------
// Iterates through all tanks and returns the closest enemy tank for the given tank
// @note I can't use the grid system because it always needs to find a new tank.
// -----------------------------------------------------------
Tank& Game::find_closest_enemy(Tank& current_tank)
{

    float closest_distance = numeric_limits<float>::infinity();
    int closest_index = 0;

    for (int i = 0; i < tanks.size(); i++)
    {
        if (tanks.at(i).allignment != current_tank.allignment && tanks.at(i).active)
        {
            float sqr_dist = fabsf((tanks.at(i).get_position() - current_tank.get_position()).sqr_length());
            if (sqr_dist < closest_distance)
            {
                closest_distance = sqr_dist;
                closest_index = i;
            }
        }
    }

    return tanks.at(closest_index);
}
