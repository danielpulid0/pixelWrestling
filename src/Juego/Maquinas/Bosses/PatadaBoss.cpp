#include "PatadaBoss.hpp"
#include "IdleBoss2.hpp"
#include "Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    PatadaBoss::PatadaBoss(int max_frames, float frame_rate)
        :FSMBoss{}, sprite{nullptr}, t_w{0}, t_h{0},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate}
    {
        nombre = "PatadaBoss";
    }

    FSM* PatadaBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        if(animacion_terminada)
            return new IdleBoss2(2, 0.2f);
        return nullptr;
    }

    void PatadaBoss::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        t_w = obj.getComponente<CE::ISprite>()->width;
        t_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;

        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = true;
            combate->tipo_ataque = 2; // patada
            combate->cooldown_ataque = 1.0f; // cooldown general después de atacar
        }

        //mostrar primer frame (row 5 = 4 * t_h)
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 4 * t_h),
                sf::Vector2i(t_w, t_h)
            )
        );
        id_frame = 1;
    }

    void PatadaBoss::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
        }
    }

    void PatadaBoss::onUpdate(const Entidad& obj, float dt)
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
                    sf::Vector2i(t_w * (id_frame % max_frames), 4 * t_h),
                    sf::Vector2i(t_w, t_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
