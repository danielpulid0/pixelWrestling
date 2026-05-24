#include "Escena_CharacterSelect.hpp"
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Juego/Escenas/EscenaMatch.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Render/Render.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>

namespace IVJ
{
    Escena_CharacterSelect::Escena_CharacterSelect() : CE::Escena{}
    {
    }

    void Escena_CharacterSelect::onInit()
    {
        CE::GestorCamaras::Get().setCamaraActiva(1);
        // Limpiar el pool
        objetos.borrarPool();
        entidades_retratos.clear();

        estado_actual = EstadoSeleccion::ELIGIENDO_JUGADOR;
        seleccion_jugador = -1;

        // 1. Cargar las texturas de los retratos si no están cargadas
        // (Usamos dim {0,0} para que cargue toda la imagen)
        auto& gestor = CE::GestorAssets::Get();
        gestor.agregarSonido("bloqueado", ASSETS "/sonidos/bloqueado.ogg");
        gestor.agregarTextura("sP", ASSETS "/sprites/lucha/sP.png", {0,0}, {0,0});
        gestor.agregarTextura("mP", ASSETS "/sprites/lucha/mP.png", {0,0}, {0,0});
        gestor.agregarTextura("bretP", ASSETS "/sprites/lucha/bretP.png", {0,0}, {0,0});
        gestor.agregarTextura("rP", ASSETS "/sprites/lucha/rP.png", {0,0}, {0,0});
        gestor.agregarTextura("ricP", ASSETS "/sprites/lucha/ricP.png", {0,0}, {0,0});
        gestor.agregarTextura("uP", ASSETS "/sprites/lucha/uP.png", {0,0}, {0,0});

        // 2. Definir los personajes disponibles
        personajes = {
            {"sP", "ShawnM.png", "Shawn Michaels", true},
            {"mP", "MrP.png", "Mr. Perfect", true},
            {"bretP", "BretHart.png", "Bret Hart", false},
            {"rP", "RazorRamon.png", "Razor Ramon", false},
            {"ricP", "RicFlair.png", "Ric Flair", false},
            {"uP", "Undertaker.png", "Undertaker", false}
        };

        // 3. Crear el fondo
        auto background = std::make_shared<Entidad>();
        auto bg_sprite = std::make_shared<CE::ISprite>(gestor.getTextura("fondo_menu"), 672, 447, 1.f);
        background->addComponente(bg_sprite);
        background->setPosicion(336.f, 223.5f);
        objetos.agregarPool(background);

        // Fondo semi-transparente para la cuadrícula
        auto fondo_grid = std::make_shared<Rectangulo>(600.f, 400.f, sf::Color(0, 0, 0, 180), sf::Color(255, 215, 0, 100));
        fondo_grid->setPosicion(336.f, 250.f);
        objetos.agregarPool(fondo_grid);

        // 4. Crear los títulos y textos
        titulo = std::make_shared<Texto>(gestor.getFont("default_font"), "SELECCIONA TU LUCHADOR");
        titulo->setFontSize(40u);
        titulo->setColor(sf::Color::White);
        titulo->setOriginCenter();
        titulo->setPosicion(336.f, 80.f);
        objetos.agregarPool(titulo);

        nombre_personaje_txt = std::make_shared<Texto>(gestor.getFont("default_font"), "");
        nombre_personaje_txt->setFontSize(30u);
        nombre_personaje_txt->setColor(sf::Color::Yellow);
        nombre_personaje_txt->setOriginCenter();
        nombre_personaje_txt->setPosicion(336.f, 420.f);
        objetos.agregarPool(nombre_personaje_txt);

        // 5. Crear la cuadrícula de retratos
        const int columnas = 3;
        const float espacio_x = 150.f;
        const float espacio_y = 150.f;
        const float inicio_x = 336.f - espacio_x;
        const float inicio_y = 180.f;

        for (size_t i = 0; i < personajes.size(); ++i) {
            auto ent_retrato = std::make_shared<Entidad>();
            auto& tex = gestor.getTextura(personajes[i].textura_id);
            float w = tex.getSize().x;
            float h = tex.getSize().y;
            // Escalar para que quepan en un cuadro de 120x120 aprox
            float scale = 120.f / std::max(w, h);
            
            auto sprite = std::make_shared<CE::ISprite>(tex, w, h, scale);
            
            // Si está bloqueado, oscurecer
            if (!personajes[i].desbloqueado) {
                sprite->m_sprite.setColor(sf::Color(80, 80, 80));
            }

            ent_retrato->addComponente(sprite);
            
            float x = inicio_x + (i % columnas) * espacio_x;
            float y = inicio_y + (i / columnas) * espacio_y;
            ent_retrato->setPosicion(x, y);
            
            objetos.agregarPool(ent_retrato);
            entidades_retratos.push_back(ent_retrato);
        }

        // 6. Crear el cursor de selección (Recuadro amarillo/rojo)
        // El cursor es un rectángulo sin relleno y con borde grueso
        cursor_rect = std::make_shared<Rectangulo>(130.f, 130.f, sf::Color::Transparent, sf::Color::Yellow);
        // CimaEngine3v Rectangulo class in Figuras.hpp might not have setOutlineThickness, 
        // wait, let's just make it a rectangle with a transparent fill color and yellow outline.
        // Usually the Rectangulo constructor takes (w, h, fillColor, outlineColor).
        objetos.agregarPool(cursor_rect);

        // 7. Registrar botones
        registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::Right, "derecha");
        registrarBotones(sf::Keyboard::Scancode::D, "derecha");
        registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
        registrarBotones(sf::Keyboard::Scancode::W, "arriba");
        registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
        registrarBotones(sf::Keyboard::Scancode::S, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
        registrarBotones(sf::Keyboard::Scancode::Escape, "escape");

        opcion_index = 0;
        actualizarUI();
    }

