#include "RebotePinLucha.hpp"
#include "LevantarseLucha.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace IVJ
{
    RebotePinLucha::RebotePinLucha(float start_x, float start_y, float dir_x)
        : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, 
          start_x{start_x}, start_y{start_y}, dir_x{dir_x}, 
          timer{0.f}, max_time{0.4f}, m_ente{nullptr}
    {
        nombre = "RebotePinLucha";
    }

    FSM* RebotePinLucha::onInputs(const CE::IControl& control)
    {
        (void)control;
        if(timer >= max_time) {
            return new LevantarseLucha(7, 0.15f);
        }
        return nullptr;
    }

    void RebotePinLucha::onEntrar(const Entidad& obj)
    {
        m_ente = const_cast<Entidad*>(&obj);
        sprite = &m_ente->getComponente<CE::ISprite>()->m_sprite;
        s_w = m_ente->getComponente<CE::ISprite>()->width;
        s_h = m_ente->getComponente<CE::ISprite>()->height;
        
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 7 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
    }

    void RebotePinLucha::onSalir(const Entidad& obj)
    {
        (void)obj;
    }

    void RebotePinLucha::onUpdate(const Entidad& obj, float dt)
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
