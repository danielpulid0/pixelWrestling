#include "RebotePinBoss.hpp"
#include "LevantarseBoss.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace IVJ
{
    RebotePinBoss::RebotePinBoss(float start_x, float start_y, float dir_x)
        : FSMBoss{}, sprite{nullptr}, t_w{0}, t_h{0},
          start_x{start_x}, start_y{start_y}, dir_x{dir_x}, 
          timer{0.f}, max_time{0.4f}, m_ente{nullptr}
    {
        nombre = "RebotePinBoss";
    }

    FSM* RebotePinBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        if(timer >= max_time) {
            return new LevantarseBoss(7, 0.15f);
        }
        return nullptr;
    }

    void RebotePinBoss::onEntrar(const Entidad& obj)
    {
        m_ente = const_cast<Entidad*>(&obj);
        sprite = &m_ente->getComponente<CE::ISprite>()->m_sprite;
        t_w = m_ente->getComponente<CE::ISprite>()->width;
        t_h = m_ente->getComponente<CE::ISprite>()->height;

        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 6 * t_h),
                sf::Vector2i(t_w, t_h)
            )
        );
    }

    void RebotePinBoss::onSalir(const Entidad& obj)
    {
        (void)obj;
    }

    void RebotePinBoss::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        timer += dt;
        if (timer > max_time) timer = max_time;
        
        float t = timer / max_time; // 0.0 a 1.0
        float altura = 40.f; // altura del rebote
        
        float nx = start_x + (dir_x * 60.f * t); // se mueve 60 pixeles hacia la direccion
        float ny = start_y - (altura * std::sin(M_PI * t)); // parabola
        
        // Limitar dentro de los límites del ring (incluyendo hitbox del jefe)
        float half_w = 17.5f;
        float half_h = 44.f;
        auto bbox = m_ente->getComponente<CE::IBoundingBox>();
        if (bbox) {
            half_w = bbox->mitad.x;
            half_h = bbox->mitad.y;
        }

        // Límites calculados a partir de las paredes del ring:
        // Pared Izq (x=90, w=20) -> Borde = 100
        // Pared Der (x=590, w=20) -> Borde = 580
        // Pared Sup (y=126, h=10) -> Borde = 131
        // Pared Inf (y=296, h=10) -> Borde = 291
        float min_x = 100.f + half_w;
        float max_x = 580.f - half_w;
        float min_y = 131.f + half_h;
        float max_y = 291.f - half_h;

        if (nx < min_x) nx = min_x;
        if (nx > max_x) nx = max_x;
        if (ny < min_y) ny = min_y;
        if (ny > max_y) ny = max_y;
        
        m_ente->setPosicion(nx, ny);
    }
}
