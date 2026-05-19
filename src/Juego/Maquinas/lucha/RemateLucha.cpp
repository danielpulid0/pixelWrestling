#include"RemateLucha.hpp"
#include"IdleLucha.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    RemateLucha::RemateLucha(int max_frames, float frame_rate)
        :FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate}
    {
        nombre = "RemateLucha";
    }

    FSM* RemateLucha::onInputs(const CE::IControl& control)
    {
        (void)control;
        if(animacion_terminada)
            return new IdleLucha(2, 0.12f);
        return nullptr;
    }

    void RemateLucha::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;

        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = true;
            combate->tipo_ataque = 4; //remate
        }
        //consumir momentum
        auto momentum = obj.getComponente<IMomentum>();
        if(momentum) {
            momentum->valor = 0.f;
            momentum->remate_disponible = false;
        }

        //consumir el input
        auto control = obj.getComponente<CE::IControl>();
        if(control) control->finisher = false;

        //mostrar primer frame inmediatamente (row 13)
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 12 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1;
    }

    void RemateLucha::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
        }
    }

    void RemateLucha::onUpdate(const Entidad& obj, float dt)
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
                    sf::Vector2i(s_w * (id_frame % max_frames), 12 * s_h), //fila 11 (finisher)
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
