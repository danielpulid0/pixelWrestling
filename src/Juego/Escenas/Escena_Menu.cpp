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

namespace IVJ
{
    Escena_Menu::Escena_Menu()
        : CE::Escena{}
    {
    }

    void Escena_Menu::onInit()
    {
        if(!inicializar)
            return;

        // Fondo oscuro estilo arcade
        setColorFondo(sf::Color(10, 10, 15));

        CE::GestorAssets::Get().agregarMusica("menu_music", ASSETS "/musica/cancionFondo.ogg");
        CE::GestorAssets::Get().getMusica("menu_music").play();

        auto background = std::make_shared<Rectangulo>(1080.f, 720.f, sf::Color(15, 15, 30), sf::Color::Black);
        background->setPosicion(540.f, 360.f);

        // --- TÍTULO: ARRIBA Y PERFECTAMENTE CENTRADO ---
        // Con FontSize 88u y el texto "PIXEL WRESTLING", la coordenada X=220.f lo centra de forma ideal en los 1080px de pantalla.
        auto sombraTitulo = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("nova"), "PIXEL WRESTLING");
        sombraTitulo->setFontSize(88u);
        sombraTitulo->setColor(sf::Color(150, 0, 0)); // Sombra arcade roja
        sombraTitulo->setPosicion(190.f, 34.f); 

        titulo = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("nova"), "PIXEL WRESTLING");
        titulo->setFontSize(88u);
        titulo->setColor(sf::Color(255, 215, 0)); // Dorado clásico de Wrestling
        titulo->setPosicion(175.f, 30.f);

        // --- ARTE SIN RECORTAR: ESCALADO COMPLETO A LA IZQUIERDA ---
        // 1. Ponemos las dimensiones reales de tu nueva imagen (672x447)
        CE::GestorAssets::Get().agregarTextura("fondo_menu", ASSETS "/atlas/RingM.png", {0,0}, {672, 447});
        auto imagen = std::make_shared<Entidad>();
        auto& texture = CE::GestorAssets::Get().getTextura("fondo_menu");       
        
        // 2. ¡Creamos el componente Sprite que se te había borrado!
        int rectAnchoOriginal = 672;
        int rectAltoOriginal = 447;
        
        // Aumentamos la escala para que cubra toda la pantalla (1080x720)
        // 1080 / 672 = 1.607, 720 / 447 = 1.610 -> usamos 1.62f para cubrir todo
        float scale = 1.62f; 
        
        auto spriteImagen = std::make_shared<CE::ISprite>(texture, rectAnchoOriginal, rectAltoOriginal, scale);
        
        // Le pegamos el dibujo a la entidad
        imagen->addComponente(spriteImagen);
        
        // Posicionamos el centro de la imagen escalada en el centro de la pantalla
        imagen->setPosicion(540.f, 360.f);

        // --- OPCIONES DEL MENÚ: CENTRADAS EN LA PANTALLA ---
        const float xOpciones = 400.f; // Ajustado para centrar el texto horizontalmente
        const float yInicial = 260.f;  // Ajustado para centrar el bloque de opciones verticalmente
        salto = 50.f; 
        opcion = 0u;

        // Opción 1 (Inicia seleccionada en Blanco)
        opcion1 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Partida rapida");
        opcion1->setFontSize(44u); 
        opcion1->setColor(sf::Color::White);
        opcion1->setPosicion(xOpciones, yInicial);

        // Opción 2 (Inicia apagada en Gris)
        opcion2 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Modo Historia");
        opcion2->setFontSize(44u);
        opcion2->setColor(sf::Color(180, 180, 220));
        opcion2->setPosicion(xOpciones, yInicial + salto);

        // Opción 3 (Inicia apagada en Gris)
        opcion3 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Controles");
        opcion3->setFontSize(44u);
        opcion3->setColor(sf::Color(180, 180, 220));
        opcion3->setPosicion(xOpciones, yInicial + salto * 2);

        // Opción 4 (Inicia apagada en Gris)
        opcion4 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Salir");
        opcion4->setFontSize(44u);
        opcion4->setColor(sf::Color(180, 180, 220));
        opcion4->setPosicion(xOpciones, yInicial + salto * 3);

        marcador = std::make_shared<Triangulo>(20.f, sf::Color(255, 215, 0), sf::Color::White);
        marcador->setRotacionTriangulo(sf::degrees(90.f));
        marcador->setPosicion(xOpciones - 55.f, yInicial + opcion * salto + 15.f);

        // Registro ordenado en el Pool de renderizado
        objetos.agregarPool(background);
        objetos.agregarPool(imagen); 
        objetos.agregarPool(sombraTitulo); 
        objetos.agregarPool(titulo);
        objetos.agregarPool(opcion1);
        objetos.agregarPool(opcion2);
        objetos.agregarPool(opcion3);
        objetos.agregarPool(opcion4);
        objetos.agregarPool(marcador);

        registrarBotones(sf::Keyboard::Scancode::W, "arriba");
        registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
        registrarBotones(sf::Keyboard::Scancode::S, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");

        inicializar = false;
    }

    void Escena_Menu::onFinal()
    {
        if (!inicializar)
            CE::GestorAssets::Get().getMusica("menu_music").stop();
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

                    // Gestión del color para variables de texto independientes
                    opcion1->setColor(opcion == 0 ? sf::Color::White : sf::Color(180, 180, 220));
                    opcion2->setColor(opcion == 1 ? sf::Color::White : sf::Color(180, 180, 220));
                    opcion3->setColor(opcion == 2 ? sf::Color::White : sf::Color(180, 180, 220));
                    opcion4->setColor(opcion == 3 ? sf::Color::White : sf::Color(180, 180, 220));

                    const float yInicial = 320.f;
                    const float xOpciones = 400.f;
                    
                    marcador->setPosicion(xOpciones - 55.f, yInicial + opcion * salto + 15.f);
                }

                if(accion.getNombre() == "aceptar"){
                    switch(opcion)
                    {
                        case 0:
                            CE::GestorEscenas::Get().cambiarEscena("SpriteTiles");
                            break;
                        case 1:
                            CE::GestorEscenas::Get().cambiarEscena("Match");
                            break;
                        case 2:
                            // TODO: implementar escena de controles
                            break;
                        case 3:
                            std::exit(0);
                            break;
                    }
                }
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                break;
            }
            case CE::Botones::TipoAccion::None:
            {
                break;
            }
        }
    }

    void Escena_Menu::onRender()
    {
        for(auto& obj: objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
    }
}