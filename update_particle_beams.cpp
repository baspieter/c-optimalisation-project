#include "precomp.h" // include (only) this in every .cpp file

void Game::update_particle_beams(Sprite& smoke) {
    for (Particle_beam& particle_beam : particle_beams)
    {
        Game::update_particle_beam(particle_beam, smoke);
    }
}


void Game::update_particle_beam(Particle_beam& particle_beam, Sprite& smoke) {
    particle_beam.tick(tanks);

    //Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
    // Iterate through all cells and their tanks.
    for (Cell& cell : cells) {
        for (int tank_index : cell.tank_indices) {
            Tank& tank = tanks.at(tank_index);
            if (!tank.active) continue;

            if (particle_beam.rectangle.intersects_circle(tank.get_position(), tank.get_collision_radius()))
            {
                if (tank.hit(particle_beam.damage))
                {
                    smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
                }

                // If tank is no longer active, remove from cell.
                if (!tank.active) {
                    inactive_tank_indices.push_back(tank.index);
                }
            }
        }
    }
}