    void Escena_CharacterSelect::actualizarUI()
    {
        if (estado_actual == EstadoSeleccion::ELIGIENDO_JUGADOR) {
            titulo->setString("SELECCIONA TU LUCHADOR");
        } else {
            titulo->setString("SELECCIONA TU RIVAL");
        }
        titulo->setOriginCenter();

        // Oscurecer el retrato del jugador seleccionado para indicar que no es elegible
        for (size_t i = 0; i < personajes.size(); ++i) {
            auto sprite = entidades_retratos[i]->getComponente<CE::ISprite>();
            if (!personajes[i].desbloqueado || (estado_actual == EstadoSeleccion::ELIGIENDO_RIVAL && i == seleccion_jugador)) {
                sprite->m_sprite.setColor(sf::Color(80, 80, 80));
            } else {
                sprite->m_sprite.setColor(sf::Color::White);
            }
        }

        bool es_seleccionable = personajes[opcion_index].desbloqueado && 
                                !(estado_actual == EstadoSeleccion::ELIGIENDO_RIVAL && opcion_index == seleccion_jugador);

        // Actualizar el texto del nombre del personaje
        if (es_seleccionable) {
            nombre_personaje_txt->setString(personajes[opcion_index].nombre_mostrar);
            nombre_personaje_txt->setColor(sf::Color::Yellow);
            cursor_rect->setOutlineColor(sf::Color::Yellow);
        } else {
            nombre_personaje_txt->setString("???");
            nombre_personaje_txt->setColor(sf::Color(150, 150, 150));
            cursor_rect->setOutlineColor(sf::Color::Red);
        }
        nombre_personaje_txt->setOriginCenter();

        // Actualizar la posición del cursor
        if (opcion_index < entidades_retratos.size()) {
            auto pos = entidades_retratos[opcion_index]->getTransformada()->posicion;
            cursor_rect->setPosicion(pos.x, pos.y);
        }
    }

    void Escena_CharacterSelect::onFinal()
    {
    }

    void Escena_CharacterSelect::onUpdate(float dt)
    {
        for (auto& obj : objetos.getPool())
        {
            obj->onUpdate(dt);
        }
    }

    void Escena_CharacterSelect::onInputs(const CE::Botones& accion)
    {
        if (accion.getTipo() == CE::Botones::TipoAccion::OnPress)
        {
            const unsigned int columnas = 3;
            const unsigned int total = personajes.size();
            unsigned int col = opcion_index % columnas;
            unsigned int row = opcion_index / columnas;

            if (accion.getNombre() == "izquierda") {
                if (col > 0) opcion_index--;
            }
            else if (accion.getNombre() == "derecha") {
                if (col < columnas - 1 && opcion_index + 1 < total) opcion_index++;
            }
            else if (accion.getNombre() == "arriba") {
                if (row > 0) opcion_index -= columnas;
            }
            else if (accion.getNombre() == "abajo") {
                if (opcion_index + columnas < total) opcion_index += columnas;
            }
            else if (accion.getNombre() == "aceptar") {
                bool es_seleccionable = personajes[opcion_index].desbloqueado && 
                                        !(estado_actual == EstadoSeleccion::ELIGIENDO_RIVAL && opcion_index == seleccion_jugador);
                
                if (es_seleccionable) {
                    CE::GestorAssets::Get().getSonido("bloqueado").play();
                    
                    if (estado_actual == EstadoSeleccion::ELIGIENDO_JUGADOR) {
                        seleccion_jugador = opcion_index;
                        estado_actual = EstadoSeleccion::ELIGIENDO_RIVAL;
                    } else {
                        // Pasar la selección a la EscenaMatch
                        EscenaMatch::personaje_jugador = seleccion_jugador;
                        EscenaMatch::personaje_rival = opcion_index;
                        CE::GestorEscenas::Get().cambiarEscena("Match");
                    }
                } else {
                    // Reproducir sonido de error si estuviera disponible
                }
            }
            else if (accion.getNombre() == "escape") {
                if (estado_actual == EstadoSeleccion::ELIGIENDO_RIVAL) {
                    estado_actual = EstadoSeleccion::ELIGIENDO_JUGADOR;
                    seleccion_jugador = -1;
                } else {
                    CE::GestorEscenas::Get().cambiarEscena("Menu");
                }
            }

            actualizarUI();
        }
    }

    void Escena_CharacterSelect::onRender()
    {
        for (auto& obj : objetos.getPool())
        {
            CE::Render::Get().AddToDraw(*obj);
        }
    }
}
