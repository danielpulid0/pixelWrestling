#include "Escena_Menu.hpp"
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/Texto.hpp>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Juego/Escenas/EscenaMatch.hpp>

namespace IVJ
{
    Escena_Menu::Escena_Menu()
        : CE::Escena{}
    {
    }

    void Escena_Menu::onInit()
    {
        CE::GestorCamaras::Get().setCamaraActiva(0);
        if(!inicializar)
            return;

        // Volvemos a tu método original para el color de fondo base
        setColorFondo(sf::Color(10, 10, 15));

        CE::GestorAssets::Get().agregarMusica("menu_music", ASSETS "/musica/cancionFondo.ogg");
        CE::GestorAssets::Get().getMusica("menu_music").play();

        auto background = std::make_shared<Rectangulo>(1080.f, 720.f, sf::Color(15, 15, 30), sf::Color::Black);
        background->setPosicion(540.f, 360.f);

        // --- TÍTULO ---
        auto sombraTitulo = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("nova"), "PIXEL WRESTLING");
        sombraTitulo->setFontSize(88u);
        sombraTitulo->setColor(sf::Color(150, 0, 0)); 
        sombraTitulo->setPosicion(190.f, 34.f); 

        titulo = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("nova"), "PIXEL WRESTLING");
        titulo->setFontSize(88u);
        titulo->setColor(sf::Color(255, 215, 0)); 
        titulo->setPosicion(175.f, 30.f);

        // --- ARTE EN EL FONDO ---
        CE::GestorAssets::Get().agregarTextura("fondo_menu", ASSETS "/atlas/RingM.png", {0,0}, {672, 447});
        auto imagen = std::make_shared<Entidad>();
        auto& texture = CE::GestorAssets::Get().getTextura("fondo_menu");       
        
        int rectAnchoOriginal = 672;
        int rectAltoOriginal = 447;
        float scale = 1.62f; 
        
        auto spriteImagen = std::make_shared<CE::ISprite>(texture, rectAnchoOriginal, rectAltoOriginal, scale);
        imagen->addComponente(spriteImagen);
        imagen->setPosicion(540.f, 360.f);

        // --- CONFIGURACIÓN DE OPCIONES ---
        const float xOpciones = 390.f; 
        const float yInicial = 240.f;  
        salto = 60.f; 
        opcion = 0u;

        // --- CUADRO DE CONTRASTE (FONDO PARA LAS OPCIONES) ---
        // Creamos un rectángulo negro semi-transparente (el '180' al final es el canal Alfa/Opacidad)
        // Cubre un área de 450px de ancho por 270px de alto, ideal para englobar el bloque de texto
        auto cuadroTexto = std::make_shared<Rectangulo>(450.f, 270.f, sf::Color(0, 0, 0, 180), sf::Color(255, 215, 0, 100));
        // Lo centramos justo debajo de las opciones del menú
        cuadroTexto->setPosicion(565.f, 355.f);

        // Colores de las opciones: Blanco para seleccionado, Gris claro/Azulado para no seleccionado
        sf::Color colorApagado(170, 175, 190);

        // Opción 1 (Inicia seleccionada)
        opcion1 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Partida rapida");
        opcion1->setFontSize(44u); 
        opcion1->setColor(sf::Color::White);
        opcion1->setPosicion(xOpciones, yInicial);

        // Opción 2
        opcion2 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Modo Historia");
        opcion2->setFontSize(44u);
        opcion2->setColor(colorApagado);
        opcion2->setPosicion(xOpciones, yInicial + salto);

        // Opción 3
        opcion3 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Controles");
        opcion3->setFontSize(44u);
        opcion3->setColor(colorApagado);
        opcion3->setPosicion(xOpciones, yInicial + salto * 2);

        // Opción 4
        opcion4 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Salir");
        opcion4->setFontSize(44u);
        opcion4->setColor(colorApagado);
        opcion4->setPosicion(xOpciones, yInicial + salto * 3);

        // Registro ordenado en el Pool (El cuadroTexto va ANTES que las opciones para que no las tape)
        objetos.agregarPool(background);
        objetos.agregarPool(imagen); 
        objetos.agregarPool(cuadroTexto); // <-- Dibujado encima del ring, debajo de las letras
        objetos.agregarPool(sombraTitulo); 
        objetos.agregarPool(titulo);
        objetos.agregarPool(opcion1);
        objetos.agregarPool(opcion2);
        objetos.agregarPool(opcion3);
        objetos.agregarPool(opcion4);

        registrarBotones(sf::Keyboard::Scancode::W, "arriba");
        registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
        registrarBotones(sf::Keyboard::Scancode::S, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
        registrarBotones(sf::Keyboard::Scancode::Escape, "escape");

        txt_controles = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), 
            "CONTROLES\n\nZ: Golpe básico\nX: Patada\nC: Bloquear\nV: Recoger Objeto\nF: Remate (Con Momentum lleno)\nFlechas: Moverse\nDoble Tap Flechas: Correr\nP: Pausa\n\nPresiona Enter para volver");
        txt_controles->setFontSize(30u);
        txt_controles->setColor(sf::Color::White);
        txt_controles->setOriginCenter();
        txt_controles->setPosicion(540.f, 360.f);

        fondo_controles = std::make_shared<Rectangulo>(1080.f, 720.f, sf::Color(0, 0, 0, 180), sf::Color::Transparent);
        fondo_controles->setPosicion(540.f, 360.f);

        inicializar = false;
    }

    void Escena_Menu::onFinal()
    {
        if (!inicializar)
            CE::GestorAssets::Get().getMusica("menu_music").setVolume(10.f);
    }

    void Escena_Menu::onUpdate(float dt)
    {
       for(auto& obj: objetos.getPool())
       {
            obj->onUpdate(dt);
            SistemaGirar(*obj, dt);
       }
    }

    void Escena_Menu::onInputs(const CE::Botones& accion)
    {
        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {   
                if(mostrando_controles) {
                    if(accion.getNombre() == "aceptar" || accion.getNombre() == "escape") {
                        mostrando_controles = false;
                    }
                    return;
                }

                if(accion.getNombre() == "arriba" || accion.getNombre() == "abajo") 
                {
                    if(accion.getNombre() == "arriba")
                    {
                        if(opcion == 0u)
                            opcion = 3u; 
                        else
                            opcion--;
                    }
                    else
                    {
                        opcion = (opcion + 1u) % 4u; 
                    }

                    sf::Color colorApagado(170, 175, 190);

                    opcion1->setColor(opcion == 0 ? sf::Color::White : colorApagado);
                    opcion2->setColor(opcion == 1 ? sf::Color::White : colorApagado);
                    opcion3->setColor(opcion == 2 ? sf::Color::White : colorApagado);
                    opcion4->setColor(opcion == 3 ? sf::Color::White : colorApagado);
                }

                if(accion.getNombre() == "aceptar"){
                    switch(opcion)
                    {
                        case 0:
                            CE::GestorEscenas::Get().cambiarEscena("CharacterSelect");
                            break;
                        case 1:
                            IVJ::EscenaMatch::personaje_jugador = 0;
                            IVJ::EscenaMatch::personaje_rival = 1;
                            CE::GestorEscenas::Get().cambiarEscena("Cutscene");
                            break;
                        case 2:
                            mostrando_controles = true;
                            break;
                        case 3:
                            std::exit(0);
                            break;
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    void Escena_Menu::onRender()
    {
        for(auto& obj: objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
            
        if(mostrando_controles) {
            fondo_controles->onUpdate(0);
            CE::Render::Get().AddToDraw(*fondo_controles);
            CE::Render::Get().AddToDraw(*txt_controles);
        }
    }
}