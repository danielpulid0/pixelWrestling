#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/EnteVibora.hpp>
#include <memory>

namespace IVJ
{
    class Escena_Snake : public CE::Escena
    {
        public:
            explicit Escena_Snake();
            virtual ~Escena_Snake(){};

            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
            std::shared_ptr<Entidad> getJugador() override { return nullptr; }
       
        private:
            int inicializar{1};
            std::shared_ptr<IVJ::EnteVibora> vibora;

    };
}