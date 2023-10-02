#include "precomp.h"
#include "cell.h"

namespace Tmpl8
{

    Cell::Cell(int index, int column, int row, vector<int> tank_indices) : index(index), column(column), row(row), tank_indices(tank_indices)
    {
    }

    Cell::~Cell()
    {
    }

    Cell* Cell::find_cell_for_tank(int pos_x, int pos_y, vector<Cell>& cells)
    {
        int tank_col = pos_x / CELL_WIDTH;
        int tank_row = pos_y / CELL_HEIGHT;

        vector<Cell>::iterator cell_iterator;
        cell_iterator = std::find_if(cells.begin(), cells.end(), [&tank_col, &tank_row](Cell& cell) { return cell.column == tank_col && cell.row == tank_row; });
        if (cell_iterator != cells.end()) {
            return &(*cell_iterator);
        }

        int cell_index = cells.size();
        Cell new_cell = Cell(cell_index, tank_col, tank_row, {});

        cells.push_back(new_cell);
        return &cells.back();
    }

    // Check and update current tank cell when the tank moves.
    void Cell::check_or_update_cell(Tank& tank, vector<Cell>& cells, vector<int>& outside_cells_indices, vector<Tank>& tanks)
    {
        int tank_col = tank.position.x / CELL_WIDTH;
        int tank_row = tank.position.y / CELL_HEIGHT;
        Cell& cell = cells.at(tank.cell_index);

        // Return when tank is still in the same cell. If so, return.
        if (cell.row == tank_row && cell.column == tank_col) return;

        // Remove tank from cell and find or create a new one.
        cell.tank_indices.erase(remove_if(cell.tank_indices.begin(), cell.tank_indices.end(), [tank](int tank_index) { return tank.index == tank_index; }), cell.tank_indices.end());
        Cell* new_cell = Cell::find_cell_for_tank(tank.position.x, tank.position.y, cells);
        tank.cell_index = new_cell->index;
        new_cell->tank_indices.push_back(tank.index);

        // If there are no tanks left in the cell, outside_cells_indices should be updated.
        if (!cell.tank_indices.empty()) return;

        outside_cells_indices.clear();
        outside_cells_indices = initialize_outside_cell_indices(cells);
    }

    void Cell::remove_tank_from_cell(int tank_index, int cell_index, vector<Cell>& cells, vector<int>& outside_cells_indices)
    {
        Cell& cell = cells.at(cell_index);
        cell.tank_indices.erase(std::remove(cell.tank_indices.begin(), cell.tank_indices.end(), tank_index), cell.tank_indices.end());

        // If there are no tanks left in the cell, outside_cells_indices should be updated.
        if (!cell.tank_indices.empty()) return;

        outside_cells_indices.clear();
        outside_cells_indices = initialize_outside_cell_indices(cells);
    }

    vector<int> Cell::initialize_outside_cell_indices(vector<Cell>& cells)
    {
        // Find the minimum and maximum row and column values in the cells vector
        int min_col = INT_MAX;
        int max_col = INT_MIN;
        int min_row = INT_MAX;
        int max_row = INT_MIN;

        for (const Cell& cell : cells) {
            if (cell.tank_indices.size() == 0) continue;

            if (cell.column < min_col) {
                min_col = cell.column;
            }
            if (cell.column > max_col) {
                max_col = cell.column;
            }
            if (cell.row < min_row) {
                min_row = cell.row;
            }
            if (cell.row > max_row) {
                max_row = cell.row;
            }
        }

        // Create vectors to hold the outside cells
        vector<int> outside_cells_indices;

        // Iterate over all the cells and identify the outside cells
        for (const auto& cell : cells) {
            if (cell.tank_indices.size() == 0) continue;

            if (cell.row == min_row || cell.column == min_col || cell.column == max_col || cell.row == max_row) {
                outside_cells_indices.push_back(cell.index);
            }
        }

        // Erase duplicates.
        auto it = std::unique(outside_cells_indices.begin(), outside_cells_indices.end());
        outside_cells_indices.erase(it, outside_cells_indices.end());

        return outside_cells_indices;
    }

} // namespace Tmpl8