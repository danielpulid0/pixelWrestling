#include"IdleLucha.hpp"
#include"MoverLuchador.hpp"
#include"CorrerLuchador.hpp"
#include"GolpeLigero.hpp"
#include"PatadaLigera.hpp"
#include"DefensaLucha.hpp"
#include"RemateLucha.hpp"

namespace IVJ
{
    IdleLucha::IdleLucha(int max_frames, float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_frames{max_frames},
        id_frame{0},
        max_frame_time{frame_rate},
        cur_frame_time{frame_rate}
    {
        nombre = "IdleLucha";
    }
    FSM* IdleLucha::onInputs(const CE::IControl& control)
    {
        //movimiento (incluyendo arriba/abajo)
        if(control.izq || control.der || control.arr || control.abj)
        {
            if(control.run)
                return new CorrerLuchador(4, 0.08f);
            return new MoverLuchador(3, 0.1f);
        }

        //golpe ligero (Z)
        if(control.punch)
            return new GolpeLigero(2, 0.09f);

        //patada ligera (Y)
        if(control.kick)
            return new PatadaLigera(2, 0.09f);

        //defensa (C)
        if(control.guard) {
            if(m_ente) {
                auto defensa = m_ente->getComponente<IDefensa>();
                if(!defensa || defensa->cooldown_defensa <= 0.f) {
                    return new DefensaLucha();
                }
            } else {
                return new DefensaLucha();
            }
        }

        //remate (F) — solo si momentum lleno
        if(control.finisher) {
            if(m_ente) {
                auto momentum = m_ente->getComponente<IMomentum>();
                if(momentum && momentum->remate_disponible)
                    return new RemateLucha(3, 0.05f);
            }
        }

        return nullptr;
    }
    void IdleLucha::onEntrar(const Entidad& obj)
    {
        m_ente = &obj;
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
    }
    void IdleLucha::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void IdleLucha::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj;

        cur_frame_time -= dt;
        if (cur_frame_time <= 0)
        {
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % max_frames), 0 * s_h),
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
