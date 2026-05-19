#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/objetos/Entidad.hpp>

namespace IVJ {
    class EscenaMatch: public CE::Escena{
        public:
            explicit EscenaMatch(std::shared_ptr<Entidad>& pref);
            virtual ~EscenaMatch(){};
            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
            std::shared_ptr<Entidad> getJugador() override {return jugador_ref;}
        private:
            int inicializar{1};
            std::shared_ptr<Entidad>& jugador_ref;

            //referencia al boss para sistemas de combate
            std::shared_ptr<Entidad> boss_ref;

            //conteo (pinfall)
            IConteo conteo_boss;
            IConteo conteo_jugador;
            bool match_terminado{false};
            bool jugador_gano{false};

            //item spawn
            float item_timer{0.f};
            static constexpr float ITEM_INTERVAL = 30.f;
            std::shared_ptr<Entidad> silla_ref{nullptr};
            bool mostrar_usar_silla{false};

            //doble-tap para correr
            float tap_timer_der{0.f};
            float tap_timer_izq{0.f};
            int tap_count_der{0};
            int tap_count_izq{0};
            static constexpr float TAP_WINDOW = 0.3f;

            //HUD helpers
            void dibujarBarraHP(Entidad& ente, float x, float y,
                               const sf::Color& color, const sf::Color& fondo, bool inverso=false);
            void dibujarMomentum(Entidad& ente, float x, float y, bool inverso=false);
            void dibujarConteo(int conteo, float x, float y);
            void dibujarBarraEscape(float progreso, float x, float y);

            // Shaders
            std::unique_ptr<CE::IShader> momentum_shader;
            float tiempo_shader{0.f};

            //shader de parpadeo al bloquear golpe
            float flash_intensity_jug{0.f};
            float flash_intensity_boss{0.f};
    };
}