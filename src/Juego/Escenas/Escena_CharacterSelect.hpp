#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <vector>
#include <memory>
#include <string>
#include <Juego/objetos/Entidad.hpp>

namespace IVJ
{
    class Texto;
    
    struct PersonajeSelectInfo {
        std::string textura_id;
        std::string archivo_imagen;
        std::string nombre_mostrar;
        bool desbloqueado;
    };

    enum class EstadoSeleccion { ELIGIENDO_JUGADOR, ELIGIENDO_RIVAL };

    class Escena_CharacterSelect : public CE::Escena
    {
        public:
            explicit Escena_CharacterSelect();
            virtual ~Escena_CharacterSelect(){};
            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
            
        private:
            int inicializar{1};
            unsigned int opcion_index{0};
            EstadoSeleccion estado_actual{EstadoSeleccion::ELIGIENDO_JUGADOR};
            int seleccion_jugador{-1};
            std::vector<PersonajeSelectInfo> personajes;
            
            // UI elements
            std::shared_ptr<IVJ::Texto> titulo;
            std::shared_ptr<IVJ::Texto> nombre_personaje_txt;
            std::shared_ptr<IVJ::Rectangulo> cursor_rect;
            std::vector<std::shared_ptr<Entidad>> entidades_retratos;
            
            void actualizarUI();
    };
}
