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
            return new LevantarseBoss(6, 0.15f);
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
        
        m_ente->setPosicion(nx, ny);
    }
}
