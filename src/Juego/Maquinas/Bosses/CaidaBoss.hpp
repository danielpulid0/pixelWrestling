#pragma once
#include "FSMBoss.hpp"

namespace IVJ
{
    class CaidaBoss : public FSMBoss
    {
    public:
        explicit CaidaBoss(int max_frames = 4, float frame_rate = 0.15f);
        ~CaidaBoss() override = default;

        FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
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
        float timer_suelo;
        static constexpr float TIEMPO_SUELO = 2.0f;
    };
}
