#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <vector>
#include <memory>

namespace IVJ
{
    class Texto;
    class Escena_Menu : public CE::Escena
    {
        public:
            explicit Escena_Menu();
            virtual ~Escena_Menu(){};
            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
        private:
            int inicializar{1};
            float salto;
            unsigned int opcion;
            std::shared_ptr<IVJ::Texto> titulo;
            std::shared_ptr<IVJ::Triangulo> marcador;
            std::shared_ptr<Texto> opcion1;
            std::shared_ptr<Texto> opcion2;
            std::shared_ptr<Texto> opcion3;
            std::shared_ptr<Texto> opcion4;
    };
}