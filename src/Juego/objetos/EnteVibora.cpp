#include "EnteVibora.hpp"
#include <Juego/Componentes/ICParte.hpp>
#include <Juego/Componentes/ICPartesCuerpo.hpp>
#include <Juego/Componentes/ICFigura.hpp>
#include <Motor/Render/Render.hpp>

namespace IVJ {

    EnteVibora::EnteVibora() {
    }

    void EnteVibora::onUpdate(float dt) {
        auto cuerpo = this->getComponente<ICPartesCuerpo>();
        if (!cuerpo || cuerpo->partes.empty()) return;

        auto& tf = this->getTransformada(); 
        if (!tf) return;
        
     
        tf->posicion.x += tf->velocidad.x * dt;
        tf->posicion.y += tf->velocidad.y * dt;
        auto windowSize = CE::Render::Get().GetVentana().getSize();
        
        if (tf->posicion.x < 0) {
            tf->posicion.x = windowSize.x;
        } else if (tf->posicion.x > windowSize.x) {
            tf->posicion.x = 0;
        }
        
        if (tf->posicion.y < 0) {
            tf->posicion.y = windowSize.y;
        } else if (tf->posicion.y > windowSize.y) {
            tf->posicion.y = 0;
        }
        
       
        auto figuraComponente = this->getComponente<ICFigura>();
        if (figuraComponente && figuraComponente->figura) {
            figuraComponente->figura->setPosicion(tf->posicion.x, tf->posicion.y);
        }

        CE::Vector2D act_pos = tf->posicion;

      
        cuerpo->partes[0]->posiciones.push(act_pos);

        for (size_t i = 0; i < cuerpo->partes.size(); i++) {
            auto& parte = cuerpo->partes[i];
            
            if (!parte->hacerAccion) {
                parte->timer->curr_frame++;
                if (parte->timer->curr_frame >= parte->timer->max_frame) {
                    parte->hacerAccion = true;
                }
            }

            if (parte->hacerAccion && !parte->posiciones.empty()) {
                CE::Vector2D pos_destino = parte->posiciones.front();
                parte->posiciones.pop();
                
                CE::Vector2D mi_pos_anterior = parte->pos->posicion;
                
                if (i + 1 < cuerpo->partes.size()) {
                     cuerpo->partes[i + 1]->posiciones.push(mi_pos_anterior);
                }
                
                parte->pos->posicion = pos_destino;
                
                if (parte->parte && parte->parte->figura) {
                    parte->parte->figura->setPosicion(pos_destino.x, pos_destino.y);
                }
            }
        }
    }

}
