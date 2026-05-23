#pragma once
#include "FSMBoss.hpp"
namespace IVJ
{
    class AturdidoBoss : public FSMBoss
    {
        public:
            AturdidoBoss(float duracion = 2.0f);
            ~AturdidoBoss() override{};
            FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite *sprite;
            int t_w, t_h;
            float duracion;
            float timer;
            int id_frame{0};
            float frame_time{0.3f};
            float cur_frame_time{0.3f};
    };
}
