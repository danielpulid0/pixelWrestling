#include "Escena_Snake.hpp"
#include <Juego/Componentes/ICPartesCuerpo.hpp>
#include <Juego/Componentes/ICParte.hpp>
#include <Juego/Componentes/ICFigura.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/Figuras/Figuras.hpp>

namespace IVJ
{
    Escena_Snake::Escena_Snake() : CE::Escena{} 
    {
    }

    void Escena_Snake::onInit()
    {
        if(!inicializar) return;

        registrarBotones(sf::Keyboard::Scancode::W, "arriba");
        registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
        registrarBotones(sf::Keyboard::Scancode::S, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
        registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::D, "derecha");
        registrarBotones(sf::Keyboard::Scancode::Right, "derecha");

        vibora = std::make_shared<IVJ::EnteVibora>();
        vibora->getNombre()->nombre = "cabeza_vibora";
        
        auto figura_cabeza = std::make_shared<Triangulo>(15.f, sf::Color::Green, sf::Color::White);
        figura_cabeza->setOrigin(sf::Vector2f(15.f, 15.f)); 
        figura_cabeza->setRotation(sf::degrees(90.f)); 
        vibora->addComponente(std::make_shared<ICFigura>(figura_cabeza));
        
        vibora->setPosicion(400.f, 300.f); 
        if (vibora->getTransformada()) {
            vibora->getTransformada()->velocidad = CE::Vector2D(150.f, 0.f); // Mover a la derecha al iniciar
        }
        objetos.agregarPool(figura_cabeza);
        
        auto partes_cuerpo = std::make_shared<ICPartesCuerpo>(30, 30);
        
        for(int i = 0; i < 3; i++) {
            auto fig = std::make_shared<Rectangulo>(30.f, 30.f, sf::Color::Green, sf::Color::Black);
            fig->setOrigin(sf::Vector2f(15.f, 15.f)); 
            
            auto parte = std::make_shared<ICParte>(fig, 15); 
            
            if (vibora->getTransformada()) {
                parte->pos->posicion = vibora->getTransformada()->posicion;
                fig->setPosicion(parte->pos->posicion.x, parte->pos->posicion.y);
            }
            
            partes_cuerpo->partes.push_back(parte);
            objetos.agregarPool(fig);
        }
        
        vibora->addComponente(partes_cuerpo);

        inicializar = false;
    }

    void Escena_Snake::onFinal()
    {
    }

    void Escena_Snake::onUpdate(float dt)
    {
        if (vibora) {
            vibora->onUpdate(dt);
        }

        for(auto& obj: objetos.getPool()) {
            obj->onUpdate(dt);
        }

        static float tiempo_acumulado = 0.0f;
        tiempo_acumulado += dt;

        if (tiempo_acumulado >= 3.0f) { 
            tiempo_acumulado = 0.0f; 
            if (vibora) {
                auto cuerpo = vibora->getComponente<ICPartesCuerpo>();
                if (cuerpo) {
                    auto fig = std::make_shared<Rectangulo>(30.f, 30.f, sf::Color::Green, sf::Color::Black);
                    fig->setOrigin(sf::Vector2f(15.f, 15.f)); 
                    auto nueva_parte = std::make_shared<ICParte>(fig, 15);
                    
                    if (!cuerpo->partes.empty()) {
                        nueva_parte->pos->posicion = cuerpo->partes.back()->pos->posicion;
                    } else if (vibora->getTransformada()) { 
                        nueva_parte->pos->posicion = vibora->getTransformada()->posicion;
                    }
                    
                    fig->setPosicion(nueva_parte->pos->posicion.x, nueva_parte->pos->posicion.y);
                    cuerpo->partes.push_back(nueva_parte);
                    objetos.agregarPool(fig);
                }
            }
        }
    }

    void Escena_Snake::onInputs(const CE::Botones& accion)
    {
        if (!vibora || !vibora->getTransformada()) return;

        float velocidadVibora = 140.f;

        if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
            std::string direccion = accion.getNombre();
            auto figuraComponente = vibora->getComponente<ICFigura>();
            Triangulo* tri = nullptr;
            if (figuraComponente && figuraComponente->figura) {
                tri = dynamic_cast<Triangulo*>(figuraComponente->figura.get());
            }

            if (direccion == "arriba") {
                vibora->getTransformada()->velocidad = CE::Vector2D(0.f, -velocidadVibora);
                if (tri) tri->setRotacionTriangulo(sf::degrees(0.f));
            }
            else if (direccion == "abajo") {
                vibora->getTransformada()->velocidad = CE::Vector2D(0.f, velocidadVibora);
                if (tri) tri->setRotacionTriangulo(sf::degrees(180.f));
            }
            else if (direccion == "izquierda") {
                vibora->getTransformada()->velocidad = CE::Vector2D(-velocidadVibora, 0.f);
                if (tri) tri->setRotacionTriangulo(sf::degrees(270.f));
            }
            else if (direccion == "derecha") {
                vibora->getTransformada()->velocidad = CE::Vector2D(velocidadVibora, 0.f);
                if (tri) tri->setRotacionTriangulo(sf::degrees(90.f));
            }
        }
    }


    void Escena_Snake::onRender()
    {
        for(auto& obj: objetos.getPool()) {
            CE::Render::Get().AddToDraw(*obj);
        }
    }
}
