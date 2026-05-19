#include"PatadaLigera.hpp"
#include"IdleLucha.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    PatadaLigera::PatadaLigera(int max_frames, float frame_rate)
        :FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate}
    {
        nombre = "PatadaLigera";
    }

    FSM* PatadaLigera::onInputs(const CE::IControl& control)
    {
        (void)control;
        if(animacion_terminada)
            return new IdleLucha(2, 0.12f);
        return nullptr;
    }

    void PatadaLigera::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;

        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = true;
            combate->tipo_ataque = 2; //patada
        }

        //consumir el input
        auto control = obj.getComponente<CE::IControl>();
        if(control) control->kick = false;

        //mostrar primer frame inmediatamente
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 4 * s_h), //row 5 (patada)
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1; // Pasar al 1
    }

    void PatadaLigera::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
        }
    }

    void PatadaLigera::onUpdate(const Entidad& obj, float dt)
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
                    sf::Vector2i(s_w * (id_frame % max_frames), 4 * s_h), //row 3
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
