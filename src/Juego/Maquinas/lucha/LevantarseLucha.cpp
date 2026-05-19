#include "LevantarseLucha.hpp"
#include "IdleLucha.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"

namespace IVJ
{
    LevantarseLucha::LevantarseLucha(int max_frames, float frame_rate)
        : FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
          max_frames{max_frames}, id_frame{0},
          max_frame_time{frame_rate}, cur_frame_time{frame_rate},
          animacion_terminada{false}
    {
        nombre = "LevantarseLucha";
    }

    FSM* LevantarseLucha::onInputs(const CE::IControl& control)
    {
        (void)control;
        if(animacion_terminada)
            return new IdleLucha(2, 0.12f);
        return nullptr;
    }

    void LevantarseLucha::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;

        // fila 8 (levantarse) - row index 7
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 7 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1;
    }

    void LevantarseLucha::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_derribado = false;
            combate->en_levantarse = false;
        }
    }

    void LevantarseLucha::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        cur_frame_time -= dt;
        if(cur_frame_time <= 0)
        {
            if(id_frame >= max_frames) {
                animacion_terminada = true;
                return;
            }
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % max_frames), 7 * s_h),
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
