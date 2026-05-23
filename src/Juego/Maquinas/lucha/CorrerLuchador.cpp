#include"CorrerLuchador.hpp"
#include"IdleLucha.hpp"
#include"MoverLuchador.hpp"
#include"AtaqueSilla.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    CorrerLuchador::CorrerLuchador(int max_frames, float frame_rate)
        :FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{frame_rate},
        max_frames{max_frames}, id_frame{0},
        m_ente{nullptr}
    {
        nombre = "CorrerLucha";
    }

    FSM* CorrerLuchador::onInputs(const CE::IControl& control)
    {
        //si sueltan todo, volver a idle
        if(!control.arr && !control.abj && !control.der && !control.izq)
            return new IdleLucha(2, 0.12f);
        
        //si ya no están corriendo, volver a caminar
        if(!control.run)
            return new MoverLuchador(3, 0.1f);

        // ataque con silla
        if(control.punch || control.kick) {
            if(m_ente) {
                auto combate = m_ente->getComponente<ICombate>();
                if(combate && combate->tiene_silla) {
                    return new AtaqueSilla(3, 0.12f);
                }
            }
        }

        return nullptr;
    }

    void CorrerLuchador::onEntrar(const Entidad& obj)
    {
        m_ente = &obj;
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        
        // Mostrar primer frame inmediatamente (Fila 2)
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 2 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1;
    }

    void CorrerLuchador::onSalir(const Entidad& obj)
    {
        //restaurar la escala normal al salir de correr
        if(obj.getComponente<CE::ISprite>())
            obj.getComponente<CE::ISprite>()->m_sprite.setScale({1.f, 1.f});
    }

    void CorrerLuchador::onUpdate(const Entidad& obj, float dt)
    {
        //voltear sprite según dirección relativa al oponente
        auto target = obj.getComponente<ITarget>();
        auto control = obj.getComponente<CE::IControl>();
        if(target && target->pos && control)
        {
            auto pos = const_cast<Entidad&>(obj).getTransformada()->posicion;
            float dx = target->pos->x - pos.x; // positivo = oponente a la derecha

            // Si corre hacia la izquierda y el oponente está a la derecha (corre alejándose)
            // o si corre hacia la derecha y el oponente está a la izquierda (corre alejándose)
            bool corriendo_izq = control->izq;
            bool corriendo_der = control->der;
            bool oponente_derecha = dx > 0;

            if((corriendo_izq && oponente_derecha) || (corriendo_der && !oponente_derecha))
            {
                // Corriendo en dirección contraria al oponente: voltear sprite
                sprite->setScale({-1.f, 1.f});
            }
            else
            {
                // Corriendo hacia el oponente: escala normal
                sprite->setScale({1.f, 1.f});
            }
        }

        act_tiempo -= dt;
        if (act_tiempo <= 0)
        {
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % max_frames), 2 * s_h),
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            act_tiempo = max_tiempo;
        }
    }
}
