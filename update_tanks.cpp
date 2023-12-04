#include "precomp.h" // include (only) this in every .cpp file

// Todo: Add mutexes more specific place, to save time.
void Game::update_tanks(Sprite& rocket_red, Sprite& rocket_blue, BS::thread_pool& pool) {
    tank_positions.clear();
    inactive_tank_indices.clear();

    std::mutex mutex;
    std::mutex update_cell_mutex;
    std::mutex mutex2;
    std::size_t tanks_size = tanks.size();
    std::size_t number_of_threads = 4;
    std::size_t tanks_per_thread = tanks_size / number_of_threads;
    for (int thread_index = 0; thread_index < number_of_threads; ++thread_index) {
        std::size_t start_index = thread_index * tanks_per_thread;
        std::size_t end_index = (thread_index == number_of_threads - 1) ? tanks_size : (thread_index + 1) * tanks_per_thread;

        pool.push_task([=, &rocket_red, &rocket_blue, &update_cell_mutex] {
            Game::update_tank(start_index, end_index, update_cell_mutex);
        });
    }

    pool.wait_for_tasks();

    vector<int> outside_cells = Cell::initialize_outside_cell_indices(cells);

    for (int thread_index = 0; thread_index < number_of_threads; ++thread_index) {
        std::size_t start_index = thread_index * tanks_per_thread;
        std::size_t end_index = (thread_index == number_of_threads - 1) ? tanks_size : (thread_index + 1) * tanks_per_thread;

        pool.push_task([=, &rocket_red, &rocket_blue, &outside_cells, &mutex, &mutex2] {
            Game::run_tank_collision(start_index, end_index, outside_cells, rocket_red, rocket_blue, mutex2, mutex);
        });
    }

    pool.wait_for_tasks();

}

void Game::update_tank(int start_index, int end_index, mutex& update_cell_mutex) {
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

void Game::run_tank_collision(int start_index, int end_index, vector<int>& outside_cells, Sprite& rocket_red, Sprite& rocket_blue, mutex& mutex2, mutex& mutex) {
    for (std::size_t j = start_index; j < end_index; ++j) {
        Tank& tank = tanks[j];
        if (!tank.active) continue;

        check_tank_collision(tank);

        //Shoot at closest target if reloaded
        if (tank.rocket_reloaded())
        {
            std::lock_guard<std::mutex> lock(mutex);
            Tank& target = Game::find_closest_enemy(tank);
            rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, ROCKET_RADIUS, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));
            tank.reload_rocket();
        }

        // See if the current tank is positioned in one of the outside_cells. If so, add their position to tank_positions.
        // This is to prepare the convex_hull calculation.
        int cell_index = tank.cell_index;
        vector<int>::iterator outside_cell_iterator;
        outside_cell_iterator = std::find_if(outside_cells.begin(), outside_cells.end(), [cell_index](int outside_cell_index) { return cell_index == outside_cell_index; });
        if (outside_cell_iterator != outside_cells.end()) {
            std::lock_guard<std::mutex> lock(mutex);
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
