#include "precomp.h" // include (only) this in every .cpp file



// -----------------------------------------------------------
// PERFORMANCE IDEAS
// - Remove cell & tank index. Make them link together by position. (Calculation to col/row)
// -----------------------------------------------------------



// -----------------------------------------------------------
// GAME ORDER
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------



// -----------------------------------------------------------
// DRAW ALL CELLS
//for (Cell cell : cells) {
//    
//    int column = cell.column;
//    int row = cell.row;

//    vec2 line_left = vec2((column * CELL_WIDTH), (row * CELL_HEIGHT));
//    vec2 line_right = vec2((column * CELL_WIDTH + CELL_WIDTH), (row * CELL_HEIGHT));
//    line_left.x += HEALTHBAR_OFFSET;
//    line_right.x += HEALTHBAR_OFFSET;
//    screen->line(line_left, line_right, 0x0000ff);
//    screen->print((std::string{ "Cell index: " } + std::to_string(cell.index)).c_str(), line_left.x, line_right.y, 0x111111);

//    vec2 line_left2 = vec2((column * CELL_WIDTH), (row * CELL_HEIGHT + CELL_HEIGHT));
//    vec2 line_right2 = vec2((column * CELL_WIDTH + CELL_WIDTH), (row * CELL_HEIGHT + CELL_HEIGHT));
//    line_left2.x += HEALTHBAR_OFFSET;
//    line_right2.x += HEALTHBAR_OFFSET;
//    screen->line(line_left2, line_right2, 0x0000ff);

//    screen->print((std::string{ "Tanks: " } + std::to_string(cell.tank_indices.size())).c_str(), line_left.x, line_right.y + 50, 0x000000);

//    vec2 line_top = vec2((column * CELL_WIDTH), (row * CELL_HEIGHT));
//    vec2 line_bottom = vec2((column * CELL_WIDTH), (row * CELL_HEIGHT + CELL_HEIGHT));
//    line_top.x += HEALTHBAR_OFFSET;
//    line_bottom.x += HEALTHBAR_OFFSET;
//    screen->line(line_top, line_bottom, 0x0000ff);

//    vec2 line_top2 = vec2((column * CELL_WIDTH + CELL_WIDTH), (row * CELL_HEIGHT));
//    vec2 line_bottom2 = vec2((column * CELL_WIDTH + CELL_WIDTH), (row * CELL_HEIGHT + CELL_HEIGHT));
//    line_top2.x += HEALTHBAR_OFFSET;
//    line_bottom2.x += HEALTHBAR_OFFSET;
//    screen->line(line_top2, line_bottom2, 0x0000ff);
//}
// -----------------------------------------------------------



// -----------------------------------------------------------
// DRAW FORCEFIELD
//for (size_t i = 0; i < forcefield_hull.size(); i++)
//{
//    vec2 line_start = forcefield_hull.at(i);
//    vec2 line_end = forcefield_hull.at((i + 1) % forcefield_hull.size());
//    line_start.x += HEALTHBAR_OFFSET;
//    line_end.x += HEALTHBAR_OFFSET;
//    screen->line(line_start, line_end, 0x0000ff);
//}
// -----------------------------------------------------------



// -----------------------------------------------------------
// MEASURE PERFORMANCE
//using std::chrono::high_resolution_clock;
//using std::chrono::duration_cast;
//using std::chrono::duration;
//using std::chrono::microseconds;
//auto t1 = high_resolution_clock::now();
//auto t2 = high_resolution_clock::now();
//auto ms_int = duration_cast<microseconds>(t2 - t1);
//std::cout << ms_int.count() << "microseconds\n";
// -----------------------------------------------------------