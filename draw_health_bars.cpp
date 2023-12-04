#include "precomp.h" // include (only) this in every .cpp file

void initialize_team_health_bar(vector<Tank> &tanks, int team, Surface* screen) {
    vector<int> team_healths = sort_tank_team_healths(tanks, team);
    draw_health_bars(team_healths, team, screen);
}

vector<int> sort_tank_team_healths(vector<Tank>& tanks, int team) {
    vector<int> team_healths;
    team_healths.clear();

    // Fill team_healths with the health of current team.
    if (team == 0) {
        team_healths.reserve(NUM_TANKS_BLUE);
        for (auto it = tanks.begin(); it != tanks.begin() + NUM_TANKS_BLUE; ++it) {
            if (it->active) { team_healths.emplace_back(it->health); }
        }
    }
    else {
        team_healths.reserve(NUM_TANKS_RED);
        for (auto it = tanks.begin() + NUM_TANKS_RED; it != tanks.end(); ++it) {
            if (it->active) { team_healths.emplace_back(it->health); }
        }
    }

    // The sort() function uses a 3 fold hybrid sorting technique named Introsort. It is a combination of Quick Sort, Heap Sort, and Insertion Sort
    sort(team_healths.begin(), team_healths.end());

    return team_healths;
}

void draw_health_bars(const std::vector<int>& sorted_healths, const int team, Surface* screen) {
    int health_bar_start_x = (team < 1) ? 0 : (SCRWIDTH - HEALTHBAR_OFFSET) - 1;
    int health_bar_end_x = (team < 1) ? HEALTH_BAR_WIDTH : health_bar_start_x + HEALTH_BAR_WIDTH - 1;

    for (int i = 0; i < SCRHEIGHT - 1; i++)
    {
        //Health bars are 1 pixel each
        int health_bar_start_y = i * 1;
        int health_bar_end_y = health_bar_start_y + 1;

        screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
    }

    //Draw the <SCRHEIGHT> least healthy tank health bars
    int draw_count = std::min(SCRHEIGHT, (int)sorted_healths.size());
    for (int i = 0; i < draw_count - 1; i++)
    {
        //Health bars are 1 pixel each
        int health_bar_start_y = i * 1;
        int health_bar_end_y = health_bar_start_y + 1;
        float health_fraction = (1 - ((double)sorted_healths.at(i) / (double)TANK_MAX_HEALTH));

        if (team == 0) { screen->bar(health_bar_start_x + (int)((double)HEALTH_BAR_WIDTH * health_fraction), health_bar_start_y, health_bar_end_x, health_bar_end_y, GREENMASK); }
        else { screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x - (int)((double)HEALTH_BAR_WIDTH * health_fraction), health_bar_end_y, GREENMASK); }
    }
}