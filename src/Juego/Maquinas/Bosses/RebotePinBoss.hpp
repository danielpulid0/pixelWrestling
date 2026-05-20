#pragma once
#include "FSMBoss.hpp"

namespace IVJ
{
    class RebotePinBoss : public FSMBoss
    {
        public:
            RebotePinBoss(float start_x, float start_y, float dir_x);
            ~RebotePinBoss() override = default;
            FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite* sprite;
            int t_w, t_h;
            float start_x;
            float start_y;
            float dir_x;
            float timer;
            float max_time;
            Entidad* m_ente;
    };
}
