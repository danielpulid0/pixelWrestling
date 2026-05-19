#include "Escena_SS.hpp"
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Render/Render.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/GUI/GLogger.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <memory>
#include <fstream>
#include <iostream>
#include <cstdint>


namespace IVJ{
    Escena_SS::Escena_SS(std::shared_ptr<Entidad>& pref) : CE::Escena{}, jugador_ref{pref}{
    }

    void Escena_SS::onInit(){
        if(!inicializar)
    return;

  auto ptr_int = std::make_shared<int>();
  *ptr_int = 0;
  std::cout << ptr_int << ": " << *ptr_int << "\n";

  std::ifstream miarchivo{ASSETS "/config/figuras.txt"};
  std::string tipo;
        float px,py;
        float w,h,radio, lado;
        int fr,fg,fb,cr,cg,cb;

        while (miarchivo >> tipo){

            if (tipo == "Rectangulo"){
      miarchivo >> px >> py >> w >> h >> fr >> fg >> fb >> cr >> cg >> cb;
                auto nuevaFigura = std::make_shared<Rectangulo>(w, h,
                    sf::Color(fr,fg,fb,255),   
                    sf::Color(cr,cg,cb,255));
                nuevaFigura->setPosicion(px,py);

                nuevaFigura->addComponente(std::make_shared<IUpDown>(
                    200.f,    
                    -70.f,          
                    +230.f           
                ));
      objetos.agregarPool(nuevaFigura);
    }

            else if (tipo == "Circulo"){
      miarchivo >> px >> py >> radio >> fr >> fg >> fb >> cr >> cg >> cb;
                auto nuevaFigura = std::make_shared<Circulo>(radio,
                    sf::Color(fr,fg,fb,255),
                    sf::Color(cr,cg,cb,255));
                nuevaFigura->setPosicion(px,py);
                nuevaFigura->addComponente(std::make_shared<IGirar>(
                    3.1416f,    
                    5.5f          
                ));
      objetos.agregarPool(nuevaFigura);
    }

            else if (tipo == "Triangulo"){
      miarchivo >> px >> py >> lado >> fr >> fg >> fb >> cr >> cg >> cb;
                auto nuevaFigura = std::make_shared<Triangulo>(lado,
                    sf::Color(fr,fg,fb,255),
                    sf::Color(cr,cg,cb,255));
                nuevaFigura->setPosicion(px,py);
                nuevaFigura->addComponente(std::make_shared<IOnda>(
                    50.f,    
                    4.f,     
                    80.f,   
                    px-250.f, 
                    px+350.f  
                ));
      objetos.agregarPool(nuevaFigura);
    }

    else
      continue;
  }

        inicializar=0;
}

    void Escena_SS::onFinal(){}

    void Escena_SS::onUpdate(float dt){   
        for (auto&obj : objetos.getPool()){
    obj->onUpdate(dt);
    SistemaUpDown(*obj, dt);
    SistemaGirar(*obj, dt);
    SistemaOnda(*obj, dt);
  }
    }

    void Escena_SS::onInputs(const CE::Botones& accion){
        (void)accion;
    }

    void Escena_SS::onRender(){
        for (auto&obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
}
}