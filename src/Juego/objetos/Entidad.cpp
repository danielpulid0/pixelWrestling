#include "Entidad.hpp"
#include "../Maquinas/Bosses/FSMBoss.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../Maquinas/lucha/GolpeadoLucha.hpp"
#include "../Maquinas/Bosses/GolpeadoBoss.hpp"
#include "../Maquinas/lucha/CaidaLucha.hpp"
#include "../Maquinas/Bosses/CaidaBoss.hpp"
#include <memory>

namespace IVJ
{
    static int num_cpy = 0;
    Entidad::Entidad(const Entidad &cpy) : CE::Objeto{cpy}
    {
        nombre->nombre = cpy.getNombre()->nombre + "_" + std::to_string(++num_cpy);
        //copia profunda de los componentes
        //ya que no queremos la referencia si no un componente nuevo
        for (auto &comp : cpy.componentes)
        {
            componentes.push_back(comp->clonar());
        }
    }
    
    void Entidad::onUpdate(float dt) {
        //CE::Objeto::onUpdate(dt);

        //revisar si tiene ciertos componentes
        if(tieneComponente<ITriangulo>()){
            auto fig = getComponente<ITriangulo>();
            auto pos = getTransformada()->posicion;
            //dibujamos el triángulo en la posición correcta
            fig->tri_shape.setPosition({pos.x,pos.y});
            //aplicamos rotación dependiendo de la dirección
            auto r = sf::radians(fig->angulo);
            fig->tri_shape.setRotation(r);
        }
        //para actualizarlos debidamente
        //o hacer una clase hija y ahí revisar        
        //actualizar el angulo y posición del sprite
        if(tieneComponente<CE::ISprite>())
        {
            auto sprite = getComponente<CE::ISprite>();
            auto pos = getTransformada()->posicion;
            sprite->m_sprite.setPosition({pos.x,pos.y});
            auto angulo = sf::radians(getTransformada()->angulo);
            sprite->m_sprite.setRotation(angulo);
        }

        if(tieneComponente<CE::ISprite>()){
            auto sprite = getComponente<CE::ISprite>();
            auto pos = getTransformada()->posicion;
            sprite->m_sprite.setPosition({pos.x,pos.y});
        }
        //if(tieneComponente<CE::ISprite>() && tieneComponente<CE::IBoundingBox>())
        //{
        //    auto sprite = getComponente<CE::ISprite>();
        //    auto bound = getComponente<CE::IBoundingBox>();
        //    //sprite->m_sprite.setOrigin({bound->mitad.x,bound->mitad.y});
        //}

        // si tiene un FSM actualizar el estado
        if(tieneComponente<IVJ::IMaquinaEstado>())
        {
            auto mq = getComponente<IVJ::IMaquinaEstado>();
            if(mq->fsm)
                mq->fsm->onUpdate(*this,dt);
        }
        transform->pos_prev =  transform->posicion;
    }

    void Entidad::inputFSM()
    {
        if(!getComponente<IMaquinaEstado>()
        || !getComponente<IMaquinaEstado>()->fsm)
            return;
        auto mq = getComponente<IMaquinaEstado>();
        
        auto combate = getComponente<ICombate>();
        if(combate) {
            //si está defendiendo, NO interrumpir con golpeado/caida
            //el sistema de defensa ya maneja la reducción de daño
            auto defensa = getComponente<IDefensa>();
            bool defendiendo = (defensa && defensa->defendiendo);
            
            //prioridad: caída > golpeado
            if(combate->en_caida && !defendiendo) {
                combate->en_caida = false;
                combate->fue_golpeado = false; //consumir también
                
                bool es_boss = (mq->fsm->getNombre().find("Boss") != std::string::npos);
                if(es_boss) {
                    setFSM(std::make_shared<IVJ::CaidaBoss>());
                } else {
                    setFSM(std::make_shared<IVJ::CaidaLucha>());
                }
                return;
            }
            
            if(combate->fue_golpeado && !defendiendo) {
                combate->fue_golpeado = false;
                
                bool es_boss = (mq->fsm->getNombre().find("Boss") != std::string::npos);
                if(es_boss) {
                    setFSM(std::make_shared<IVJ::GolpeadoBoss>(1, 0.2f));
                } else {
                    setFSM(std::make_shared<IVJ::GolpeadoLucha>(1, 0.2f));
                }
                return;
            }
            
            //consumir flags si estaba defendiendo (el daño ya se redujo)
            if(defendiendo) {
                combate->fue_golpeado = false;
                combate->en_caida = false;
            }
        }
        //Lab 8 Animaciones: Pequeño facade usando el nombre del nodo
        
        
        if(mq->fsm->getNombre().find("Boss")!=std::string::npos)
        {
            auto target = getComponente<ITarget>()->pos;
            //usar el nodo correcto de boss
            auto n_estado = std::static_pointer_cast<FSMBoss>(mq->fsm)->onInputs(*this,*target);
            std::shared_ptr<FSM> estado(n_estado);
            if(estado)
                setFSM(estado);
        }else
        {
            if(!getComponente<CE::IControl>())  return;
            auto control = *getComponente<CE::IControl>();
            auto n_estado = mq->fsm->onInputs(control);
            std::shared_ptr<FSM> estado(n_estado);
            if(estado)
                setFSM(estado);
        }
    }
    
    void Entidad::setFSM(const std::shared_ptr<FSM>& mq){
        // cuando llamamos setFSM con make_shared como parámetro
        // hay que congelar para que no siempre lo cambie
        if (getComponente<IMaquinaEstado>()->congelar)
            return;
        auto &estado_actual = getComponente<IMaquinaEstado>()->fsm;
        //transición de salida
        estado_actual->onSalir(*this);
        //transición de entrar a otro estado
        estado_actual = mq;
        estado_actual->onEntrar(*this);
    }

    void Entidad::draw(sf::RenderTarget &target, sf::RenderStates state) const{
        state.transform *= getTransform();
        #if DEBUG
        if(tieneComponente<CE::IBoundingBox>()){
            auto escala = 1;//getComponente<CE::ISprite>()->escala;
            auto bound = getComponente<CE::IBoundingBox>();
            sf::RectangleShape box{{bound->tam.x*escala,bound->tam.y*escala}};
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color::Red);
            box.setOutlineThickness(1.5f);
            box.setOrigin({bound->mitad.x*escala,bound->mitad.y*escala});
            auto pos = transform->posicion;
            box.setPosition({pos.x,pos.y});
            target.draw(box);
        }
        #endif

        if(tieneComponente<ITriangulo>()){
            auto fig = getComponente<ITriangulo>();
            target.draw(fig->tri_shape);
        }

        // ---------------------------------------------
        //revisar si tiene ciertos componentes
        //para renderizarlos debidamente
        //o hacer una clase hija y ahí revisar
        if(tieneComponente<CE::ISprite>()){
            auto sprite = getComponente<CE::ISprite>();
            
            //SHADER  NO ME GUSTA ES LENTO
            if(tieneComponente<CE::IShader>()){
                target.draw(sprite->m_sprite,&getComponente<CE::IShader>()->m_shader);
            }
            else
                target.draw(sprite->m_sprite);
        }
    }
}
