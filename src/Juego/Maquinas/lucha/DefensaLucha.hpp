#pragma once
#include"../FSM.hpp"
namespace IVJ
{
    class DefensaLucha : public FSM
    {
        public:
            DefensaLucha();
            ~DefensaLucha() override{};
            FSM* onInputs(const CE::IControl& control) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            sf::Sprite *sprite;
            int s_w, s_h;
            const Entidad* m_ente;
    };
}
