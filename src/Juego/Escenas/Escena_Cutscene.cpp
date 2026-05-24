#include "Escena_Cutscene.hpp"
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/Escenas/EscenaMatch.hpp>
#include <cstdio>
#include <iostream>

namespace IVJ {

    Escena_Cutscene::Escena_Cutscene() 
        : CE::Escena(), sprite_video(textura_actual), tiempo_acumulado(0.f), frame_actual(1), frame_rate(15.f), termino_video(false) {
    }

    void Escena_Cutscene::onInit() {
        tiempo_acumulado = 0.f;
        frame_actual = 1;
        termino_video = false;

        // Registrar controles para saltar
        registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
        registrarBotones(sf::Keyboard::Scancode::Escape, "escape");

        // Cargar y reproducir el audio
        CE::GestorAssets::Get().agregarMusica("historia_audio", ASSETS "/sonidos/historia.ogg");
        CE::GestorAssets::Get().getMusica("historia_audio").setLooping(false);
        CE::GestorAssets::Get().getMusica("historia_audio").play();

        // Cargar primer fotograma
        char buffer[256];
        snprintf(buffer, sizeof(buffer), ASSETS "/video/frame_%03d.png", frame_actual);
        
        if (textura_actual.loadFromFile(buffer)) {
            sprite_video.setTexture(textura_actual, true);
            sprite_video.setPosition({0.f, 0.f});
        } else {
            std::cerr << "[Cutscene] No se pudo cargar el primer fotograma: " << buffer << std::endl;
            termino_video = true;
        }
    }

    void Escena_Cutscene::onFinal() {
        CE::GestorAssets::Get().getMusica("historia_audio").stop();
    }

    void Escena_Cutscene::onUpdate(float dt) {
        if (termino_video) {
            CE::GestorEscenas::Get().cambiarEscena("Match");
            return;
        }

        tiempo_acumulado += dt;
        int expected_frame = static_cast<int>(tiempo_acumulado * frame_rate) + 1;

        if (expected_frame > frame_actual) {
            frame_actual = expected_frame;
            
            char buffer[256];
            snprintf(buffer, sizeof(buffer), ASSETS "/video/frame_%03d.png", frame_actual);
            
            // Intenta cargar la siguiente imagen
            if (textura_actual.loadFromFile(buffer)) {
                // Actualizar sprite con nueva textura, importante para mantener posibles ajustes de tamano
                sprite_video.setTexture(textura_actual, true);
            } else {
                // Si la imagen falla (porque se acabaron los fotogramas o no existe)
                std::cout << "[Cutscene] Fin del video en el frame " << frame_actual << std::endl;
                termino_video = true;
            }
        }
    }

    void Escena_Cutscene::onInputs(const CE::Botones& accion) {
        if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
            if (accion.getNombre() == "aceptar" || accion.getNombre() == "escape") {
                termino_video = true; // Saltara en el proximo onUpdate
            }
        }
    }

    void Escena_Cutscene::onRender() {
        if (!termino_video) {
            CE::Render::Get().AddToDraw(sprite_video);
        }
    }

}
