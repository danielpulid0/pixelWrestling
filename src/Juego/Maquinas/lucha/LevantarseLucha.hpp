#pragma once
#include "../FSM.hpp"

namespace IVJ
{
    class LevantarseLucha : public FSM
    {
    public:
        explicit LevantarseLucha(int max_frames = 3, float frame_rate = 0.15f);
        ~LevantarseLucha() override = default;

        FSM* onInputs(const CE::IControl& control) override;
        void onEntrar(const Entidad& obj) override;
        void onSalir(const Entidad& obj) override;
        void onUpdate(const Entidad& obj, float dt) override;

    private:
        sf::Sprite* sprite;
        int s_w;
        int s_h;
        int max_frames;
        int id_frame;
        float max_frame_time;
        float cur_frame_time;
        bool animacion_terminada;
    };
}
