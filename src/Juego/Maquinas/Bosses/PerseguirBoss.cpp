#include"PerseguirBoss.hpp"
#include"IdleBoss2.hpp"
#include"AttackBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"
#include<cmath>

namespace IVJ
{
    PerseguirBoss::PerseguirBoss(int max_frames, float frame_rate, float velocidad)
        :FSMBoss{},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate},
        t_w{0}, t_h{0}, velocidad{velocidad}
    {
        nombre = "PerseguirBoss";
    }

    FSM* PerseguirBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        auto pos = parent.getTransformada()->posicion;
        float dx = target.x - pos.x;
        float dy = target.y - pos.y;
        float distancia_x = std::abs(dx);
        float distancia_y = std::abs(dy);

        //seguir al jugador en Y de forma suave (interpolación)
        if(distancia_y > 2.f) {
            float dir_y = (dy > 0) ? 1.f : -1.f;
            parent.getTransformada()->posicion.y += dir_y * velocidad * 0.016f;
        }

        //si está en rango de ataque (muy cerca en X y Y), atacar
        //pero respetar cooldown
        auto combate = parent.getComponente<ICombate>();
        bool en_cooldown = (combate && combate->cooldown_ataque > 0);
        if(en_cooldown) combate->cooldown_ataque -= 0.016f;

        if(distancia_x < 40.f && distancia_y < 20.f && !en_cooldown)
            return new AttackBoss2(2, 0.18f);

        //si sale del rango de persecución en X Y ya está alineado en Y, volver a idle
        if(distancia_x > 200.f && distancia_y <= 2.f)
            return new IdleBoss2(2, 0.7f);

        //perseguir en X hacia el jugador SOLO si está en el rango original deseado
        if (distancia_x > 2.f && distancia_x <= 200.f) {
            float dir_x = (dx > 0) ? 1.f : -1.f;
            parent.getTransformada()->posicion.x += dir_x * velocidad * 0.016f;
        }

        //voltear sprite hacia el jugador
        if(parent.getComponente<CE::ISprite>()) {
            if(dx < 0)
                parent.getComponente<CE::ISprite>()->m_sprite.setScale({1.f, 1.f});
            else
                parent.getComponente<CE::ISprite>()->m_sprite.setScale({-1.f, 1.f});
        }

        return nullptr;
    }

    void PerseguirBoss::onEntrar(const Entidad& obj)
    {
        auto sprite = obj.getComponente<CE::ISprite>();
        t_w = sprite->width;
        t_h = sprite->height;
        id_frame = 0;
    }

    void PerseguirBoss::onSalir(const Entidad& obj)
    {
        (void)obj;
    }

    void PerseguirBoss::onUpdate(const Entidad& obj, float dt)
    {
        cur_frame_time -= dt;
        if(cur_frame_time <= 0)
        {
            //animación de caminar (row 1)
            obj.getComponente<CE::ISprite>()->m_sprite.setTextureRect(
                sf::IntRect(
                    {t_w * (id_frame % max_frames), 1 * t_h},
                    {t_w, t_h}
                )
            );
            cur_frame_time = max_frame_time;
            id_frame++;
        }
    }
}
