#include"AttackBoss2.hpp"
#include"IdleBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"
#include"Motor/Utils/Vector2D.hpp"
#include<cmath>

namespace IVJ
{

    AttackBoss2::AttackBoss2(int max_frames, float frame_rate)
        :FSMBoss{},
        max_frames{max_frames},
        id_frame{0},
        max_frame_time{frame_rate},
        cur_frame_time{frame_rate},
        t_w{0}, t_h{0},
        animacion_terminada{false},
        golpe_registrado{false},
        hit_frame{1}
    {
        //muy importante el nombre es el que vamos utilizar para nuestro facade
        nombre = "AttackBoss2";
    }
    FSM* AttackBoss2::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)target;
        (void)parent;

        //solo salir cuando la animación terminó
        if(animacion_terminada)
            return new IdleBoss2(2, 0.7f);

        return nullptr;
    }
    void AttackBoss2::onEntrar(const Entidad& obj)
    {
        auto sprite = obj.getComponente<CE::ISprite>();
        t_w = sprite->width;
        t_h = sprite->height;
        id_frame = 0;
        animacion_terminada = false;
        golpe_registrado = false;

        //NO activar esta_atacando aquí; se activa en el hit_frame

        //mostrar primer frame inmediatamente
        sprite->m_sprite.setTextureRect(
            sf::IntRect(
                {0, 3*t_h},
                {t_w, t_h}
            )
        );
        id_frame = 1;
    }
    void AttackBoss2::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
            //cooldown para que no ataque de nuevo inmediatamente
            combate->cooldown_ataque = 0.4f;
        }
    }
    void AttackBoss2::onUpdate(const Entidad& obj,float dt)
    {
        cur_frame_time -= dt;

        if (cur_frame_time <=0)
        {
            if(id_frame >= max_frames) {
                animacion_terminada = true;
                //asegurar que esta_atacando se desactive al terminar
                auto combate = obj.getComponente<ICombate>();
                if(combate) combate->esta_atacando = false;
                return;
            }

            //activar el golpe en el hit_frame
            if(id_frame == hit_frame && !golpe_registrado) {
                auto combate = obj.getComponente<ICombate>();
                if(combate) {
                    combate->esta_atacando = true;
                    combate->tipo_ataque = 1; //golpe
                }
                golpe_registrado = true;
            }

            obj.getComponente<CE::ISprite>()->m_sprite.setTextureRect(
                    sf::IntRect(
                         {//pos
                          t_w*(id_frame%max_frames),
                          3*t_h
                         },
                         {//dimensiones
                            t_w,
                            t_h
                         }
                    )
            );
            cur_frame_time = max_frame_time;
            id_frame++;
        }
    }
}