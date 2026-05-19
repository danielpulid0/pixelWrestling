#pragma once
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Primitivos/Objetos.hpp>
#include <memory>

namespace IVJ {
    class ICFigura : public CE::IComponentes {
    public:
        ICFigura(std::shared_ptr<CE::Objeto> f) : figura(f) {}
        ~ICFigura() override = default;
        
        std::shared_ptr<CE::IComponentes> clonar() const override {
            return std::make_shared<ICFigura>(*this);
        }

        std::shared_ptr<CE::Objeto> figura;
    };
}