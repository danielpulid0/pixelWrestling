#pragma once
#include "../FSM.hpp"

namespace IVJ
{
    class RebotePinLucha : public FSM
    {
        public:
            RebotePinLucha(float start_x, float start_y, float dir_x);
            ~RebotePinLucha() override = default;
            FSM* onInputs(const CE::IControl& control) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite* sprite;
            int s_w, s_h;
            float start_x;
            float start_y;
            float dir_x;
            float timer;
            float max_time;
            Entidad* m_ente;
    };
}
