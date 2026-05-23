#include"RemateBoss.hpp"
#include"IdleBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    RemateBoss::RemateBoss(int max_frames, float frame_rate)
        :FSMBoss{}, sprite{nullptr}, t_w{0}, t_h{0},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate}
    {
        nombre = "RemateBoss";
    }

    FSM* RemateBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        if(animacion_terminada)
            return new IdleBoss2(2, 0.2f);
        return nullptr;
    }

    void RemateBoss::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        t_w = obj.getComponente<CE::ISprite>()->width;
        t_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;

        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = true;
            combate->tipo_ataque = 4; //remate
            combate->cooldown_ataque = 2.0f; // mayor cooldown después de remate
        }

        auto momentum = obj.getComponente<IMomentum>();
        if(momentum) {
            momentum->valor = 0.f;
            momentum->remate_disponible = false;
        }

        //mostrar primer frame (row 13 = 12 * t_h)
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 12 * t_h),
                sf::Vector2i(t_w, t_h)
            )
        );
        id_frame = 1;
    }

    void RemateBoss::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
        }
    }

    void RemateBoss::onUpdate(const Entidad& obj, float dt)
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
                    sf::Vector2i(t_w * (id_frame % max_frames), 12 * t_h),
                    sf::Vector2i(t_w, t_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
