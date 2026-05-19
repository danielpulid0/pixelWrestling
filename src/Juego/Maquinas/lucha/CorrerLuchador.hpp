#pragma once
#include"../FSM.hpp"

namespace IVJ
{
    class CorrerLuchador : public FSM
    {
        public:
            CorrerLuchador(int max_frames = 4, float frame_rate = 0.1f);
            ~CorrerLuchador() override{};

            FSM* onInputs(const CE::IControl& control) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;

        private:
            sf::Sprite *sprite;
            int s_w, s_h;
            float max_tiempo, act_tiempo;
            int max_frames, id_frame;
            const Entidad* m_ente;
    };
}
