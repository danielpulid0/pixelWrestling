#pragma once
#include "FSMBoss.hpp"

namespace IVJ
{
    class CubiertoBoss : public FSMBoss
    {
        public:
            CubiertoBoss();
            ~CubiertoBoss() override = default;
            FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite* sprite;
            int t_w, t_h;
            int max_frames;
            int id_frame;
            float max_frame_time;
            float cur_frame_time;
    };
}
