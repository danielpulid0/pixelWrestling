#include "Escena_Epidemia.hpp"
#include <Juego/Componentes/ICPartesCuerpo.hpp>
#include <Juego/Componentes/ICParte.hpp>
#include <Juego/Componentes/ICFigura.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/Persona.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <random>
#include <algorithm>
#include <iostream>

namespace IVJ{
    Escena_Epidemia::Escena_Epidemia() : CE::Escena{} 
    {
    }

    void Escena_Epidemia::onInit(){
        if(!inicializar) 
            return; 
        inicializar = false;

        setColorFondo(sf::Color(255, 255, 255)); 

        const int ROWS = 15;
        const int COLS = 15;
        const float CELL_SIZE = 40.0f; 
        const float OFFSET_X = 350.0f; 
        const float OFFSET_Y = 80.0f;  
        
        std::vector<std::shared_ptr<Persona>> grid_personas;
        for (int row = 0; row < ROWS; ++row){
            for (int col = 0; col < COLS; ++col){
                float world_x = OFFSET_X + col * CELL_SIZE;
                float world_y = OFFSET_Y + row * CELL_SIZE;
                
                auto persona = std::make_shared<Persona>(col, row, world_x, world_y);
                grid_personas.push_back(persona);
                objetos.agregarPool(persona);
            }
        }

        int total_personas = ROWS * COLS;
        int num_enfermos = total_personas * 0.10f;
        
        std::vector<int> indices(total_personas);
        std::iota(indices.begin(), indices.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);

        for (int i = 0; i < num_enfermos; ++i){
            auto estado = grid_personas[indices[i]]->getComponente<IEstadoEpidemia>();
            if (estado)
                estado->estado_actual = EstadoSalud::ENFERMO;
        }

        auto& font = CE::GestorAssets::Get().getFont("default_font");
        txt_normal = std::make_shared<Texto>(font, "Normal: 0");
        txt_enfermo = std::make_shared<Texto>(font, "Enfermo: 0");
        txt_recuperacion = std::make_shared<Texto>(font, "Recupera: 0");
        txt_inmune = std::make_shared<Texto>(font, "Inmune: 0");
        txt_muerto = std::make_shared<Texto>(font, "Muerto: 0");
        txt_status = std::make_shared<Texto>(font, "Epidemia en curso");

        auto t_n = std::dynamic_pointer_cast<Texto>(txt_normal);
        auto t_e = std::dynamic_pointer_cast<Texto>(txt_enfermo);
        auto t_r = std::dynamic_pointer_cast<Texto>(txt_recuperacion);
        auto t_i = std::dynamic_pointer_cast<Texto>(txt_inmune);
        auto t_m = std::dynamic_pointer_cast<Texto>(txt_muerto);
        auto t_s = std::dynamic_pointer_cast<Texto>(txt_status);

        t_n->setFontSize(24); t_n->setColor(sf::Color(0, 49, 181)); t_n->setPosicion(50, 50);
        t_e->setFontSize(24); t_e->setColor(sf::Color::Red); t_e->setPosicion(50, 90);
        t_r->setFontSize(24); t_r->setColor(sf::Color(89, 4, 133)); t_r->setPosicion(50, 130);
        t_i->setFontSize(24); t_i->setColor(sf::Color::Green); t_i->setPosicion(50, 170);
        t_m->setFontSize(24); t_m->setColor(sf::Color::Black); t_m->setPosicion(50, 210);
        t_s->setFontSize(24); t_s->setColor(sf::Color::Black); t_s->setPosicion(50, 250);
        
        objetos.agregarPool(txt_normal);
        objetos.agregarPool(txt_enfermo);
        objetos.agregarPool(txt_recuperacion);
        objetos.agregarPool(txt_inmune);
        objetos.agregarPool(txt_muerto);
        objetos.agregarPool(txt_status);
    }

    void Escena_Epidemia::onFinal(){
    }

    void Escena_Epidemia::onUpdate(float dt){
        if (fin_juego) 
            return;

        SistemaEpidemia(objetos, dt);
        int cont_normal = 0, cont_enfermo = 0, cont_recuperacion = 0, cont_inmune = 0, cont_muerto = 0;
        for(auto& obj: objetos.getPool()) {
            obj->onUpdate(dt);
            if (obj->tieneComponente<IEstadoEpidemia>()){
                auto estado = obj->getComponente<IEstadoEpidemia>()->estado_actual;
                if (estado == EstadoSalud::NORMAL)
                    cont_normal++;
                else if (estado == EstadoSalud::ENFERMO)
                    cont_enfermo++;
                else if (estado == EstadoSalud::RECUPERACION)
                    cont_recuperacion++;
                else if (estado == EstadoSalud::INMUNE)
                    cont_inmune++;
                else if (estado == EstadoSalud::MUERTO)
                    cont_muerto++;
            }
        }

        if ((cont_enfermo == 0 && cont_recuperacion == 0) || cont_muerto == 225)
            fin_juego = true;

        if (txt_normal) std::dynamic_pointer_cast<Texto>(txt_normal)->setString("Normal: " + std::to_string(cont_normal));
        if (txt_enfermo) std::dynamic_pointer_cast<Texto>(txt_enfermo)->setString("Enfermo: " + std::to_string(cont_enfermo));
        if (txt_recuperacion) std::dynamic_pointer_cast<Texto>(txt_recuperacion)->setString("En recuperacion: " + std::to_string(cont_recuperacion));
        if (txt_inmune) std::dynamic_pointer_cast<Texto>(txt_inmune)->setString("Inmune: " + std::to_string(cont_inmune));
        if (txt_muerto) std::dynamic_pointer_cast<Texto>(txt_muerto)->setString("Muerto: " + std::to_string(cont_muerto));

        if (fin_juego) {
            if (txt_status) std::dynamic_pointer_cast<Texto>(txt_status)->setString(cont_muerto == 225 ? "TODOS MUERTOS" : "EPIDEMIA ERRADICADA");
        }
    }

    void Escena_Epidemia::onInputs(const CE::Botones& accion){
        (void)accion;
    }


    void Escena_Epidemia::onRender()
    {
        for(auto& obj: objetos.getPool()) {
            CE::Render::Get().AddToDraw(*obj);
        }
    }
}
