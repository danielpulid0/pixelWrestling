#include"AturdidoBoss.hpp"
#include"IdleBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    AturdidoBoss::AturdidoBoss(float duracion)
        :FSMBoss{}, sprite{nullptr}, t_w{0}, t_h{0},
        duracion{duracion}, timer{duracion}
    {
        nombre = "AturdidoBoss";
    }

    FSM* AturdidoBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        if(timer <= 0)
            return new IdleBoss2(2, 0.2f);
        return nullptr;
    }

    void AturdidoBoss::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        t_w = obj.getComponente<CE::ISprite>()->width;
        t_h = obj.getComponente<CE::ISprite>()->height;
        timer = duracion;

        auto combate = obj.getComponente<ICombate>();
        if(combate) combate->esta_aturdido = true;

        auto defensa = obj.getComponente<IDefensa>();
        if(defensa) {
            defensa->defendiendo = false;
            defensa->golpes_bloqueados = 0;
        }
    }

    void AturdidoBoss::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) combate->esta_aturdido = false;
    }

    void AturdidoBoss::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        timer -= dt;
        cur_frame_time -= dt;
        if(cur_frame_time <= 0)
        {
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(t_w * (id_frame % 2), 11 * t_h), // fila 12
                    sf::Vector2i(t_w, t_h)
                )
            );
            id_frame++;
            cur_frame_time = frame_time;
        }
    }
}
