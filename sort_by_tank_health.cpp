#include "precomp.h" // include (only) this in every .cpp file


// partition krijgt telkens een kant van de boom.
// Wisselt tanks op basis van de laatste tank. Links lager, rechts hoger.
// Hierdoor staat de laatste tank op de juiste plek.
// Stuurt de positie van de tank die op de juiste plek staat terug.
int partition(vector<int>& healths, int start, int end) {

    int last = end;
    int first = start;
    for (int i = start; i < end; ++i) {
        if (healths[i] < healths[last]) {
            swap(healths[i], healths[first]);
            ++first;
        }
    }
    swap(healths[first], healths[last]);
    return first;

}

// sort_tank_by_health krijgt de tank die op de juiste plek staat terug.
// Splitst het in twee kanten en laat die weer door partition uitvoeren.
// Op de gegeven moment zijn er alleen maar boom waardes met 1 value.
// Die worden achter elkaar aan gezet, wat een gesorteerde lijs creert.
void sort_tanks_by_health(vector<int>& healths, int start, int end) {
    if (start < end) {
        int p = partition(healths, start, end);
        sort_tanks_by_health(healths, start, p - 1);
        sort_tanks_by_health(healths, p + 1, end);
    }
}