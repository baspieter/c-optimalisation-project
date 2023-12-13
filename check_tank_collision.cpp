#include "precomp.h" // include (only) this in every .cpp file

void Game::check_tank_collision(Tank& current_tank) {
    // Find the cell the tank is currently in.
    Cell& tank_cell = cells.at(current_tank.cell_index);
    // Find the surrounding cells.
    vector<Cell> surrounding_cells = Game::find_surrounding_cells(tank_cell.column, tank_cell.row);


    // Iterate through the current and surrounding cells.
    for (Cell& cell : surrounding_cells)
    {
        for (int tank_index : cell.tank_indices)
        {
            Tank& tank = tanks.at(tank_index);
            if (!tank.active || &current_tank == &tank) continue;

            vec2 dir = current_tank.get_position() - tank.get_position();
            float dir_squared_len = dir.sqr_length();

            float col_squared_len = (current_tank.get_collision_radius() + tank.get_collision_radius());
            col_squared_len *= col_squared_len;

            if (dir_squared_len < col_squared_len)
            {
                current_tank.push(dir.normalized(), 1.f);
            }
        }
    }
}

vector<Cell> Game::find_surrounding_cells(int col, int row)
{
    vector<Cell> surrounding_cells;

    // Tank original position.
    int tank_col_or = col;
    int tank_row_or = row;

    // Tank most left top cell.
    int tank_col = tank_col_or - 1;
    int tank_row = tank_row_or - 1;
 
    // Start on the left top, keep updating the column until 3 and 9. In these cases we reset the column & add one to the row.
    // At te end we return all surrounding and current cells.
    for (int j = 0;j < 9;++j) {
        if (j == 3 || j == 6) {
            ++tank_row;
            tank_col = tank_col_or - 1;
        }

        if (tank_row < 0 || tank_col < 0) continue;

        vector<Cell>::iterator cell_iterator;
        cell_iterator = std::find_if(cells.begin(), cells.end(), [&tank_col, &tank_row](Cell& cell) { return cell.column == tank_col && cell.row == tank_row; });
        if (cell_iterator != cells.end()) {
            // @note: This does not require a mutex, because it's not an global vector. Just within the thread. So it can't be updated at the same time.
            surrounding_cells.push_back((*cell_iterator));
        }

        tank_col++;
    }

    return surrounding_cells;
}