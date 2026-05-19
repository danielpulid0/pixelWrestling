#pragma once
#include "FSMBoss.hpp"

namespace IVJ
{
    class GolpeadoBoss : public FSMBoss
    {
    public:
        explicit GolpeadoBoss(int max_frames = 1, float frame_rate = 0.2f);
        ~GolpeadoBoss() override = default;

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
    };
}
