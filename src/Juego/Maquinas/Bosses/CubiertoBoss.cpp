#include "CubiertoBoss.hpp"
#include "LevantarseBoss.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"

namespace IVJ
{
    CubiertoBoss::CubiertoBoss()
        : FSMBoss{}, sprite{nullptr}, t_w{0}, t_h{0},
          max_frames{1}, id_frame{0},
          max_frame_time{0.25f}, cur_frame_time{0.25f}
    {
        nombre = "CubiertoBoss";
    }

    FSM* CubiertoBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        return nullptr;
    }

    void CubiertoBoss::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        t_w = obj.getComponente<CE::ISprite>()->width;
        t_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        cur_frame_time = max_frame_time;

        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 8 * t_h),
                sf::Vector2i(t_w, t_h)
            )
        );
        id_frame = 1;
    }

    void CubiertoBoss::onSalir(const Entidad& obj)
    {
        (void)obj;
    }

    void CubiertoBoss::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        cur_frame_time -= dt;
        if(cur_frame_time <= 0) {
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(t_w * (id_frame % max_frames), 8 * t_h),
                    sf::Vector2i(t_w, t_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
