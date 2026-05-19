#pragma once
#include <Motor/Componentes/IComponentes.hpp>
#include <Juego/Componentes/ICParte.hpp>
#include <vector>
#include <memory>

namespace IVJ {
    class ICPartesCuerpo : public CE::IComponentes {
    public:
        ICPartesCuerpo(int w, int h) : width(w), height(h) {}
        ~ICPartesCuerpo() override = default;

        std::shared_ptr<CE::IComponentes> clonar() const override {
            return std::make_shared<ICPartesCuerpo>(*this);
        }

        // 1) vector de componentes ICParte
        std::vector<std::shared_ptr<ICParte>> partes;
        // 2 y 3) width y height
        int width;
        int height;
    };
}
