#include "EscenaMatch.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <memory>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <cmath>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/TileMap.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Juego/Maquinas/lucha/IdleLucha.hpp>
#include <Juego/Maquinas/lucha/AtaqueSilla.hpp>
#include <Juego/Maquinas/lucha/CubriendoLucha.hpp>
#include <Juego/Maquinas/lucha/CubiertoLucha.hpp>
#include <Juego/Maquinas/lucha/LevantarseLucha.hpp>
#include <Juego/Maquinas/lucha/RebotePinLucha.hpp>
#include <Juego/Maquinas/Bosses/IdleBoss2.hpp>
#include <Juego/Maquinas/Bosses/CubriendoBoss.hpp>
#include <Juego/Maquinas/Bosses/CubiertoBoss.hpp>
#include <Juego/Maquinas/Bosses/LevantarseBoss.hpp>
#include <Juego/Maquinas/Bosses/RebotePinBoss.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>

namespace IVJ
{
    int EscenaMatch::personaje_jugador = 0;
    int EscenaMatch::personaje_rival = 1;

    EscenaMatch::EscenaMatch(std::shared_ptr<Entidad>& pref) :CE::Escena{},jugador_ref{pref}{}
    
    void EscenaMatch::onInit(){
        CE::GestorCamaras::Get().setCamaraActiva(1);

        if(!inicializar) {
            match_terminado = false;
            jugador_gano = false;
            en_pausa = false;
            opcion_pausa = 0;
            opcion_fin = 0;
            mostrando_controles = false;
            campana_sonada = false;
            campana_timer = 0.5f;

            conteo_boss.en_conteo = false;
            conteo_boss.cuenta = 0;
            conteo_jugador.en_conteo = false;
            conteo_jugador.cuenta = 0;
            conteo_jugador.barra_escape = 0.f;

            jugador_ref->setPosicion(234.f, 241.f);
            jugador_ref->getStats()->hp = jugador_ref->getStats()->hp_max;
            if(auto m = jugador_ref->getComponente<IMomentum>()) { m->valor = 0; m->remate_disponible = false; }
            if(auto c = jugador_ref->getComponente<ICombate>()) { c->esta_derribado = false; c->en_caida = false; c->tiene_silla = false; }
            if(jugador_ref->tieneComponente<IMaquinaEstado>()) { jugador_ref->setFSM(std::make_shared<IdleLucha>(2, 0.7f)); }
            if(auto control = jugador_ref->getComponente<CE::IControl>()) { 
                control->setActivo(true); 
                control->izq = control->der = control->arr = control->abj = control->punch = control->kick = control->guard = control->pickup = control->finisher = control->escape = control->run = false; 
            }

            if(boss_ref) {
                boss_ref->setPosicion(450.f, 241.f);
                boss_ref->getStats()->hp = boss_ref->getStats()->hp_max;
                if(auto m = boss_ref->getComponente<IMomentum>()) { m->valor = 0; m->remate_disponible = false; }
                if(auto c = boss_ref->getComponente<ICombate>()) { c->esta_derribado = false; c->en_caida = false; c->tiene_silla = false; }
                if(boss_ref->tieneComponente<IMaquinaEstado>()) { boss_ref->setFSM(std::make_shared<IdleBoss2>(2, 0.7f)); }
            }

            // re-asignar texturas por si se cambió de personaje
            std::string tex_jugador = (personaje_jugador == 0) ? "shawn" : "enemy";
            if(auto s = jugador_ref->getComponente<CE::ISprite>()) { 
                s->m_sprite.setTexture(CE::GestorAssets::Get().getTextura(tex_jugador), true);
                s->m_sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(s->width, s->height)));
            }
            std::string tex_boss = (personaje_rival == 0) ? "shawn" : "enemy";
            if(auto s = boss_ref->getComponente<CE::ISprite>()) { 
                s->m_sprite.setTexture(CE::GestorAssets::Get().getTextura(tex_boss), true);
                s->m_sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(s->width, s->height)));
            }

            if(silla_ref) {
                auto& pool = objetos.getPool();
                pool.erase(std::remove(pool.begin(), pool.end(), silla_ref), pool.end());
                silla_ref = nullptr;
            }
            item_timer = 0.f;
            return;
        }
        CE::GestorAssets::Get().agregarSonido("golpe", ASSETS "/sonidos/punch.ogg");
        CE::GestorAssets::Get().agregarSonido("sillazo", ASSETS "/sonidos/sillazo.ogg");
        CE::GestorAssets::Get().agregarSonido("campana", ASSETS "/sonidos/bell.ogg");
        CE::GestorAssets::Get().agregarSonido("bloqueo", ASSETS "/sonidos/bloqueado.ogg");
        CE::GestorAssets::Get().agregarSonido("conteo1", ASSETS "/sonidos/one.ogg");
        CE::GestorAssets::Get().agregarSonido("conteo2", ASSETS "/sonidos/two.ogg");
        CE::GestorAssets::Get().agregarSonido("conteo3", ASSETS "/sonidos/three.ogg");
        CE::GestorAssets::Get().agregarSonido("sweet", ASSETS "/sonidos/Sweet.ogg");
        CE::GestorAssets::Get().agregarSonido("awesomechant", ASSETS "/sonidos/AwesomeChant.ogg");
        CE::GestorAssets::Get().agregarSonido("finisher", ASSETS "/sonidos/finisher.ogg");


        //=== REGISTRAR BOTONES ===
        registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
        registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
        registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");
        
        //combate
        registrarBotones(sf::Keyboard::Scancode::Z,"punch");
        registrarBotones(sf::Keyboard::Scancode::X,"kick");
        registrarBotones(sf::Keyboard::Scancode::C,"guard");
        registrarBotones(sf::Keyboard::Scancode::V,"pickup");
        registrarBotones(sf::Keyboard::Scancode::F,"finisher");
        registrarBotones(sf::Keyboard::Scancode::Space,"escape");

        //cargar mapa 2 layers
        tiles_layers.push_back(TileMap()); // ring
        tiles_layers.push_back(TileMap()); // cuerdas

        // CAMBIO: Ya no le pasamos el pool 'objetos' al TileMap. 
        // Solo sirve de dibujo de fondo.
        if(!tiles_layers[0].loadTileMap(ASSETS "/mapas/ringCentral.txt"))
            exit(EXIT_FAILURE);
        if(!tiles_layers[1].loadTileMap(ASSETS "/mapas/cuerdas.txt"))
           exit(EXIT_FAILURE);

        // === LÍMITES INVISIBLES DEL RING ===
        // Función auxiliar para crear paredes invisibles
        auto crearPared = [&](float ancho, float alto, float x, float y) {
            auto pared = std::make_shared<Entidad>();
            // Solo ocupamos el BoundingBox, no necesitamos Sprite
            pared->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{ancho, alto}));
            pared->setPosicion(x, y);
            // Paredes deben estar "vivas" para que no las borre el pool
            pared->getStats()->hp = 100;
            pared->getStats()->hp_max = 100;
            objetos.agregarPool(pared);
        };

        // NOTA: Ajusta estas coordenadas (x,y) y dimensiones según donde esté dibujada tu lona
        crearPared(450.f, 10.f, 334.f, 126.f); // Límite Superior (Arriba)
        crearPared(500.f, 10.f, 334.f, 296.f); // Límite Inferior (Abajo)
        crearPared(20.f, 140.f, 90.f, 206.f); // Límite Izquierdo
        crearPared(20.f, 140.f, 590.f, 206.f); // Límite Derecho

        // === CARGAR OBJETOS INDIVIDUALES DESDE EL ATLAS ===
        CE::GestorAssets::Get().agregarTextura("atlas_objetos",
                ASSETS "/atlas/RingWWF.png",
                CE::Vector2D{0.f,0.f},
                CE::Vector2D{672,769});

        //=== TEXTURAS DE LUCHADORES ===
        CE::GestorAssets::Get().agregarTextura(
                "shawn",
                ASSETS "/sprites/lucha/ShawnM.png",
                CE::Vector2D{0.f,0.f},
                CE::Vector2D{560.f,1144.f}
            );
        
        CE::GestorAssets::Get().agregarTextura(
                "enemy",
                ASSETS "/sprites/lucha/MrP.png",
                CE::Vector2D{0.f,0.f},
                CE::Vector2D{560.f,1280.f}
            );

        CE::GestorAssets::Get().agregarTextura(
            "silla",
            ASSETS "/sprites/lucha/silla.png",
            CE::Vector2D{0.f,0.f},
            CE::Vector2D{27.f,52.f}
        );

        CE::GestorAssets::Get().agregarTextura(
            "shawnSillazo",
            ASSETS "/sprites/lucha/shawnSillazo.png",
            CE::Vector2D{0.f,0.f},
            CE::Vector2D{240.f,128.f}
        );

        CE::GestorAssets::Get().agregarTextura(
            "enemySillazo",
            ASSETS "/sprites/lucha/enemySillazo.png",
            CE::Vector2D{0.f,0.f},
            CE::Vector2D{240.f,128.f}
        );


        //=== JUGADOR ===
        auto trans = jugador_ref->getTransformada();
        trans->velocidad = CE::Vector2D{80.f,80.f};
        jugador_ref->setPosicion(234.f,241.f);
        
        std::string tex_jugador = (personaje_jugador == 0) ? "shawn" : "enemy";
        auto sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura(tex_jugador),
                80,88,
                1.f);
        
        float orientacion_jugador = (personaje_jugador == 1) ? -1.f : 1.f;
        sprite->m_sprite.setScale({orientacion_jugador, 1.f});
        
        jugador_ref->addComponente(sprite);
        jugador_ref->addComponente(std::make_shared<CE::IControl>());
        jugador_ref->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{35.f,88.f}));
        //componentes de combate
        jugador_ref->addComponente(std::make_shared<ILuchadorStats>(9, 6, 7));
        jugador_ref->addComponente(std::make_shared<ICombate>());
        jugador_ref->addComponente(std::make_shared<IDefensa>());
        jugador_ref->addComponente(std::make_shared<IMomentum>());
        //stats de vida
        jugador_ref->getStats()->hp_max = 100;
        jugador_ref->getStats()->hp = 100;

        //FSM
        auto me = std::make_shared<IVJ::IMaquinaEstado>();
        me->fsm = std::make_shared<IVJ::IdleLucha>(2,0.7f); 
        jugador_ref->addComponente(me);
        jugador_ref->setFSM(me->fsm);

        //shader de parpadeo al bloquear
        auto shader_jug = std::make_shared<CE::IShader>("", ASSETS "/shaders/block_flash.frag");
        flash_intensity_jug = 0.f;
        shader_jug->setEscalar("flash_intensity", &flash_intensity_jug);
        jugador_ref->addComponente(shader_jug);

        //target para saber dónde está el oponente (necesario para flip al correr)
        auto jugador_target = std::make_shared<ITarget>(nullptr);
        jugador_ref->addComponente(jugador_target);

        //=== BOSS (ANTAGONISTA) ===
        boss_ref = std::make_shared<Entidad>();
        std::string tex_boss = (personaje_rival == 0) ? "shawn" : "enemy";
        auto boss_sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura(tex_boss),
                80.f,88.f,
                1.f);
        
        float orientacion_boss = (personaje_rival == 0) ? -1.f : 1.f;
        boss_sprite->m_sprite.setScale({orientacion_boss, 1.f});
        auto boss_me = std::make_shared<IVJ::IMaquinaEstado>();
        boss_me->fsm = std::make_shared<IVJ::IdleBoss2>(2,0.7f);
        auto boss_target = std::make_shared<ITarget>(nullptr);
        boss_target->setTarget(*jugador_ref);

        boss_ref->addComponente(boss_sprite)
            .addComponente(boss_me)
            .addComponente(std::make_shared<IRangoAggro>(500.f))
            .addComponente(boss_target)
            .addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{35.f,88.f}))
            //componentes de combate
            .addComponente(std::make_shared<ILuchadorStats>(8, 6, 6))
            .addComponente(std::make_shared<ICombate>())
            .addComponente(std::make_shared<IDefensa>())
            .addComponente(std::make_shared<IMomentum>());

        boss_ref->setPosicion(450.f,241.f);
        boss_ref->setFSM(boss_me->fsm);
        boss_ref->getStats()->hp_max = 100;
        boss_ref->getStats()->hp = 100;

        //shader de parpadeo al bloquear
        auto shader_boss = std::make_shared<CE::IShader>("", ASSETS "/shaders/block_flash.frag");
        flash_intensity_boss = 0.f;
        shader_boss->setEscalar("flash_intensity", &flash_intensity_boss);
        boss_ref->addComponente(shader_boss);

        objetos.agregarPool(boss_ref);

        //asignar el boss como target del jugador (ahora que boss ya fue creado)
        jugador_target->setTarget(*boss_ref);
        
        //cargar shaders
        momentum_shader = std::make_unique<CE::IShader>("", ASSETS "/shaders/momentum.frag");

        // === PRUEBA: SPAWN INICIAL DE SILLA ===
        auto silla_test = std::make_shared<Entidad>();
        silla_test->setPosicion(132.f, 206.f);
        auto sprite_silla = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("silla"), 
            27.f, 52.f, 1.f
        );
        silla_test->addComponente(sprite_silla);
        silla_test->addComponente(std::make_shared<IItem>(IItem::SILLA));
        silla_test->getStats()->hp = 100;
        silla_test->getStats()->hp_max = 100;
        objetos.agregarPool(silla_test);
        silla_ref = silla_test;
        item_timer = 0;
        
        // === MENÚ DE PAUSA ===
        registrarBotones(sf::Keyboard::Scancode::P,"pausa");
        
        fondo_pausa = std::make_shared<Rectangulo>(1080.f, 720.f, sf::Color(0, 0, 0, 180), sf::Color::Transparent);
        fondo_pausa->setPosicion(540.f, 360.f);

        txt_pausa_titulo = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "PAUSA");
        txt_pausa_titulo->setFontSize(60u);
        txt_pausa_titulo->setColor(sf::Color::White);
        txt_pausa_titulo->setOriginCenter();
        txt_pausa_titulo->setPosicion(540.f, 200.f);

        txt_pausa_opc1 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Reanudar");
        txt_pausa_opc1->setFontSize(40u);
        txt_pausa_opc1->setOriginCenter();
        txt_pausa_opc1->setPosicion(540.f, 350.f);

        txt_pausa_opc2 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Volver al Menu");
        txt_pausa_opc2->setFontSize(40u);
        txt_pausa_opc2->setOriginCenter();
        txt_pausa_opc2->setPosicion(540.f, 420.f);

        txt_pausa_opc3 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Ver Controles");
        txt_pausa_opc3->setFontSize(40u);
        txt_pausa_opc3->setOriginCenter();
        txt_pausa_opc3->setPosicion(540.f, 490.f);

        txt_pausa_controles = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), 
            "CONTROLES\n\nZ: Golpe Ligero\nX: Patada Ligera\nC: Bloquear\nV: Recoger Objeto\nF: Remate (Con Momentum lleno)\nFlechas: Moverse\nDoble Tap Flechas: Correr\nP: Pausa\n\nPresiona Enter para volver");
        txt_pausa_controles->setFontSize(30u);
        txt_pausa_controles->setColor(sf::Color::White);
        txt_pausa_controles->setOriginCenter();
        txt_pausa_controles->setPosicion(540.f, 360.f);

        // Menú Fin de Combate
        txt_fin_titulo = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "VICTORIA!");
        txt_fin_titulo->setFontSize(80u);
        txt_fin_titulo->setColor(sf::Color::Yellow);
        txt_fin_titulo->setOriginCenter();
        txt_fin_titulo->setPosicion(540.f, 200.f);

        txt_fin_opc1 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Avanzar (Menu)");
        txt_fin_opc1->setFontSize(40u);
        txt_fin_opc1->setOriginCenter();
        txt_fin_opc1->setPosicion(540.f, 400.f);

        txt_fin_opc2 = std::make_shared<Texto>(CE::GestorAssets::Get().getFont("default_font"), "Reiniciar Combate");
        txt_fin_opc2->setFontSize(40u);
        txt_fin_opc2->setOriginCenter();
        txt_fin_opc2->setPosicion(540.f, 480.f);

        inicializar=false;
    }

    void EscenaMatch::onFinal(){
         //reseteamos la camara a la estática al salir/cambiar de escena
        CE::GestorCamaras::Get().setCamaraActiva(0);
    }

    void EscenaMatch::onUpdate(float dt){
        // actualizar tiempo del shader
        tiempo_shader += dt;
        if(momentum_shader)
            momentum_shader->setEscalar("time", &tiempo_shader);

        //campana inicio con retraso
        if(!campana_sonada) {
            campana_timer -= dt;
            if(campana_timer <= 0.f) {
                CE::GestorAssets::Get().getSonido("campana").play();
                campana_sonada = true;
            }
        }

        if(match_terminado) return;
        
        // Pausar toda la lógica si estamos en pausa
        if(en_pausa) return;

        jugador_ref->inputFSM();
        jugador_ref->onUpdate(dt);
        SistemaMover2(jugador_ref,dt);

        // Voltear al jugador hacia el boss
        if(boss_ref && jugador_ref->getComponente<CE::ISprite>()) {
            auto control = jugador_ref->getComponente<CE::IControl>();
            bool moviendo = control && (control->izq || control->der);
            float orientacion = (personaje_jugador == 1) ? -1.f : 1.f;
            if (moviendo) {
                if (control->der) {
                    jugador_ref->getComponente<CE::ISprite>()->m_sprite.setScale({orientacion, 1.f});
                } else if (control->izq) {
                    jugador_ref->getComponente<CE::ISprite>()->m_sprite.setScale({-orientacion, 1.f});
                }
            } else {
                float dx = boss_ref->getTransformada()->posicion.x - jugador_ref->getTransformada()->posicion.x;
                if(dx > 0)
                    jugador_ref->getComponente<CE::ISprite>()->m_sprite.setScale({orientacion, 1.f});
                else if(dx < 0)
                    jugador_ref->getComponente<CE::ISprite>()->m_sprite.setScale({-orientacion, 1.f});
            }
        }

        //doble-tap timer update
        if(tap_timer_der > 0) tap_timer_der -= dt;
        else tap_count_der = 0;
        if(tap_timer_izq > 0) tap_timer_izq -= dt;
        else tap_count_izq = 0;

        bool en_conteo_activo = (conteo_boss.en_conteo || conteo_jugador.en_conteo);
        for(auto& obj: objetos.getPool())
        {
            obj->inputFSM();
            obj->onUpdate(dt);
            if(!en_conteo_activo) {
                // El jugador colisiona con el boss pero sin resolución física (para que no se atasquen).
                // Con las paredes (que no tienen ICombate), sí hay resolución física.
                bool es_combate = obj->tieneComponente<ICombate>();
                SistemaColAABBMid(*jugador_ref, *obj, !es_combate);

                // El Boss debe colisionar con las paredes (para no salirse del ring)
                if (boss_ref && obj != boss_ref && !es_combate) {
                    SistemaColAABBMid(*boss_ref, *obj, true);
                }
            }
        }

        //=== SISTEMAS DE COMBATE ===
        if(boss_ref) {
            //daño jugador → boss
            auto entJ = dynamic_cast<Entidad*>(jugador_ref.get());
            auto entB = dynamic_cast<Entidad*>(boss_ref.get());
            if(entJ && entB) {
                SistemaDaño(*entJ, *entB);
                SistemaDaño(*entB, *entJ);
                SistemaDefensa(*entJ, dt);
                SistemaDefensa(*entB, dt);
                SistemaMomentum(*entJ);
                SistemaMomentum(*entB);
            }

            //=== CONTEO (PINFALL) ===
            //solo se activa pinfall cuando HP=0 y se aplica un remate (finisher)
            //no durante caidas normales de combate
            auto combateBoss = boss_ref->getComponente<ICombate>();
            auto combateJug = jugador_ref->getComponente<ICombate>();

            // === INICIO AUTOMÁTICO DE CONTEO ===
            auto meJ = jugador_ref->getComponente<IMaquinaEstado>();
            auto meB = boss_ref->getComponente<IMaquinaEstado>();
            if(meJ && meJ->fsm && meB && meB->fsm) {
                if(meJ->fsm->getNombre() == "CubriendoLucha" && !conteo_boss.en_conteo && meB->fsm->getNombre() == "CubiertoBoss") {
                    conteo_boss.en_conteo = true;
                    conteo_boss.cuenta = 0;
                    conteo_boss.timer_cuenta = 0.f;
                }
                if(meB->fsm->getNombre() == "CubriendoBoss" && !conteo_jugador.en_conteo && meJ->fsm->getNombre() == "CubiertoLucha") {
                    conteo_jugador.en_conteo = true;
                    conteo_jugador.cuenta = 0;
                    conteo_jugador.timer_cuenta = 0.f;
                    conteo_jugador.barra_escape = 0.f;
                }
            }

            //desactivar conteo si el derribado se levantó
            if(conteo_boss.en_conteo && combateBoss && !combateBoss->esta_derribado) {
                conteo_boss.en_conteo = false;
                conteo_boss.cuenta = 0;
            }
            if(conteo_jugador.en_conteo && combateJug && !combateJug->esta_derribado) {
                conteo_jugador.en_conteo = false;
                conteo_jugador.cuenta = 0;
            }

            // Gestionar salida de los estados de Pinfall cuando ya no hay conteo
            if(!conteo_boss.en_conteo) {
                if(meJ && meJ->fsm && meJ->fsm->getNombre() == "CubriendoLucha") {
                    float s_x = jugador_ref->getTransformada()->posicion.x;
                    float s_y = jugador_ref->getTransformada()->posicion.y;
                    float dir_x = 1.f;
                    if(jugador_ref->getComponente<CE::ISprite>()) {
                        dir_x = (jugador_ref->getComponente<CE::ISprite>()->m_sprite.getScale().x > 0) ? -1.f : 1.f;
                    }
                    jugador_ref->setFSM(std::make_shared<RebotePinLucha>(s_x, s_y, dir_x));
                }
                if(meB && meB->fsm && meB->fsm->getNombre() == "CubiertoBoss") {
                    boss_ref->setFSM(std::make_shared<LevantarseBoss>(7, 0.15f));
                }
            }
            if(!conteo_jugador.en_conteo) {
                if(meB && meB->fsm && meB->fsm->getNombre() == "CubriendoBoss") {
                    float s_x = boss_ref->getTransformada()->posicion.x;
                    float s_y = boss_ref->getTransformada()->posicion.y;
                    float dir_x = 1.f;
                    if(boss_ref->getComponente<CE::ISprite>()) {
                        dir_x = (boss_ref->getComponente<CE::ISprite>()->m_sprite.getScale().x > 0) ? 1.f : -1.f;
                    }
                    boss_ref->setFSM(std::make_shared<RebotePinBoss>(s_x, s_y, dir_x));
                }
                if(meJ && meJ->fsm && meJ->fsm->getNombre() == "CubiertoLucha") {
                    jugador_ref->setFSM(std::make_shared<LevantarseLucha>(7, 0.15f));
                }
            }

            if(conteo_boss.en_conteo) {
                // Alinear posición
                jugador_ref->setPosicion(boss_ref->getTransformada()->posicion.x, boss_ref->getTransformada()->posicion.y);
                // Alinear escala/orientación
                if(jugador_ref->getComponente<CE::ISprite>() && boss_ref->getComponente<CE::ISprite>()) {
                    jugador_ref->getComponente<CE::ISprite>()->m_sprite.setScale(
                        boss_ref->getComponente<CE::ISprite>()->m_sprite.getScale()
                    );
                }
                if(SistemaConteoPC(*entB, dt, conteo_boss)) {
                    match_terminado = true;
                    jugador_gano = true;
                }
            }

            if(conteo_jugador.en_conteo) {
                // Alinear posición
                boss_ref->setPosicion(jugador_ref->getTransformada()->posicion.x, jugador_ref->getTransformada()->posicion.y);
                // Alinear escala/orientación
                if(boss_ref->getComponente<CE::ISprite>() && jugador_ref->getComponente<CE::ISprite>()) {
                    boss_ref->getComponente<CE::ISprite>()->m_sprite.setScale(
                        jugador_ref->getComponente<CE::ISprite>()->m_sprite.getScale()
                    );
                }
                if(SistemaConteoJugador(*entJ, dt, conteo_jugador)) {
                    match_terminado = true;
                    jugador_gano = false;
                }
            }
        }

        //=== SPAWN DE SILLA (solo si no hay una en escena) ===
        bool silla_existe = (silla_ref && silla_ref->estaVivo());
        if(!silla_existe && !boss_ref->getComponente<ICombate>()->esta_derribado) {
            item_timer += dt;
            if(item_timer >= ITEM_INTERVAL) {
                auto silla = std::make_shared<Entidad>();
                float rnd_x = 120.f + static_cast<float>(rand() % 280);
                float rnd_y = 150.f + static_cast<float>(rand() % 110);
                silla->setPosicion(rnd_x, rnd_y);

                auto sprite_silla = std::make_shared<CE::ISprite>(
                    CE::GestorAssets::Get().getTextura("silla"), 
                    27.f, 52.f, 1.f
                );
                silla->addComponente(sprite_silla);
                silla->addComponente(std::make_shared<IItem>(IItem::SILLA));
                // Iniciar con vida para que estaVivo() sea true
                silla->getStats()->hp = 100;
                silla->getStats()->hp_max = 100;
                
                // Sin BoundingBox: la silla no bloquea el tránsito
                objetos.agregarPool(silla);
                silla_ref = silla;
                item_timer = 0;
            }
        }

        //=== PROXIMIDAD SILLA + PICKUP ===
        mostrar_usar_silla = false;
        if(silla_ref && silla_ref->estaVivo()) {
            auto item = silla_ref->getComponente<IItem>();
            auto combateJ = jugador_ref->getComponente<ICombate>();
            auto combateB = boss_ref ? boss_ref->getComponente<ICombate>() : nullptr;
            
            if(item && item->recogido) {
                if(combateJ && combateJ->tiene_silla) {
                    auto pos = jugador_ref->getTransformada()->posicion;
                    silla_ref->setPosicion(pos.x, pos.y + 17.f);
                } else if(combateB && combateB->tiene_silla) {
                    auto pos = boss_ref->getTransformada()->posicion;
                    silla_ref->setPosicion(pos.x, pos.y + 17.f);
                } else {
                    // Ninguno tiene la silla ya, la destruimos
                    auto& pool = objetos.getPool();
                    pool.erase(std::remove(pool.begin(), pool.end(), silla_ref), pool.end());
                    silla_ref = nullptr;
                }
            } else {
                auto& posS = silla_ref->getTransformada()->posicion;
                // Pickup Jugador
                auto& posJ = jugador_ref->getTransformada()->posicion;
                float distJ = std::sqrt((posJ.x-posS.x)*(posJ.x-posS.x) + (posJ.y-posS.y)*(posJ.y-posS.y));
                if(distJ < 40.f) {
                    mostrar_usar_silla = true;
                    auto control = jugador_ref->getComponente<CE::IControl>();
                    if(control && control->pickup) {
                        control->pickup = false;
                        item->recogido = true;
                        if(combateJ) combateJ->tiene_silla = true;
                    }
                }
                
                // Pickup Boss (automático si está cerca)
                if(!item->recogido && boss_ref && boss_ref->estaVivo()) {
                    auto& posB = boss_ref->getTransformada()->posicion;
                    float distB = std::sqrt((posB.x-posS.x)*(posB.x-posS.x) + (posB.y-posS.y)*(posB.y-posS.y));
                    if(distB < 40.f) {
                        item->recogido = true;
                        if(combateB) combateB->tiene_silla = true;
                    }
                }
            }
        }

        //=== PROXIMIDAD BOSS + CUBRIR ===
        mostrar_cubrir = false;
        if (boss_ref) {
            auto combateBoss = boss_ref->getComponente<ICombate>();
            if (combateBoss && combateBoss->esta_derribado && !conteo_boss.en_conteo && !conteo_jugador.en_conteo) {
                auto& posJ = jugador_ref->getTransformada()->posicion;
                auto& posB = boss_ref->getTransformada()->posicion;
                float dist = std::sqrt((posJ.x-posB.x)*(posJ.x-posB.x) + (posJ.y-posB.y)*(posJ.y-posB.y));
                if (dist < 50.f) {
                    mostrar_cubrir = true;
                    auto control = jugador_ref->getComponente<CE::IControl>();
                    if (control && control->pickup) {
                        control->pickup = false;
                        
                        // Iniciar cover
                        conteo_boss.en_conteo = true;
                        conteo_boss.cuenta = 0;
                        conteo_boss.timer_cuenta = 0.f;

                        // Cambiar a estados de cubrir
                        if(jugador_ref->tieneComponente<IMaquinaEstado>()) {
                            jugador_ref->setFSM(std::make_shared<CubriendoLucha>());
                        }
                        if(boss_ref->tieneComponente<IMaquinaEstado>()) {
                            boss_ref->setFSM(std::make_shared<CubiertoBoss>());
                        }
                    }
                }
            }
        }
    }

    void EscenaMatch::onInputs(const CE::Botones& accion){
        if(accion.getTipo() == CE::Botones::TipoAccion::OnPress)
        {
            if (match_terminado) {
                if(accion.getNombre() == "arriba") {
                    if(opcion_fin == 0) opcion_fin = 1;
                    else opcion_fin--;
                }
                else if(accion.getNombre() == "abajo") {
                    opcion_fin = (opcion_fin + 1) % 2;
                }
                else if(accion.getNombre() == "aceptar") {
                    if(opcion_fin == 0) {
                        CE::GestorEscenas::Get().cambiarEscena("Menu");
                    }
                    else if(opcion_fin == 1) {
                        CE::GestorEscenas::Get().cambiarEscena("Match");
                    }
                }
                return;
            }

            if(accion.getNombre() == "pausa")
            {
                en_pausa = !en_pausa;
                mostrando_controles = false;
                opcion_pausa = 0;
            }

            if(en_pausa)
            {
                if(!mostrando_controles)
                {
                    if(accion.getNombre() == "arriba") {
                        if(opcion_pausa == 0) opcion_pausa = 2;
                        else opcion_pausa--;
                    }
                    else if(accion.getNombre() == "abajo") {
                        opcion_pausa = (opcion_pausa + 1) % 3;
                    }
                    else if(accion.getNombre() == "aceptar") {
                        if(opcion_pausa == 0) {
                            en_pausa = false; // Reanudar
                        }
                        else if(opcion_pausa == 1) {
                            CE::GestorEscenas::Get().cambiarEscena("Menu"); // Volver al Menú
                        }
                        else if(opcion_pausa == 2) {
                            mostrando_controles = true; // Ver Controles
                        }
                    }
                }
                else
                {
                    // Si estamos viendo los controles, cualquier tecla de aceptar o pausa vuelve
                    if(accion.getNombre() == "aceptar" || accion.getNombre() == "escape" || accion.getNombre() == "pausa") {
                        mostrando_controles = false;
                    }
                }
                return; // Si estamos en pausa, ignorar el resto de inputs del juego
            }
        }

        auto control = jugador_ref->getComponente<CE::IControl>();
        if(!control) return;

        switch(accion.getTipo()){
            case CE::Botones::TipoAccion::OnPress:{   
                if(accion.getNombre() == "izquierda") {
                    control->izq = true;
                    //doble-tap
                    tap_count_izq++;
                    tap_timer_izq = TAP_WINDOW;
                    if(tap_count_izq >= 2) {
                        control->run = true;
                        jugador_ref->getTransformada()->velocidad.x = 160.f;
                    }
                }
                if(accion.getNombre() == "derecha") {
                    control->der = true;
                    tap_count_der++;
                    tap_timer_der = TAP_WINDOW;
                    if(tap_count_der >= 2) {
                        control->run = true;
                        jugador_ref->getTransformada()->velocidad.x = 160.f;
                    }
                }
                if(accion.getNombre() == "arriba")
                    control->arr = true;
                if(accion.getNombre() == "abajo")
                    control->abj = true;
                //combate
                if(accion.getNombre() == "punch")    control->punch = true;
                if(accion.getNombre() == "kick")     control->kick = true;
                if(accion.getNombre() == "guard")     control->guard = true;
                if(accion.getNombre() == "pickup")    control->pickup = true;
                if(accion.getNombre() == "finisher")  control->finisher = true;
                if(accion.getNombre() == "escape")    control->escape = true;
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:{
                if(accion.getNombre() == "izquierda") {
                    control->izq = false;
                    control->run = false;
                    jugador_ref->getTransformada()->velocidad.x = 80.f;
                }
                if(accion.getNombre() == "derecha") {
                    control->der = false;
                    control->run = false;
                    jugador_ref->getTransformada()->velocidad.x = 80.f;
                }
                if(accion.getNombre() == "arriba")
                    control->arr = false;
                if(accion.getNombre() == "abajo")
                    control->abj = false;
                if(accion.getNombre() == "punch")    control->punch = false;
                if(accion.getNombre() == "kick")     control->kick = false;
                if(accion.getNombre() == "guard")     control->guard = false;
                if(accion.getNombre() == "pickup")    control->pickup = false;
                if(accion.getNombre() == "finisher")  control->finisher = false;
                if(accion.getNombre() == "escape")    control->escape = false;
                break;
            }
            case CE::Botones::TipoAccion::None:{
                break;
            }
        }
    }

   void EscenaMatch::onRender(){
        // 1. DIBUJAR EL FONDO
        CE::Render::Get().AddToDraw(tiles_layers[0]); 

        // 2. DIBUJAR OBJETOS Y ENEMIGOS
        for(auto& obj: objetos.getPool())
        {
            CE::Render::Get().AddToDraw(*obj);
        }

        // 3. DIBUJAR AL JUGADOR
        CE::Render::Get().AddToDraw(*jugador_ref);

        // 4. DIBUJAR LAS CUERDAS FRONTALES
        CE::Render::Get().AddToDraw(tiles_layers[1]);

        // 5. HUD (sobre todo)
        auto& textura = CE::Render::Get().GetTextura();
        sf::View ui_view = textura.getDefaultView();
        textura.setView(ui_view);

        float vw = (float)textura.getSize().x;
        float vh = (float)textura.getSize().y;

        //barras de HP
        dibujarBarraHP(*jugador_ref, 20.f, 20.f, sf::Color{50,200,50}, sf::Color{60,60,60}, false);
        if(boss_ref)
            dibujarBarraHP(*boss_ref, vw - 320.f, 20.f, sf::Color{200,50,50}, sf::Color{60,60,60}, true);

        //medidor de Momentum
        dibujarMomentum(*jugador_ref, 20.f, 40.f, false);
        if(boss_ref)
            dibujarMomentum(*boss_ref, vw - 320.f, 40.f, true);

        //prompt de silla
        if(mostrar_usar_silla) {
            sf::Font& font = CE::GestorAssets::Get().getFont("nova");
            sf::Text txt(font, "Usar silla (V)", 14);
            txt.setPosition({vw/2.f - 50.f, vh - 30.f});
            txt.setFillColor(sf::Color::Yellow);
            txt.setOutlineColor(sf::Color::Black);
            txt.setOutlineThickness(1.f);
            CE::Render::Get().AddToDraw(txt);
        }

        //prompt de cubrir
        if(mostrar_cubrir) {
            sf::Font& font = CE::GestorAssets::Get().getFont("nova");
            sf::Text txt(font, "Cubrir (V)", 14);
            txt.setPosition({vw/2.f - 40.f, vh - 50.f});
            txt.setFillColor(sf::Color::Yellow);
            txt.setOutlineColor(sf::Color::Black);
            txt.setOutlineThickness(1.f);
            CE::Render::Get().AddToDraw(txt);
        }

        //conteo pinfall
        if(conteo_boss.en_conteo && conteo_boss.cuenta !=0)
            dibujarConteo(conteo_boss.cuenta, vw/2.f - 30.f, vh/2.f - 80.f);
        if(conteo_jugador.en_conteo && conteo_jugador.cuenta !=0) {
            dibujarConteo(conteo_jugador.cuenta, vw/2.f - 30.f, vh/2.f - 80.f);
            dibujarBarraEscape(conteo_jugador.barra_escape, vw/2.f - 100.f, vh/2.f + 20.f);
        }

        //mensaje de fin de match
        if(match_terminado) {
            fondo_pausa->setPosicion(vw/2.f, vh/2.f);
            fondo_pausa->onUpdate(0);
            CE::Render::Get().AddToDraw(*fondo_pausa); // Reutilizamos el fondo oscuro de la pausa
            
            txt_fin_titulo->setString(jugador_gano ? "VICTORIA!" : "DERROTA...");
            txt_fin_titulo->setColor(jugador_gano ? sf::Color::Yellow : sf::Color::Red);
            txt_fin_titulo->setOriginCenter();
            txt_fin_titulo->setPosicion(vw/2.f, vh/2.f - 100.f);
            
            sf::Color color_sel = sf::Color::Yellow;
            sf::Color color_unsel = sf::Color(170, 175, 190);
            
            txt_fin_opc1->setColor(opcion_fin == 0 ? color_sel : color_unsel);
            txt_fin_opc1->setPosicion(vw/2.f, vh/2.f + 50.f);

            txt_fin_opc2->setColor(opcion_fin == 1 ? color_sel : color_unsel);
            txt_fin_opc2->setPosicion(vw/2.f, vh/2.f + 120.f);

            CE::Render::Get().AddToDraw(*txt_fin_titulo);
            CE::Render::Get().AddToDraw(*txt_fin_opc1);
            CE::Render::Get().AddToDraw(*txt_fin_opc2);
        }

        // --- MENÚ DE PAUSA ---
        if(en_pausa) {
            fondo_pausa->setPosicion(vw/2.f, vh/2.f);
            fondo_pausa->onUpdate(0);
            CE::Render::Get().AddToDraw(*fondo_pausa);
            
            if(!mostrando_controles) {
                txt_pausa_titulo->setPosicion(vw/2.f, vh/2.f - 160.f);
                CE::Render::Get().AddToDraw(*txt_pausa_titulo);
                
                sf::Color color_sel = sf::Color::Yellow;
                sf::Color color_unsel = sf::Color(170, 175, 190);
                
                txt_pausa_opc1->setColor(opcion_pausa == 0 ? color_sel : color_unsel);
                txt_pausa_opc1->setPosicion(vw/2.f, vh/2.f - 40.f);
                
                txt_pausa_opc2->setColor(opcion_pausa == 1 ? color_sel : color_unsel);
                txt_pausa_opc2->setPosicion(vw/2.f, vh/2.f + 30.f);
                
                txt_pausa_opc3->setColor(opcion_pausa == 2 ? color_sel : color_unsel);
                txt_pausa_opc3->setPosicion(vw/2.f, vh/2.f + 100.f);

                CE::Render::Get().AddToDraw(*txt_pausa_opc1);
                CE::Render::Get().AddToDraw(*txt_pausa_opc2);
                CE::Render::Get().AddToDraw(*txt_pausa_opc3);
            }
            else {
                CE::Render::Get().AddToDraw(*txt_pausa_controles);
            }
        }

        //restaurar vista del juego
        textura.setView(CE::GestorCamaras::Get().getCamaraActiva().getView());

        #if DEBUG
                auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
                auto csv = (CE::CamaraSnapVentana*)cam;
                auto csvpos = csv->getTransformada().posicion;
                sf::RectangleShape debugcam{{csv->m_vdim.x,csv->m_vdim.y}};
                debugcam.setOrigin({csv->m_vdim.x/2.f,csv->m_vdim.y/2.f});
                debugcam.setPosition({csvpos.x,csvpos.y});
                debugcam.setOutlineThickness(3.f);
                debugcam.setOutlineColor(sf::Color::Yellow);
                debugcam.setFillColor(sf::Color::Transparent);
                CE::Render::Get().AddToDraw(debugcam);
        #endif
    }

    void EscenaMatch::dibujarBarraHP(Entidad& ente, float x, float y,
                                      const sf::Color& color, const sf::Color& fondo, bool inverso)
    {
        float ancho = 300.f;
        float alto = 20.f;

        //fondo
        sf::RectangleShape barra_fondo{{ancho, alto}};
        barra_fondo.setPosition({x, y});
        barra_fondo.setFillColor(fondo);
        barra_fondo.setOutlineColor(sf::Color::White);
        barra_fondo.setOutlineThickness(1.f);
        CE::Render::Get().AddToDraw(barra_fondo);

        //relleno
        float hp = (float)ente.getStats()->hp;
        float hp_max = (float)ente.getStats()->hp_max;
        float ratio = (hp_max > 0) ? (hp / hp_max) : 0.f;
        sf::RectangleShape barra_hp{{ancho * ratio, alto}};
        if(inverso)
            barra_hp.setPosition({x + ancho - (ancho * ratio), y});
        else
            barra_hp.setPosition({x, y});
        barra_hp.setFillColor(color);
        CE::Render::Get().AddToDraw(barra_hp);
    }

    void EscenaMatch::dibujarMomentum(Entidad& ente, float x, float y, bool inverso)
    {
        auto momentum = ente.getComponente<IMomentum>();
        if(!momentum) return;

        float ancho = 300.f;
        float alto = 12.f;

        //fondo
        sf::RectangleShape barra_fondo{{ancho, alto}};
        barra_fondo.setPosition({x, y});
        barra_fondo.setFillColor(sf::Color{40,40,40});
        barra_fondo.setOutlineColor(sf::Color{200,200,0});
        barra_fondo.setOutlineThickness(1.f);
        CE::Render::Get().AddToDraw(barra_fondo);

        //relleno
        float ratio = momentum->valor / IMomentum::MAX_MOMENTUM;
        sf::Color col = momentum->remate_disponible ? sf::Color{255,215,0} : sf::Color{200,150,0};
        sf::RectangleShape barra{{ancho * ratio, alto}};
        if(inverso)
            barra.setPosition({x + ancho - (ancho * ratio), y});
        else
            barra.setPosition({x, y});
        barra.setFillColor(col);
        
        if(momentum->remate_disponible && momentum_shader) {
            sf::RenderStates states;
            states.shader = &momentum_shader->m_shader;
            CE::Render::Get().AddToDraw(barra, states);
        }
        else {
            CE::Render::Get().AddToDraw(barra);
        }

        //label
        sf::Font& font = CE::GestorAssets::Get().getFont("nova");
        std::string label = momentum->remate_disponible ? "REMATE LISTO! (F)" : "Momentum";
        sf::Text txt(font, label, 10);
        if(inverso)
            txt.setPosition({x + ancho - 90.f, y});
        else
            txt.setPosition({x + 5.f, y});
        txt.setFillColor(sf::Color::White);
        CE::Render::Get().AddToDraw(txt);
    }

    void EscenaMatch::dibujarConteo(int conteo, float x, float y)
    {
        sf::Font& font = CE::GestorAssets::Get().getFont("nova");
        std::string msg = std::to_string(conteo) + "!";
        sf::Text txt(font, msg, 72);
        txt.setPosition({x, y});
        txt.setFillColor(sf::Color::White);
        txt.setOutlineColor(sf::Color::Black);
        txt.setOutlineThickness(3.f);
        CE::Render::Get().AddToDraw(txt);
    }

    void EscenaMatch::dibujarBarraEscape(float progreso, float x, float y)
    {
        float ancho = 200.f;
        float alto = 16.f;

        sf::RectangleShape fondo{{ancho, alto}};
        fondo.setPosition({x, y});
        fondo.setFillColor(sf::Color{60,60,60});
        fondo.setOutlineColor(sf::Color::White);
        fondo.setOutlineThickness(1.f);
        CE::Render::Get().AddToDraw(fondo);

        float ratio = progreso / 100.f;
        if(ratio > 1.f) ratio = 1.f;
        sf::RectangleShape barra{{ancho * ratio, alto}};
        barra.setPosition({x, y});
        barra.setFillColor(sf::Color{100,200,255});
        CE::Render::Get().AddToDraw(barra);

        sf::Font& font = CE::GestorAssets::Get().getFont("nova");
        sf::Text txt(font, "MASH SPACE!", 12);
        txt.setPosition({x + 50.f, y});
        txt.setFillColor(sf::Color::White);
        CE::Render::Get().AddToDraw(txt);
    }
}