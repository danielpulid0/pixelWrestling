#pragma once
#include"../FSM.hpp"
namespace IVJ
{
    class IdleLucha : public FSM
    {
        public:
            // Soporta animación de múltiples frames en idle.
            explicit IdleLucha(int max_frames = 1, float frame_rate = 0.25f);
            ~IdleLucha() override{};
            FSM* onInputs(const CE::IControl& control)override;
            void onEntrar(const Entidad& obj)override;
            void onSalir(const Entidad& obj)override;
            void onUpdate(const Entidad& obj,float dt)override;
        private:
            sf::Sprite *sprite;
            int s_w;
            int s_h;
            int max_frames;
            int id_frame;
            float max_frame_time;
            float cur_frame_time;
            const Entidad* m_ente{nullptr};
    };
}