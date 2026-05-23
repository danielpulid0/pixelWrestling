#include"MoverLuchador.hpp"
#include"IdleLucha.hpp"
#include"CorrerLuchador.hpp"
#include"AtaqueSilla.hpp"
namespace IVJ
{
    MoverLuchador::MoverLuchador(int max_frames,float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{frame_rate},
        max_frames{max_frames}
    {
        nombre = "MoverLucha";
    }
    FSM* MoverLuchador::onInputs(const CE::IControl& control)
    {
        // si nada está presionado, regresar a idle
        if(!control.arr && !control.abj && !control.der && !control.izq)
            return new IdleLucha(2, 0.12f);

        // si presionan correr (doble tap activo), cambiar a correr
        if(control.run)
            return new CorrerLuchador(4, 0.08f);

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
    void MoverLuchador::onEntrar(const Entidad& obj)
    {
        m_ente = &obj;
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame=0;
    }
    void MoverLuchador::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void MoverLuchador::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj; //quitar el warning de no usar obj
        act_tiempo = act_tiempo - dt;
        //frame rate
        if (act_tiempo <=0)
        {
           // Supongamos que tu animación de caminar está en el segundo renglón de ShawnM.png (Renglón 1, ya que se cuenta desde 0)
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % max_frames), 1 * s_h), // <--- Multiplicamos por el renglón
                    sf::Vector2i(s_w, s_h)
                )
            );
            id_frame++;
            act_tiempo=max_tiempo;
        }
    }
}