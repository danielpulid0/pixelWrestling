#pragma once
#include <Motor/Componentes/IComponentes.hpp>
#include "ICTimer.hpp"
#include "ICFigura.hpp"
#include <Motor/Utils/Vector2D.hpp>
#include <memory>

namespace IVJ {
    // 1) Una Cola (Queue) auxiliar sin librería estandar
    template<typename T, int MaxCapacity>
    class MiCola {
    
        private:
            T datos[MaxCapacity];
            int inicio;
            int fin;
            int num_elementos;
    
        public:
            MiCola() : inicio(0), fin(0), num_elementos(0) {}
        
            void push(const T& valor) {
                if (num_elementos < MaxCapacity) {
                    datos[fin] = valor;
                    fin = (fin + 1) % MaxCapacity;
                    num_elementos++;
                }
            }

            void pop() {
                if (num_elementos > 0) {
                    inicio = (inicio + 1) % MaxCapacity;
                    num_elementos--;
                }
            }
        
            T front() const { 
                return datos[inicio]; 
            }
        
            bool empty() const { 
                return num_elementos == 0; 
            }
    };

    class ICParte : public CE::IComponentes {
        public:
            ICParte(std::shared_ptr<CE::Objeto> f, int retrasoFrames) {
                parte = std::make_shared<ICFigura>(f);
                pos = std::make_shared<CE::ITransform>();
                timer = std::make_shared<ICTimer>(retrasoFrames); 
                hacerAccion = false;                              
            }
            ~ICParte() override = default;

            std::shared_ptr<CE::IComponentes> clonar() const override {
                return std::make_shared<ICParte>(*this);
            }

            MiCola<CE::Vector2D, 1000> posiciones; 
            std::shared_ptr<ICFigura> parte;      
            std::shared_ptr<CE::ITransform> pos;   
            std::shared_ptr<ICTimer> timer;
            bool hacerAccion;
    };
}
