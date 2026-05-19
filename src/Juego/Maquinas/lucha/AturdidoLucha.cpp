#include"AturdidoLucha.hpp"
#include"IdleLucha.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    AturdidoLucha::AturdidoLucha(float duracion)
        :FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
        duracion{duracion}, timer{duracion}
    {
        nombre = "AturdidoLucha";
    }

    FSM* AturdidoLucha::onInputs(const CE::IControl& control)
    {
        (void)control;
        if(timer <= 0)
            return new IdleLucha(2, 0.7f);
        return nullptr;
    }

    void AturdidoLucha::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        timer = duracion;

        auto combate = obj.getComponente<ICombate>();
        if(combate) combate->esta_aturdido = true;

        auto defensa = obj.getComponente<IDefensa>();
        if(defensa) {
            defensa->defendiendo = false;
            defensa->golpes_bloqueados = 0;
        }
    }

    void AturdidoLucha::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) combate->esta_aturdido = false;
    }

    void AturdidoLucha::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        timer -= dt;
        cur_frame_time -= dt;
        if(cur_frame_time <= 0)
        {
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % 2), 11 * s_h), //Agregar animacion
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            cur_frame_time = frame_time;
        }
    }
}
