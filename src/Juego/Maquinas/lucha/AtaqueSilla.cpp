#include"AtaqueSilla.hpp"
#include"IdleLucha.hpp"
#include"Motor/Componentes/IComponentes.hpp"
#include"Motor/Primitivos/GestorAssets.hpp"
#include <Juego/Escenas/EscenaMatch.hpp>

namespace IVJ
{
    AtaqueSilla::AtaqueSilla(int max_frames, float frame_rate)
        :FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate}
    {
        nombre = "AtaqueSilla";
    }

    FSM* AtaqueSilla::onInputs(const CE::IControl& control)
    {
        (void)control;
        if(animacion_terminada)
            return new IdleLucha(2, 0.12f);
        return nullptr;
    }

    void AtaqueSilla::onEntrar(const Entidad& obj)
    {
        auto iSprite = obj.getComponente<CE::ISprite>();

        //cambiar a la spritesheet del sillazo (80x128 por frame)
        std::string tex = (EscenaMatch::personaje_jugador == 0) ? "shawnSillazo" : "enemySillazo";
        iSprite->m_sprite.setTexture(CE::GestorAssets::Get().getTextura(tex));
        iSprite->width = 80;
        iSprite->height = 128;

        sprite = &iSprite->m_sprite;
        s_w = 80;
        s_h = 128;
        id_frame = 0;
        animacion_terminada = false;

        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 3; //item
            combate->tiene_silla = false; // "consumir" la silla
        }

        //mostrar primer frame inmediatamente
        sprite->setTextureRect(
            sf::IntRect(
                sf::Vector2i(0, 0),
                sf::Vector2i(s_w, s_h)
            )
        );
        
        // Aplicar offset para el primer frame antes de que onUpdate tome el control
        sprite->move({0.f, -45.f});

        id_frame = 1;
    }

    void AtaqueSilla::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
        }

        //restaurar la spritesheet original (80x88 por frame)
        auto iSprite = obj.getComponente<CE::ISprite>();
        std::string tex = (EscenaMatch::personaje_jugador == 0) ? "shawn" : "enemy";
        iSprite->m_sprite.setTexture(CE::GestorAssets::Get().getTextura(tex));
        iSprite->width = 80;
        iSprite->height = 88;
        
        // Restaurar explícitamente el rect para evitar "frame cortado" antes de que Idle actualice
        iSprite->m_sprite.setTextureRect(sf::IntRect({0, 0}, {80, 88}));
    }

    void AtaqueSilla::onUpdate(const Entidad& obj, float dt)
    {
        // Re-aplicar el offset cada frame porque el motor resetea la posición del sprite en su onUpdate
        auto iSprite = obj.getComponente<CE::ISprite>();
        if (iSprite) {
            iSprite->m_sprite.move({0.f, -45.f});
        }

        cur_frame_time -= dt;
        if(cur_frame_time <= 0)
        {
            if(id_frame >= max_frames) {
                animacion_terminada = true;
                return;
            }
            sprite->setTextureRect(
                sf::IntRect(
                    sf::Vector2i(s_w * (id_frame % max_frames), 0),
                    sf::Vector2i(s_w, s_h)
                )
            );
            
            if(id_frame == max_frames - 1) {
                auto combate = obj.getComponente<ICombate>();
                if(combate) combate->esta_atacando = true;
            }
            
            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
