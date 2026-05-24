#include "CubiertoLucha.hpp"
#include "LevantarseLucha.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"

namespace IVJ
{
    CubiertoLucha::CubiertoLucha()
        : FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
          max_frames{1}, id_frame{0},
          max_frame_time{0.25f}, cur_frame_time{0.25f}
    {
        nombre = "CubiertoLucha";
    }

    FSM* CubiertoLucha::onInputs(const CE::IControl& control)
    {
        (void)control;
        return nullptr;
    }

    void CubiertoLucha::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        cur_frame_time = max_frame_time;

        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 8 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1;

        auto control = obj.getComponente<CE::IControl>();
        if(control) control->setActivo(false);
    }

    void CubiertoLucha::onSalir(const Entidad& obj)
    {
        auto control = obj.getComponente<CE::IControl>();
        if(control) control->setActivo(true);
    }

    void CubiertoLucha::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        cur_frame_time -= dt;
        if(cur_frame_time <= 0) {
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % max_frames), 8 * s_h),
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
