#include "Sistemas.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include "../objetos/Entidad.hpp"
#include <math.h>
#include "../../Motor/Utils/Lerp.hpp"
#include "../../Motor/Render/Render.hpp"
#include <memory>
#include <cstdlib>
#include "../Componentes/IJComponentes.hpp"
#include "../Figuras/Figuras.hpp"


namespace IVJ
{
    void SistemaControl(CE::Objeto&ente, float dt)
    {
        //no hay necesidad verificar si tiene, se asume que tiene
        //control y transformada
        auto p = ente.getTransformada();
        auto c = ente.getComponente<CE::IControl>();
        (void)dt;
        if(c->arr)
            p->velocidad.y=-200;
        if(c->abj)
            p->velocidad.y=200;
        if(c->der)
            p->velocidad.x=200;
        if(c->izq)
            p->velocidad.x=-200;
        if(c->run)
            p->velocidad.x = 200.f; // Valor fijo en lugar de escalar acumulativamente

    }

    void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt){
        for(auto& ente : entes){
            //todo ente tiene ITransform por lo que no requiere verificación
            auto trans = ente->getTransformada();
            trans->posicion.suma(trans->velocidad.escala(dt));
        }
    }
    
    void SistemaMover(const std::shared_ptr<CE::Objeto>& objeto, float dt){
        auto trans = objeto->getTransformada();
        auto control = objeto->getComponente<CE::IControl>();

         //Si no tiene control o el control no esta activo salir
        if(!control || !control->isActivo()) return;

        //definimos cual va ser la velocidad correcta dependiendo
        //de que boton del control este presionado
        auto vel = CE::Vector2D{0.f,0.f};
        if(control->arr) vel.y = -trans->velocidad.y;
        if(control->abj) vel.y = trans->velocidad.y;
        if(control->der) vel.x = trans->velocidad.x;
        if(control->izq) vel.x = -trans->velocidad.x;
        
        // 2. Lógica de rotación (Solo para la nave o figuras geométricas)
        if(objeto->getComponente<ITriangulo>() || objeto->getComponente<CE::ISprite>())
        {
            auto n = vel;
            n.normalizacion();
            auto trian = objeto->getComponente<ITriangulo>();//nulo 
            if (vel.x!=0 || vel.y!=0){
                if(trian)
                    trian->angulo = std::atan2(n.x,-n.y);
                trans->angulo = std::atan2(n.x,-n.y);
            }
        }

        // 3. ACTUALIZACIÓN DE POSICIÓN (Debe estar afuera de todos los IF)
        // actualizamos la posición del ente en el mundo físicamente.
        trans->posicion.suma(vel.escala(dt));
    }
  
    void SistemaMover2(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt){
        for(auto& ente : entes){
            //todo ente tiene ITransform por lo que no requiere verificación
            auto trans = ente->getTransformada();
            trans->posicion.suma(trans->velocidad.escala(dt));
        }
    }
    
    void SistemaMover2(const std::shared_ptr<CE::Objeto>& objeto, float dt){
        auto trans = objeto->getTransformada();
        auto control = objeto->getComponente<CE::IControl>();

         //Si no tiene control o el control no esta activo salir
        if(!control || !control->isActivo()) return;

        //definimos cual va ser la velocidad correcta dependiendo
        //de que boton del control este presionado
        auto vel = CE::Vector2D{0.f,0.f};
        if(control->arr) vel.y = -trans->velocidad.y;
        if(control->abj) vel.y = trans->velocidad.y;
        if(control->der) vel.x = trans->velocidad.x;
        if(control->izq) vel.x = -trans->velocidad.x;
        
        // 2. Lógica de rotación (Solo para la nave o figuras geométricas)
        if(objeto->getComponente<ITriangulo>() || objeto->getComponente<CE::ISprite>())
        {
            auto n = vel;
            n.normalizacion();
            auto trian = objeto->getComponente<ITriangulo>();//nulo 
            if (vel.x!=0 || vel.y!=0){
                if(trian)
                    trian->angulo = std::atan2(n.x,-n.y);
                //trans->angulo = std::atan2(n.x,-n.y);
            }
        }

        // 3. ACTUALIZACIÓN DE POSICIÓN (Debe estar afuera de todos los IF)
        // actualizamos la posición del ente en el mundo físicamente.
        trans->posicion.suma(vel.escala(dt));
    }

    bool SistemaColAABB(CE::Objeto& A, CE::Objeto& B, bool resolucion)
    {
        if(!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
            return false;
        auto bA = A.getComponente<CE::IBoundingBox>()->tam;
        auto mA = A.getComponente<CE::IBoundingBox>()->mitad;
        auto *pa = &A.getTransformada()->posicion;
        auto prevA = A.getTransformada()->pos_prev;
        auto bB = B.getComponente<CE::IBoundingBox>()->tam;
        auto mB = B.getComponente<CE::IBoundingBox>()->mitad;
        auto pb = B.getTransformada()->posicion;
        //calculos
        bool H = pa->y-mA.y < pb.y+bB.y - mB.y && pb.y-mB.y < pa->y+bA.y - mA.y;
        bool V = pa->x-mA.x < pb.x+bB.x - mB.x && pb.x-mB.x < pa->x+bA.x - mA.x;
        bool hay_colision = H && V;
        if(hay_colision && resolucion)
            *pa = prevA;
        return hay_colision;
    }

    bool SistemaColAABBMid(CE::Objeto& A, CE::Objeto& B, bool resolucion)
    {
        if(!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
            return false;
        auto midA = A.getComponente<CE::IBoundingBox>()->mitad;
        auto *pa = &A.getTransformada()->posicion;
        auto prevA = A.getTransformada()->pos_prev;
        auto midB = B.getComponente<CE::IBoundingBox>()->mitad;
        auto *pb = &B.getTransformada()->posicion;
        //calculos
        float dX = std::abs(pb->x - pa->x);
        float dY = std::abs(pb->y - pa->y);
        float sumMidX = midA.x + midB.x;
        float sumMidY = midA.y + midB.y;
        bool V = sumMidX - dX > 0;
        bool H = sumMidY - dY > 0;
        bool hay_colision = V && H;
        if(resolucion && hay_colision)
            *pa = prevA;
        return hay_colision;


    }

    bool checkRayoHit(CE::Objeto& npc, CE::Vector2D& p1, CE::Vector2D& p2)
    {
        if(!npc.tieneComponente<CE::IBoundingBox>()) return false;
        
        auto midnpc = npc.getComponente<CE::IBoundingBox>()->mitad;
        auto posnpc = npc.getTransformada()->posicion;
        auto linea = (p2-p1);//.escala(1.f);
        //std::cout<<"Mid linea: "<<midlinea<<"\n";
        float dx = std::abs(posnpc.x - p1.x);
        float dy = std::abs(posnpc.y - p1.y);
        float sumMidX = midnpc.x + std::abs(linea.x) ;
        float sumMidY = midnpc.y + std::abs(linea.y) ;
        
        return (sumMidX - dx >0) && (sumMidY - dy >0);
        
    }

    void SistemaSpawn(CE::Pool& spawns)
    {
        for(auto &s : spawns.getPool())
        {
            auto respawndata = s->getComponente<CE::IRespawn>();
            if(!respawndata || respawndata->curr_spawn >= respawndata->max_spawn)
                continue;
            //respawn
            if(respawndata->timer_actual >= respawndata->timer_maximo)
            {
                //crear objeto
                auto malillo = std::make_shared<Entidad>(); //(*(respawndata->prefab));
                //auto comp = respawndata->prefab->getListaComponentes();
                //auto comp = respawndata->prefab->getComponente<CE::ISprite>();
                //for(auto &c : comp)
                //{
                    //malillo->copyComponente<CE::ISprite>(comp);
                    //malillo->copyComponente<CE::IPaths>(respawndata->prefab->getComponente<CE::IPaths>());
                //}
                 
                malillo->addComponente(std::make_shared<CE::IPaths>(60))
                .addComponente(std::make_shared<CE::ISprite>(
                            CE::GestorAssets::Get().getTextura("hoja_blue"),68,91,1.f))
                .addComponente(std::make_shared<CE::IShader>("",ASSETS "/shaders/contorno.frag"));
                malillo->getComponente<CE::IPaths>()->addCurva(
                        CE::Vector2D{500.f,300.f},CE::Vector2D{600.f,500.f},CE::Vector2D{800.f,300.f});
                malillo->getComponente<CE::IPaths>()->addCurva(
                        CE::Vector2D{800.f,300.f},CE::Vector2D{600.f,100.f},CE::Vector2D{500.f,300.f});


                //mandar datos al shader
                auto shader = malillo->getComponente<CE::IShader>();
                static sf::Texture textura=malillo->getComponente<CE::ISprite>()->m_sprite.getTexture();
                
                static sf::Glsl::Vec4 uOutlineColor{1.f,0.f,1.f,1.f};
                //static  float uEdgeThreshold= 0.15f; //0.15-0.35
                //static  float uEdgeSoftness = 0.02f;
                static  float uThickness = 0.15f;
                auto w = malillo->getComponente<CE::ISprite>()->width;
                auto h = malillo->getComponente<CE::ISprite>()->height;
                static sf::Glsl::Vec2 uTexSize{(float)w,(float)h};

                shader->setTextura("uTex",&textura);
                shader->setVector4("uOutlineColor",&uOutlineColor);
                //shader->setEscalar("uEdgeThreshold",&uEdgeThreshold);
                //shader->setEscalar("uEdgeSoftness",&uEdgeSoftness);
                shader->setEscalar("uThickness",&uThickness);
                shader->setVector2("uTexSize",&uTexSize);

                //posiciones
                malillo->getStats()->hp =100;
                //asumimos que spwan tine un cuadro , tomamos el cuadro como el area
                int size_x = respawndata->width; 
                int size_y = respawndata->height;
                float x0 = s->getTransformada()->posicion.x-size_x/2.f;
                float y0 = s->getTransformada()->posicion.y-size_y/2.f;
                float x = x0+(rand()%size_x);
                float y = y0+(rand()%size_y);
                malillo->setPosicion(x,y);
                //spawns.push_back(malillo);
                spawns.agregarPool(malillo);
                respawndata->timer_actual=0;
                respawndata->curr_spawn++;
            }
            respawndata->timer_actual++;
        }
    }
    void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>>& obj)
    {
        for(auto &o: obj)
        {
            if(o->tieneComponente<CE::IPaths>())
            {
                auto path = o->getComponente<CE::IPaths>();
                int num_curvas = path->puntos.size()/3;
                if(path->puntos.size()==0) continue;
                if(path->id_curva >= num_curvas) continue;
                float t = (path->frame_actual_curva%(path->frame_total_curva+1))/(float)path->frame_total_curva;
                //std::cout<<o<<" t: "<<t<<"\n";
                CE::Vector2D P0 = path->puntos[0+path->id_curva*(path->offset-0)];
                CE::Vector2D P1 = path->puntos[1+path->id_curva*(path->offset-0)];
                CE::Vector2D P2 = path->puntos[2+path->id_curva*(path->offset-0)];
                auto posiciones_actual = CE::lerp2(P0, P1, P2, t);
                path->frame_actual_curva++;
                o->setPosicion(posiciones_actual.x,posiciones_actual.y);
                if(t == 1.f )
                    path->id_curva++;
                    //path->frame_actual_curva=0;
                //std::cout<<"[ "<<o<<" ] "<<path->frame_actual_curva<<"\n";
                if(path->id_curva == 2 && o->tieneComponente<CE::IShader>())
                {
                    //cambiar shader
                    auto shader = o->getComponente<CE::IShader>();
                    shader->cambiarShader("",ASSETS "/shaders/color.frag");
                    static sf::Glsl::Vec4 color{0.f,1.f,0.f,1.f};
                    shader->m_vars.clear();
                    shader->setVector4("color",&color);
                }
                else
                {
                    //cambiar shader
                    auto shader = o->getComponente<CE::IShader>();
                    shader->cambiarShader("",ASSETS "/shaders/contorno.frag");
                    static sf::Glsl::Vec4 outlinecolor{0.f,1.f,0.f,1.f};
                    static float thickness = 0.1;
                    static sf::Glsl::Vec2 size{63,92};
                    shader->m_vars.clear();
                    shader->setVector4("uOutlineColor",&outlinecolor);
                    shader->setEscalar("uThickness",&thickness);
                    shader->setVector2("uTexSize",&size);
                    
                }
            }
        }
    }

    //Dialogos
    void SistemaDialogos(IVJ::IDialogo* dialogo,CE::Objeto& obj)
    {
        if(!dialogo) return;
        dialogo->activo = true;
        dialogo->onInteractuar(obj);
    }

    bool revisarDistanciaInteraccion(CE::Objeto& jugador, CE::Objeto& npc, float distancia_maxima)
    {
        auto pos_jugador = jugador.getTransformada()->posicion;
        auto pos_npc = npc.getTransformada()->posicion;
        float distancia = pos_jugador.distancia(pos_npc);
        return distancia <= distancia_maxima;
    }


    //DEBUG
    void pintarLinea(CE::Vector2D& p1, CE::Vector2D& p2, const sf::Color& color)
    {
        sf::CircleShape cp1{2.f};
        cp1.setFillColor({0,0,0,255});
        sf::CircleShape cp2{2.f};
        cp2.setFillColor({0,0,0,255});
        cp1.setPosition({p1.x,p1.y});
        cp2.setPosition({p2.x,p2.y});
        //pintar lineas de guía
        for(float t=0;t<=1;t+=0.0015)
        {
            sf::CircleShape pixel{1};
            pixel.setFillColor(color);
            auto puntopos  = CE::lerp(p1,p2,t);
            pixel.setPosition({puntopos.x,puntopos.y});
            CE::Render::Get().AddToDraw(pixel);
        }
        CE::Render::Get().AddToDraw(cp1);
        CE::Render::Get().AddToDraw(cp2);
    }

     void SistemaGirar(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IGirar>()) return;
        auto componente = ente.getComponente<IGirar>();
        auto pos = ente.getTransformada()->posicion;
        float radio = componente->radio;
        float x = radio *cos(componente->angulo);
        float y = radio *sin(componente->angulo);
        ente.setPosicion(pos.x+x,pos.y+y);
        componente->angulo+= 3.146f*dt;
    }

     void SistemaUpDown(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IUpDown>()) return;

        auto componente = ente.getComponente<IUpDown>();
        auto pos = ente.getTransformada()->posicion;
        pos.y += componente->velocidad * componente->direccion * dt;

        if (pos.y >= componente->limite_inferior) {
            pos.y = componente->limite_inferior;
            componente->direccion = -1;  
        }
        else if (pos.y <= componente->limite_superior) {
            pos.y = componente->limite_superior;
            componente->direccion = 1;  
        }
        ente.setPosicion(pos.x, pos.y);
    }
    
    void SistemaOnda(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IOnda>()) 
            return;

        auto componente = ente.getComponente<IOnda>();
        auto pos = ente.getTransformada()->posicion;

        if (componente->tiempo == 0.0f) 
            componente->pos_y_inicial = pos.y;

        componente->tiempo += dt;

        float nueva_x = pos.x + componente->velocidad_x * componente->direccion_x * dt;
        bool reboto = false;

        if (nueva_x >= componente->limite_derecho) {
            nueva_x = componente->limite_derecho;  
            componente->direccion_x = -1;          
            reboto = true;
        }
        else if (nueva_x <= componente->limite_izquierdo) {
            nueva_x = componente->limite_izquierdo; 
            componente->direccion_x = 1;            
            reboto = true;
        }

        pos.x = nueva_x;
        pos.y = componente->pos_y_inicial + componente->amplitud * sin(componente->frecuencia * componente->tiempo);       
        ente.setPosicion(pos.x, pos.y);
    }
    
    void SistemaEpidemia(CE::Pool& poblacion, float dt)
    {
        const int MAX_GRID = 15;
        std::vector<CE::Objeto*> grid(MAX_GRID * MAX_GRID, nullptr);

        for (auto& obj : poblacion.getPool()){
            if (obj->tieneComponente<IEstadoEpidemia>()){
                auto estado = obj->getComponente<IEstadoEpidemia>();
                int id = estado->grid_y * MAX_GRID + estado->grid_x;
                if (id >= 0 && id < MAX_GRID * MAX_GRID)
                    grid[id] = obj.get();
            }
        }

        for (auto& obj : poblacion.getPool()){
            if (!obj->tieneComponente<IEstadoEpidemia>()) 
                continue;
            auto estado = obj->getComponente<IEstadoEpidemia>();
            auto rect = dynamic_cast<IVJ::Rectangulo*>(obj.get());
            if (estado->estado_actual == EstadoSalud::NORMAL){
                if (estado->inmunidad_parcial > 0.0f)
                    estado->inmunidad_parcial -= dt;
            }
            else if (estado->estado_actual == EstadoSalud::ENFERMO){
                estado->tiempo_transmision += dt;
                estado->tick_segundo += dt;

                if (estado->tick_segundo >= 1.0f){
                    estado->tick_segundo -= 1.0f;
                    int dx[] = {-1, 1, 0, 0};
                    int dy[] = {0, 0, -1, 1};
                    for(int i = 0; i < 4; ++i){
                        int nx = estado->grid_x + dx[i];
                        int ny = estado->grid_y + dy[i];
                        if (nx >= 0 && nx < MAX_GRID && ny >= 0 && ny < MAX_GRID){
                            CE::Objeto* neighbor = grid[ny * MAX_GRID + nx];
                            if (neighbor && neighbor->tieneComponente<IEstadoEpidemia>()){
                                auto nestado = neighbor->getComponente<IEstadoEpidemia>();
                                if (nestado->estado_actual == EstadoSalud::NORMAL && nestado->inmunidad_parcial <= 0.0f){
                                    int proc = rand() % 100;
                                    int chance = 15 + (rand() % 16);
                                    if (proc < chance){
                                        nestado->estado_actual = EstadoSalud::ENFERMO;
                                        nestado->tiempo_transmision = 0.0f;
                                        nestado->tick_segundo = 0.0f;
                                    }
                                }
                            }
                        }
                    }
                }

                if (estado->tiempo_transmision >= 3.0f){
                    estado->estado_actual = EstadoSalud::RECUPERACION;
                    estado->tiempo_recuperacion = 2.0f;
                }
            }
            else if (estado->estado_actual == EstadoSalud::RECUPERACION){
                estado->tiempo_recuperacion -= dt;
                if (estado->tiempo_recuperacion <= 0.0f){
                    int m_proc = rand() % 100;
                    if (m_proc < 10)
                        estado->estado_actual = EstadoSalud::MUERTO;
                    else{
                        int i_proc = rand() % 100;
                        int i_chance = 1 + (rand() % 10);
                        if (i_proc < i_chance)
                            estado->estado_actual = EstadoSalud::INMUNE;
                        else{
                            estado->estado_actual = EstadoSalud::NORMAL;
                            estado->inmunidad_parcial = 2.0f;
                        }
                    }
                }
            }

            if (rect) {
                switch (estado->estado_actual) {
                    case EstadoSalud::NORMAL:
                        rect->setFillColor(sf::Color::Transparent);
                        rect->setOutlineColor(sf::Color(0, 49, 181)); 
                        break;
                    case EstadoSalud::ENFERMO:
                        rect->setFillColor(sf::Color::Red);
                        rect->setOutlineColor(sf::Color::Transparent);
                        break;
                    case EstadoSalud::RECUPERACION:
                        rect->setFillColor(sf::Color(89, 4, 133));
                        rect->setOutlineColor(sf::Color::Transparent);
                        break;
                    case EstadoSalud::INMUNE:
                        rect->setFillColor(sf::Color::Green);
                        rect->setOutlineColor(sf::Color::Transparent);
                        break;
                    case EstadoSalud::MUERTO:
                        rect->setFillColor(sf::Color::Black);
                        rect->setOutlineColor(sf::Color::Transparent);
                        break;
                }
            }
        }
    }

    void SistemaNPCLookTarget(CE::Objeto &ente, CE::Vector2D& target)
    {
        if(!ente.tieneComponente<IRangoAggro>()) return;

        auto rango = ente.getComponente<IRangoAggro>();
        if (!rango->estaDentroRango(ente,target)) return;

        auto pos = ente.getTransformada()->posicion;
        auto ente_a_target =  target-pos;
        ente.getTransformada()->angulo = atan2(
                ente_a_target.y,
                ente_a_target.x
            );
        //reflejar imagen
        if (ente_a_target.x <0)
            ente.getComponente<CE::ISprite>()->m_sprite.setScale({1,-1});
        else
            ente.getComponente<CE::ISprite>()->m_sprite.setScale({1,1});
    }

    //=== SISTEMAS DE COMBATE ===

    void SistemaDaño(Entidad& atacante, Entidad& defensor)
    {
        auto combateA = atacante.getComponente<ICombate>();
        auto combateD = defensor.getComponente<ICombate>();
        if(!combateA || !combateA->esta_atacando) return;
        if(!combateD) return;

        //no golpear a alguien que ya está en hit-stun, caído, o levantándose
        if(combateD->fue_golpeado || combateD->esta_derribado
           || combateD->en_caida || combateD->en_levantarse) return;

        //verificar rango de ataque (colisión extendida)
        if(!atacante.tieneComponente<CE::IBoundingBox>() || !defensor.tieneComponente<CE::IBoundingBox>()) return;
        auto pa = atacante.getTransformada()->posicion;
        auto pb = defensor.getTransformada()->posicion;
        auto midA = atacante.getComponente<CE::IBoundingBox>()->mitad;
        auto midB = defensor.getComponente<CE::IBoundingBox>()->mitad;
        
        float dX = std::abs(pb.x - pa.x);
        float dY = std::abs(pb.y - pa.y);
        
        // Margen de alcance (15 pixeles extra, 35 para sillazo)
        float attack_range = (combateA->tipo_ataque == 3) ? 20.f : 15.f; 
        
        if(dX > (midA.x + midB.x + attack_range) || dY > (midA.y + midB.y + 10.f)) return;

        //calcular daño base según tipo
        int danio_base = 0;
        switch(combateA->tipo_ataque) {
            case 1: danio_base = combateA->danio_base_golpe;   break;
            case 2: danio_base = combateA->danio_base_patada;  break;
            case 3: danio_base = combateA->danio_base_item;    break;
            case 4: danio_base = combateA->danio_base_remate;  break;
            default: return;
        }

        //aplicar modificadores de stats
        auto statsA = atacante.getComponente<ILuchadorStats>();
        auto statsD = defensor.getComponente<ILuchadorStats>();
        float mod_fuerza = statsA ? (statsA->fuerza / 5.0f) : 1.0f;
        float mod_resist = statsD ? (5.0f / statsD->resistencia) : 1.0f;
        float danio_final = danio_base * mod_fuerza * mod_resist;

        //reducir 90% el daño de ataques básicos (golpe y patada)
        if(combateA->tipo_ataque == 1 || combateA->tipo_ataque == 2)
            danio_final *= 0.1f;

        //defensa: si el defensor está bloqueando
        auto defensa = defensor.getComponente<IDefensa>();
        if(defensa && defensa->defendiendo)
        {
            defensa->golpes_bloqueados++;
            //activar parpadeo al bloquear
            defensa->flash_timer = 0.15f;
            CE::GestorAssets::Get().getSonido("bloqueo").play(); 

            if(defensa->golpes_bloqueados > IDefensa::MAX_BLOQUEOS) {
                //romper guardia
                combateD->esta_aturdido = true;
                combateD->tiempo_aturdido = defensa->tiempo_rotura;
                defensa->defendiendo = false;
                defensa->golpes_bloqueados = 0;
                defensa->cooldown_defensa = 5.0f;
                danio_final *= 0.5f; //medio daño al romper
            } else {
                danio_final *= 0.1f; //10% de daño al bloquear
            }
        }

        //aplicar daño
        int danio_int = (int)danio_final;
        if(danio_int < 1) danio_int = 1;
        auto stats = defensor.getStats();
        if(stats->hp > danio_int)
            stats->hp -= danio_int;
        else
            stats->hp = 0;

        //contar golpes consecutivos para caída (solo ataques básicos)
        if(combateA->tipo_ataque == 1 || combateA->tipo_ataque == 2) {
            combateD->golpes_consecutivos++;
            if(combateD->golpes_consecutivos >= ICombate::MAX_GOLPES_CAIDA) {
                combateD->en_caida = true;
                combateD->causa_caida = 1; // golpe comun
                combateD->golpes_consecutivos = 0;
            }
        }

        //el remate y los ataques con objeto derriban inmediatamente
        if(combateA->tipo_ataque == 4 || combateA->tipo_ataque == 3) {
            combateD->en_caida = true;
            combateD->causa_caida = combateA->tipo_ataque; // 3=sillazo, 4=remate
            combateD->golpes_consecutivos = 0;
        }

        //cargar momentum al atacante
        auto momentum = atacante.getComponente<IMomentum>();
        if(momentum) {
            if(danio_base!=25){
                momentum->valor += danio_final * 2.5f;
                if(momentum->valor > IMomentum::MAX_MOMENTUM)
                    momentum->valor = IMomentum::MAX_MOMENTUM;
            }
        }

        //activar animación de golpeado
        combateD->fue_golpeado = true;

        //retroceso al recibir golpe (1 pixel en dirección opuesta al atacante)
        auto& posD = defensor.getTransformada()->posicion;
        auto& posA = atacante.getTransformada()->posicion;
        float retroceso = (posD.x >= posA.x) ? 2.f : -2.f;
        posD.x += retroceso;

        //evitar daño múltiple en el mismo ataque
        combateA->esta_atacando = false;
    }

    void SistemaDefensa(Entidad& ente, float dt)
    {
        auto combate = ente.getComponente<ICombate>();
        if(!combate) return;

        //manejar aturdimiento
        if(combate->esta_aturdido) {
            combate->tiempo_aturdido -= dt;
            if(combate->tiempo_aturdido <= 0) {
                combate->esta_aturdido = false;
            }
        }

        //manejar cooldown de defensa
        auto defensa = ente.getComponente<IDefensa>();
        if(defensa && defensa->cooldown_defensa > 0) {
            defensa->cooldown_defensa -= dt;
        }

        //manejar parpadeo de bloqueo (shader)
        if(defensa && defensa->flash_timer > 0) {
            defensa->flash_timer -= dt;
            float intensity = defensa->flash_timer / 0.15f; // 1.0 → 0.0
            if(intensity < 0.f) intensity = 0.f;
            auto shader = ente.getComponente<CE::IShader>();
            if(shader) {
                shader->m_shader.setUniform("flash_intensity", intensity);
            }
        }
    }

    void SistemaMomentum(Entidad& ente)
    {
        auto momentum = ente.getComponente<IMomentum>();
        if(!momentum) return;

        if(momentum->valor >= IMomentum::MAX_MOMENTUM) {
            momentum->valor = IMomentum::MAX_MOMENTUM;
            momentum->remate_disponible = true;
        } else {
            momentum->remate_disponible = false;
        }
    }

    bool SistemaConteoPC(Entidad& derribado, float dt, IConteo& conteo)
    {
        if(!conteo.en_conteo) return false;

        conteo.timer_cuenta += dt;
        if(conteo.timer_cuenta >= conteo.intervalo)
        {
            conteo.timer_cuenta = 0;
            //probabilidad de escape basada en HP
            auto stats = derribado.getStats();
            auto lstats = derribado.getComponente<ILuchadorStats>();
            float hp_ratio = (float)stats->hp / (float)stats->hp_max;
            float resist = lstats ? lstats->resistencia : 5.0f;
            float prob_escape = hp_ratio * resist * 10.f; // 0-100%

            int dado = rand() % 100;
            if(dado < (int)prob_escape) {
                //escapa
                conteo.en_conteo = false;
                conteo.cuenta = 0;
                auto combate = derribado.getComponente<ICombate>();
                if(combate) combate->esta_derribado = false;
                return false;
            }
            conteo.cuenta++;
            if(conteo.cuenta >= 3) return true; //victoria
        }
        return false;
    }

    bool SistemaConteoJugador(Entidad& jugador, float dt, IConteo& conteo)
    {
        if(!conteo.en_conteo) return false;

        conteo.timer_cuenta += dt;

        //el jugador escapa con mash (barra_escape llenada desde onInputs)
        auto stats = jugador.getStats();
        auto lstats = jugador.getComponente<ILuchadorStats>();
        float hp_ratio = (float)stats->hp / (float)stats->hp_max;
        float resist = lstats ? lstats->resistencia : 5.0f;
        //velocidad de llenado depende de HP y resistencia
        float vel_base = 15.f; //por presión de tecla
        float vel_mod = vel_base * hp_ratio * (resist / 5.f);

        //verificar si está presionando escape
        auto control = jugador.getComponente<CE::IControl>();
        if(control && control->escape) {
            conteo.barra_escape += vel_mod * dt * 60.f;
            control->escape = false; //consumir
        }

        if(conteo.barra_escape >= 100.f) {
            conteo.en_conteo = false;
            conteo.cuenta = 0;
            conteo.barra_escape = 0;
            auto combate = jugador.getComponente<ICombate>();
            if(combate) combate->esta_derribado = false;
            return false; //escapó
        }

        if(conteo.timer_cuenta >= conteo.intervalo) {
            conteo.timer_cuenta = 0;
            conteo.cuenta++;
            conteo.barra_escape = 0; //reset barra por cuenta
            if(conteo.cuenta >= 3) return true; //derrota
        }
        return false;
    }
    
    void SistemaMoverBalas(const std::shared_ptr<CE::Objeto> &ente, float dt)
    {
        auto trans = ente->getTransformada();
        auto vel = ente->getTransformada()->velocidad;
        trans->posicion.suma(vel.escala(dt));
    }
}

