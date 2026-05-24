#include "LevantarseBoss.hpp"
#include "IdleBoss2.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"

namespace IVJ
{
    LevantarseBoss::LevantarseBoss(int max_frames, float frame_rate)
        : sprite{nullptr}, s_w{0}, s_h{0},
          max_frames{max_frames}, id_frame{0},
          max_frame_time{frame_rate}, cur_frame_time{frame_rate},
          animacion_terminada{false}
    {
        nombre = "Boss_Levantarse";
    }

    FSM* LevantarseBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        if(animacion_terminada)
            return new IdleBoss2(2, 0.7f);
        return nullptr;
    }

    void LevantarseBoss::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;

        // Limitar max_frames al número real de columnas de la textura para evitar cuadros vacíos
        int texture_w = sprite->getTexture().getSize().x;
        int frames_in_texture = texture_w / s_w;
        if (max_frames > frames_in_texture) {
            max_frames = frames_in_texture;
        }

        // fila 8 (levantarse) - row index 7
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 7 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1;
    }

    void LevantarseBoss::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_derribado = false;
            combate->en_levantarse = false;
        }
    }

    void LevantarseBoss::onUpdate(const Entidad& obj, float dt)
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
