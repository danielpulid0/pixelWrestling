#pragma once
#include <Juego/Figuras/Figuras.hpp>

namespace IVJ {
    class Persona : public IVJ::Rectangulo {
    public:
        Persona(int grid_x, int grid_y, float world_x, float world_y);
        void onUpdate(float dt) override;
    };
}
