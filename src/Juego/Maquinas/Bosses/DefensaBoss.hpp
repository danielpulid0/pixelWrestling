#pragma once
#include "FSMBoss.hpp"
namespace IVJ
{
    class DefensaBoss : public FSMBoss
    {
        public:
            DefensaBoss(float duracion = 1.0f);
            ~DefensaBoss() override{};
            FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite *sprite;
            int t_w, t_h;
            float duracion;
            float timer;
    };
}
