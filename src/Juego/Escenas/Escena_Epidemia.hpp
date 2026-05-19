#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <memory>

namespace IVJ
{
    class Escena_Epidemia : public CE::Escena
    {
        public:
            explicit Escena_Epidemia();
            virtual ~Escena_Epidemia(){};

            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
            std::shared_ptr<Entidad> getJugador() override { return nullptr; }
       
        private:
            std::shared_ptr<CE::Objeto> txt_normal;
            std::shared_ptr<CE::Objeto> txt_enfermo;
            std::shared_ptr<CE::Objeto> txt_recuperacion;
            std::shared_ptr<CE::Objeto> txt_inmune;
            std::shared_ptr<CE::Objeto> txt_muerto;
            std::shared_ptr<CE::Objeto> txt_status;
            bool fin_juego{false};
            int inicializar{1};
    };
}