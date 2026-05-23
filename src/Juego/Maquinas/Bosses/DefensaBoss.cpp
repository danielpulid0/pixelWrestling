#include"DefensaBoss.hpp"
#include"IdleBoss2.hpp"
#include"AturdidoBoss.hpp"
#include"Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    DefensaBoss::DefensaBoss(float duracion)
        :FSMBoss{}, sprite{nullptr}, t_w{0}, t_h{0}, duracion{duracion}, timer{duracion}
    {
        nombre = "DefensaBoss";
    }

    FSM* DefensaBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)target;
        // Verificar si la defensa fue rota
        auto combate = parent.getComponente<ICombate>();
        if(combate && combate->esta_aturdido)
            return new AturdidoBoss(2.0f); // 2 segundos aturdido

        if(timer <= 0)
            return new IdleBoss2(2, 0.2f);

        return nullptr;
    }

    void DefensaBoss::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        t_w = obj.getComponente<CE::ISprite>()->width;
        t_h = obj.getComponente<CE::ISprite>()->height;
        timer = duracion;

        //activar defensa
        auto defensa = obj.getComponente<IDefensa>();
        if(defensa)
            defensa->defendiendo = true;

        //mostrar frame de defensa (row 11)
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 10 * t_h),
                sf::Vector2i(t_w, t_h)
            )
        );
    }

    void DefensaBoss::onSalir(const Entidad& obj)
    {
        auto defensa = obj.getComponente<IDefensa>();
        if(defensa)
            defensa->defendiendo = false;
    }

    void DefensaBoss::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        timer -= dt;
    }
}
