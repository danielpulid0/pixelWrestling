#pragma once
#include <Motor/Componentes/IComponentes.hpp>

namespace IVJ {
    class ICTimer : public CE::IComponentes {
    public:
        ICTimer(int mx_frames) : curr_frame(0), max_frame(mx_frames) {}
        ~ICTimer() override = default;

        std::shared_ptr<CE::IComponentes> clonar() const override {
            return std::make_shared<ICTimer>(*this);
        }

        int curr_frame;
        int max_frame;
    };
} 
