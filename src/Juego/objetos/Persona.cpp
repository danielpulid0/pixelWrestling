#include "Persona.hpp"
#include <Juego/Componentes/IJComponentes.hpp>

namespace IVJ {

    Persona::Persona(int grid_x, int grid_y, float world_x, float world_y) 
        : Rectangulo(38.0f, 38.0f, sf::Color::Transparent, sf::Color(30, 30, 80)) {
        
        setPosicion(world_x, world_y);

        addComponente(std::make_shared<IEstadoEpidemia>(grid_x, grid_y));
    }

    void Persona::onUpdate(float dt) {
        Rectangulo::onUpdate(dt);
    }

}
