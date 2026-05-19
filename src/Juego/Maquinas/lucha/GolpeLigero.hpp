#pragma once
#include"../FSM.hpp"
namespace IVJ
{
    class GolpeLigero : public FSM
    {
        public:
            GolpeLigero(int max_frames, float frame_rate);
            ~GolpeLigero() override{};
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
