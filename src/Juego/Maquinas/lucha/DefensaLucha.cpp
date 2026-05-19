#include"DefensaLucha.hpp"
#include"IdleLucha.hpp"
#include"AturdidoLucha.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    DefensaLucha::DefensaLucha()
        :FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, m_ente{nullptr}
    {
        nombre = "DefensaLucha";
    }

    FSM* DefensaLucha::onInputs(const CE::IControl& control)
    {
        // Verificar si la defensa fue rota (el sistema de daño activa esta flag)
        auto combate = m_ente->getComponente<ICombate>();
        if(combate && combate->esta_aturdido)
            return new AturdidoLucha(2.0f); // 2 segundos aturdido

        // Si sueltan la guardia ('C'), volver a idle rápidamente
        if(!control.guard)
            return new IdleLucha(2, 0.12f);

        return nullptr;
    }

    void DefensaLucha::onEntrar(const Entidad& obj)
    {
        m_ente = &obj;
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;

        //activar defensa
        auto defensa = obj.getComponente<IDefensa>();
        if(defensa)
            defensa->defendiendo = true;

        //mostrar frame de defensa (row 4)
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 10 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
    }

    void DefensaLucha::onSalir(const Entidad& obj)
    {
        auto defensa = obj.getComponente<IDefensa>();
        if(defensa)
            defensa->defendiendo = false;
    }

    void DefensaLucha::onUpdate(const Entidad& obj, float dt)
    {
        (void)dt;
        //verificar si la defensa fue rota
        auto defensa = obj.getComponente<IDefensa>();
        if(defensa && defensa->golpes_bloqueados >= IDefensa::MAX_BLOQUEOS)
        {
            //la rotura se maneja en el sistema de daño,
            //el cambio a aturdido se hará desde inputFSM al detectar esta_aturdido
        }
    }
}
