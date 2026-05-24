#include"IdleBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"
#include "AttackBoss2.hpp"
#include "PerseguirBoss.hpp"
#include "CubriendoBoss.hpp"
#include "../lucha/CubiertoLucha.hpp"
#include <cmath>
#include <cstdlib>
#include "PatadaBoss.hpp"
#include "RemateBoss.hpp"
#include "AtaqueSillaBoss.hpp"
#include "DefensaBoss.hpp"
#include <Juego/Escenas/EscenaMatch.hpp>

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
            float orientacion = (EscenaMatch::personaje_rival == 1) ? -1.f : 1.f;
            if(dx < 0)
                parent.getComponente<CE::ISprite>()->m_sprite.setScale({-orientacion, 1.f});
            else
                parent.getComponente<CE::ISprite>()->m_sprite.setScale({orientacion, 1.f});
        }

        // Si el jugador está derribado, ir a cubrirlo
        auto target_comp = parent.getComponente<ITarget>();
        if(target_comp && target_comp->target_obj) {
            auto player_combate = target_comp->target_obj->getComponente<ICombate>();
            if(player_combate && player_combate->esta_derribado) {
                if(distancia_x < 40.f && distancia_y < 20.f) {
                    auto entJ = dynamic_cast<Entidad*>(target_comp->target_obj);
                    if(entJ) {
                        entJ->setFSM(std::make_shared<CubiertoLucha>());
                    }
                    return new CubriendoBoss();
                } else {
                    return new PerseguirBoss(3, 0.25f, 60.f);
                }
            }
        }

        // Defensa aleatoria esporádica si está relativamente cerca
        if(distancia_x < 60.f && distancia_y < 30.f) {
            if(rand() % 100 < 2) { // 2% chance per frame
                return new DefensaBoss(0.5f + (rand()%10)/10.0f);
            }
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
        if(distancia_x < 40.f && distancia_y < 20.f) {
            // Remate
            auto momentum = parent.getComponente<IMomentum>();
            if(momentum && momentum->remate_disponible) {
                return new RemateBoss(3, 0.12f);
            }
            
            // Ataque Silla
            if(combate && combate->tiene_silla) {
                return new AtaqueSillaBoss(3, 0.12f);
            }

            // Patada vs Golpe (50/50)
            if(rand() % 2 == 0) {
                return new PatadaBoss(2, 0.14f);
            } else {
                return new AttackBoss2(2, 0.14f);
            }
        }

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