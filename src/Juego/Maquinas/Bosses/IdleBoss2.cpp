#include"IdleBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"
#include "AttackBoss2.hpp"
#include "PerseguirBoss.hpp"
#include <cmath>

namespace IVJ
{

    IdleBoss2::IdleBoss2(int max_frames, float frame_rate)
        :FSMBoss{},
        max_frames{max_frames},
        id_frame{0},
        max_frame_time{frame_rate},
        cur_frame_time{frame_rate}
    {
        //muy importante el nombre es el que vamos utilizar para nuestro facade
        nombre = "IdleBoss2";
    }
    FSM* IdleBoss2::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        auto pos = parent.getTransformada()->posicion;
        float dx = target.x - pos.x;
        float dy = target.y - pos.y;
        float distancia_x = std::abs(dx);
        float distancia_y = std::abs(dy);

        //voltear sprite hacia el jugador
        if(parent.getComponente<CE::ISprite>()) {
            if(dx < 0)
                parent.getComponente<CE::ISprite>()->m_sprite.setScale({1.f, 1.f});
            else
                parent.getComponente<CE::ISprite>()->m_sprite.setScale({-1.f, 1.f});
        }

        //respetar cooldown de ataque
        auto combate = parent.getComponente<ICombate>();
        if(combate && combate->cooldown_ataque > 0) {
            combate->cooldown_ataque -= 0.016f;
            //perseguir mientras espera cooldown si está lejos o desalineado
            if((distancia_x > 100.f && distancia_x < 200.f) || distancia_y > 2.f)
                return new PerseguirBoss(3, 0.25f, 60.f);
            return nullptr;
        }

        //en rango de ataque (cerca en X e Y)
        if(distancia_x < 40.f && distancia_y < 20.f)
            return new AttackBoss2(2, 0.18f);

        //en rango de persecución o necesita alinearse en Y → perseguir
        if(distancia_x < 200.f || distancia_y > 2.f)
            return new PerseguirBoss(3, 0.25f, 60.f);
        
        return nullptr;
    }
    void IdleBoss2::onEntrar(const Entidad& obj)
    {
        auto sprite = obj.getComponente<CE::ISprite>();
        t_w = sprite->width;
        t_h = sprite->height;
    }
    void IdleBoss2::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void IdleBoss2::onUpdate(const Entidad& obj,float dt)
    {
        cur_frame_time -= dt;

        if (cur_frame_time <=0)
        {
            obj.getComponente<CE::ISprite>()->m_sprite.setTextureRect(
                    sf::IntRect(
                        {//pos
                            t_w*(id_frame%max_frames),
                            0*t_h //primer renglon
                        },
                        {//dimensinones
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