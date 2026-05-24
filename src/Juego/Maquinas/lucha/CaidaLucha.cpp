#include "CaidaLucha.hpp"
#include "LevantarseLucha.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Juego/Componentes/IJComponentes.hpp"
#include <Motor/Primitivos/GestorAssets.hpp>


namespace IVJ
{
    CaidaLucha::CaidaLucha(int max_frames, float frame_rate)
        : FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
          max_frames{max_frames}, id_frame{0},
          max_frame_time{frame_rate}, cur_frame_time{frame_rate},
          animacion_terminada{false}, timer_suelo{0.f}
    {
        nombre = "CaidaLucha";
    }

    FSM* CaidaLucha::onInputs(const CE::IControl& control)
    {
        (void)control;
        //cuando terminó la animación de caída Y pasaron 2 seg en el suelo
        if(animacion_terminada && timer_suelo >= TIEMPO_SUELO)
            return new LevantarseLucha(7, 0.15f);
        return nullptr;
    }

    void CaidaLucha::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        id_frame = 0;
        animacion_terminada = false;
        timer_suelo = 0.f;

        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->en_caida = false;
            combate->esta_derribado = true;
        }

        auto control = obj.getComponente<CE::IControl>();
        if(control) control->setActivo(false);

        // fila 7 (caida) - row index 6
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 6 * s_h),
                sf::Vector2i(s_w, s_h)
            )
        );
        id_frame = 1;
        
        // reproducir sonido según la causa
        if(combate && combate->causa_caida == 3) {
            CE::GestorAssets::Get().getSonido("sillazo").play();
            CE::GestorAssets::Get().getSonido("awesomechant").play();
        } else {
            CE::GestorAssets::Get().getSonido("golpe").play();
        }
        
        // resetear causa para la próxima
        if(combate) combate->causa_caida = 0;
    }

    void CaidaLucha::onSalir(const Entidad& obj)
    {
        (void)obj;
    }

    void CaidaLucha::onUpdate(const Entidad& obj, float dt)
    {
        (void)obj;
        if(!animacion_terminada) {
            cur_frame_time -= dt;
            if(cur_frame_time <= 0) {
                if(id_frame >= max_frames) {
                    animacion_terminada = true;
                    return;
                }
                sprite->setTextureRect(
                    sf::IntRect(
                        sf::Vector2i(s_w * (id_frame % max_frames), 6 * s_h),
                        sf::Vector2i(s_w, s_h)
                    )
                );
                id_frame++;
                cur_frame_time = max_frame_time;
            }
        } else {
            //esperando en el suelo 2 segundos
            timer_suelo += dt;
        }
    }
}
