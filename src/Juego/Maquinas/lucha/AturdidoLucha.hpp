#pragma once
#include"../FSM.hpp"
namespace IVJ
{
    class AturdidoLucha : public FSM
    {
        public:
            AturdidoLucha(float duracion = 2.0f);
            ~AturdidoLucha() override{};
            FSM* onInputs(const CE::IControl& control) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite *sprite;
            int s_w, s_h;
            float duracion;
            float timer;
            int id_frame{0};
            float frame_time{0.3f};
            float cur_frame_time{0.3f};
    };
}
