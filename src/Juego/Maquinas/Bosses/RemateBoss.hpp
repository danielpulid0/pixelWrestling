#pragma once
#include "FSMBoss.hpp"
namespace IVJ
{
    class RemateBoss : public FSMBoss
    {
        public:
            RemateBoss(int max_frames, float frame_rate);
            ~RemateBoss() override{};
            FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite *sprite;
            int t_w, t_h;
            int max_frames;
            int id_frame;
            float max_frame_time;
            float cur_frame_time;
            bool animacion_terminada{false};
    };
}
