#pragma once
#include "../FSM.hpp"

namespace IVJ
{
    class CaidaLucha : public FSM
    {
    public:
        explicit CaidaLucha(int max_frames = 4, float frame_rate = 0.15f);
        ~CaidaLucha() override = default;

        FSM* onInputs(const CE::IControl& control) override;
        void onEntrar(const Entidad& obj) override;
        void onSalir(const Entidad& obj) override;
        void onUpdate(const Entidad& obj, float dt) override;

    private:
        sf::Sprite* sprite;
        int s_w;
        int s_h;
        int max_frames;
        int id_frame;
        float max_frame_time;
        float cur_frame_time;
        bool animacion_terminada;
        float timer_suelo;        // 2 segundos en el suelo
        static constexpr float TIEMPO_SUELO = 2.0f;
    };
}
