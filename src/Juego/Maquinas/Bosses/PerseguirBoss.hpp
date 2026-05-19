#pragma once
#include"FSMBoss.hpp"
namespace IVJ
{
    class PerseguirBoss : public FSMBoss
    {
        public:
            explicit PerseguirBoss(int max_frames, float frame_rate, float velocidad = 60.f);
            FSM* onInputs(Entidad& parent, CE::Vector2D& target) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj, float dt) override;
        private:
            int max_frames;
            int id_frame;
            float max_frame_time;
            float cur_frame_time;
            int t_w, t_h;
            float velocidad;
    };
}
