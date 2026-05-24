#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace IVJ {
    class Escena_Cutscene : public CE::Escena {
    public:
        Escena_Cutscene();
        ~Escena_Cutscene() override = default;

        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;

    private:
        sf::Texture textura_actual;
        sf::Sprite sprite_video;
        
        float tiempo_acumulado;
        int frame_actual;
        float frame_rate;
        bool termino_video;
    };
}
