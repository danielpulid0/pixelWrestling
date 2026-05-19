#pragma once
#include"../FSM.hpp"
namespace IVJ
{
    class AtaqueSilla : public FSM
    {
        public:
            AtaqueSilla(int max_frames, float frame_rate);
            ~AtaqueSilla() override{};
            FSM* onInputs(const CE::IControl& control) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite *sprite;
            int s_w, s_h;
            int max_frames;
            int id_frame;
            float max_frame_time;
            float cur_frame_time;
            bool animacion_terminada{false};
    };
}
