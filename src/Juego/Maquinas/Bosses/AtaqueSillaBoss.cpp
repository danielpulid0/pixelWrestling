#include"AtaqueSillaBoss.hpp"
#include"IdleBoss2.hpp"
#include"Motor/Componentes/IComponentes.hpp"
#include"Motor/Primitivos/GestorAssets.hpp"
#include <Juego/Escenas/EscenaMatch.hpp>

namespace IVJ
{
    AtaqueSillaBoss::AtaqueSillaBoss(int max_frames, float frame_rate)
        :FSMBoss{}, sprite{nullptr}, s_w{0}, s_h{0},
        max_frames{max_frames}, id_frame{0},
        max_frame_time{frame_rate}, cur_frame_time{frame_rate}
    {
        nombre = "AtaqueSillaBoss";
    }

    FSM* AtaqueSillaBoss::onInputs(Entidad& parent, CE::Vector2D& target)
    {
        (void)parent;
        (void)target;
        if(animacion_terminada)
            return new IdleBoss2(2, 0.2f);
        return nullptr;
    }

    void AtaqueSillaBoss::onEntrar(const Entidad& obj)
    {
        auto iSprite = obj.getComponente<CE::ISprite>();

        //cambiar a la spritesheet del sillazo del boss (80x128)
        std::string tex = (EscenaMatch::personaje_rival == 0) ? "shawnSillazo" : "enemySillazo";
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
            combate->tiene_silla = false; // consume la silla
            combate->cooldown_ataque = 1.5f; // cooldown largo después de sillazo
        }

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

    void AtaqueSillaBoss::onSalir(const Entidad& obj)
    {
        auto combate = obj.getComponente<ICombate>();
        if(combate) {
            combate->esta_atacando = false;
            combate->tipo_ataque = 0;
        }

        //restaurar la spritesheet original (80x88 por frame)
        auto iSprite = obj.getComponente<CE::ISprite>();
        std::string tex = (EscenaMatch::personaje_rival == 0) ? "shawn" : "enemy";
        iSprite->m_sprite.setTexture(CE::GestorAssets::Get().getTextura(tex));
        iSprite->width = 80;
        iSprite->height = 88;
        
        iSprite->m_sprite.setTextureRect(sf::IntRect({0, 0}, {80, 88}));
    }

    void AtaqueSillaBoss::onUpdate(const Entidad& obj, float dt)
    {
        // Re-aplicar el offset cada frame
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
            
            // Si es el último frame, activar el ataque
            if(id_frame == max_frames - 1) {
                auto combate = obj.getComponente<ICombate>();
                if(combate) combate->esta_atacando = true;
            }

            id_frame++;
            cur_frame_time = max_frame_time;
        }
    }
}
