#include "Escena_SpriteTiles.hpp"
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
#include <Juego/Maquinas/Naves/IdleJugadores.hpp>
#include <Juego/Maquinas/Bosses/IdleBoss.hpp>
#include <Juego/Maquinas/Balas/BalasIdle.hpp>
#include <Juego/Maquinas/Balas/BalasBoom.hpp>

namespace IVJ
{
    Escena_SpriteTiles::Escena_SpriteTiles(std::shared_ptr<Entidad>& pref) :CE::Escena{},jugador_ref{pref}{}

    void Escena_SpriteTiles::onInit()
    {
        CE::GestorCamaras::Get().setCamaraActiva(2);
        CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(jugador_ref);
        if (!inicializar)
            return;
        registrarBotones(sf::Keyboard::Scancode::W, "arriba");
        registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
        registrarBotones(sf::Keyboard::Scancode::S, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
        registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::D, "derecha");
        registrarBotones(sf::Keyboard::Scancode::Right, "derecha");
        registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
        // lab9 colisiones
        registrarBotones(sf::Keyboard::Scancode::Space, "atacar");

        // cargar mapa
        tiles_layers.push_back(TileMap());
        tiles_layers.push_back(TileMap());
        tiles_layers.push_back(TileMap());

        // se agrego la posición inicial del tilemap --v
        tiles_layers[0].setPosition({-500, -500});
        // se agrego la posición inicial de la colisiones --v
        if (!tiles_layers[0].loadTileMap(ASSETS "/mapas/playa_col_layer1.txt", objetos, {-500.f, -500.f}))
            exit(EXIT_FAILURE);
        if (!tiles_layers[1].loadTileMap(ASSETS "/mapas/playa_layer2.txt"))
            exit(EXIT_FAILURE);
        if (!tiles_layers[2].loadTileMap(ASSETS "/mapas/playa_layer3.txt"))
            exit(EXIT_FAILURE);

        // Lab 8 animacinoes
        CE::GestorAssets::Get().agregarTextura("nave_sheet",                             // key
                                            ASSETS "/sprites/naves/player_sheet.png", // hoja
                                            CE::Vector2D{0.f, 0.f},                   // inicio
                                            CE::Vector2D{320.f, 64.f}                 // dim de la hoja
        );

        CE::GestorAssets::Get().agregarTextura("boss_sheet", ASSETS "/sprites/bosses/VileCrustaceanIdleAttack.png",
                                            CE::Vector2D{0.f, 0.f}, CE::Vector2D{924.f, 260.f});
        //texturas balas
        CE::GestorAssets::Get().agregarTextura("balas1", ASSETS "/sprites/balas1.png", CE::Vector2D{0.f, 0.f},
                                            CE::Vector2D{16 * 3, 16});
        // textura obstaculo
        CE::GestorAssets::Get().agregarTextura("sprite1", ASSETS "/sprites/sprite1.png", CE::Vector2D{0.f, 0.f},
                                            CE::Vector2D{128, 256});

        auto trans = jugador_ref->getTransformada();
        trans->velocidad = CE::Vector2D{500.f, 500.f};
        jugador_ref->setPosicion(462.f, 273.f);

        // Lab 8 animaciones
        auto sprite = std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("nave_sheet"), // textura
                                                    64, 64,                                           // dim
                                                    1.f);                                             // escala

        jugador_ref->addComponente(sprite);
        jugador_ref->addComponente(std::make_shared<CE::IControl>());

        // lab 8 animaciones
        auto me = std::make_shared<IMaquinaEstado>();
        me->fsm = std::make_shared<IdleJugadores>();
        jugador_ref->addComponente(me);
        // ejecuta onEntrar para inicializar variables
        jugador_ref->setFSM(me->fsm);

        // lab9 colisiones
        // el parámetro es dim, se refiere a las dimensiones del sprite
        jugador_ref->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{64.f, 64.f}));

        // lab11 Prefab
        static auto bala_prefab = std::make_shared<Entidad>();
        bala_prefab->getStats()->hp_max = 1;
        bala_prefab->getStats()->hp = 1;
        bala_prefab->getStats()->str = 64;
        bala_prefab->getTransformada()->velocidad = CE::Vector2D{750.f, 750.f};
        auto bala_sprite = std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("balas1"), 16, 16, 3.f);
        bala_prefab->addComponente(bala_sprite);
        bala_prefab->addComponente(std::make_shared<CE::IBoundingBox>(16));
        bala_prefab->addComponente(std::make_shared<CE::ITimer>(180)); // 3seg a 60fps
        auto bala_me = std::make_shared<IVJ::IMaquinaEstado>();
        bala_prefab->addComponente(bala_me);
        auto emisor = std::make_shared<IEmisor>(*jugador_ref, *bala_prefab);
        jugador_ref->addComponente(emisor);
        //fin lab11 prefab
        // Sonido
        CE::GestorAssets::Get().agregarSonido("tetirorayo", ASSETS "/sonidos/tetirorayo.ogg");
        // shader

        // boss entidad
        auto boss = std::make_shared<Entidad>();
        // lab9 colisiones
        boss->getStats()->hp_max = 255;
        boss->getStats()->hp = 255;
        boss->getStats()->str = 150;
        boss->getStats()->agi = 50;
        boss->getStats()->def = 255;

        auto boss_sprite =
            std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("boss_sheet"), 154.f, 130.f, 1.f);

        auto boss_me = std::make_shared<IVJ::IMaquinaEstado>();
        boss_me->fsm = std::make_shared<IVJ::IdleBoss>(6, 0.25f);
        auto boss_target = std::make_shared<ITarget>(nullptr);
        boss_target->setTarget(*jugador_ref);

        boss->addComponente(boss_sprite)
            .addComponente(boss_me)
            .addComponente(std::make_shared<IRangoAggro>(500.f))
            .addComponente(boss_target)
            .setPosicion(500.f, 500.f);
        boss->setFSM(boss_me->fsm);

        auto shader = std::make_shared<CE::IShader>("", ASSETS "/shaders/bossfrag.frag");

        static sf::Texture boss_tex = boss_sprite->m_sprite.getTexture();

        shader->setTextura("textura", &boss_tex);

        boss->addComponente(shader);

        objetos.agregarPool(boss);

        // lab9 colisinoes
        obstaculo = std::make_shared<Entidad>();
        obstaculo->getStats()->hp_max = 100;
        obstaculo->getStats()->hp = 100;
        obstaculo->getStats()->def = 150;

        auto obstaculo_sprite = std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("sprite1"), 128, 256, 1.f);
        obstaculo_sprite->m_sprite.setOrigin({64, 128});
        obstaculo->addComponente(obstaculo_sprite)
            .addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{128, 256}));

        obstaculo->setPosicion(1300, 175);

        auto t_shader = std::make_shared<CE::IShader>("", ASSETS "/shaders/agua.frag");
        static sf::Texture p_tex = obstaculo_sprite->m_sprite.getTexture();
        static float wescala = 0.025;
        static float vescala = 1.5;
        static float frec = 5.0;

        t_shader->setTextura("textura", &p_tex);
        t_shader->setEscalar("escala_distorcion", &wescala);
        t_shader->setEscalar("vel_distorcion", &vescala);
        t_shader->setEscalar("frecuencia", &frec);

        obstaculo->addComponente(t_shader);

        objetos.agregarPool(obstaculo);

        inicializar = false;
    }

    void Escena_SpriteTiles::onFinal(){
         //reseteamos la camara a la estática al salir/cambiar de escena
        CE::GestorCamaras::Get().setCamaraActiva(0);
    }

    void Escena_SpriteTiles::onUpdate(float dt)
    {

        jugador_ref->inputFSM();
        jugador_ref->onUpdate(dt);
        SistemaMover(jugador_ref, dt);
        jugador_ref->getComponente<IEmisor>()->onUpdate(dt);
        for (auto &obj : objetos.getPool())
        {
            obj->inputFSM();
            obj->onUpdate(dt);
            if (obj->tieneComponente<ITarget>())
            {
                auto &tpos = *obj->getComponente<ITarget>()->pos;
                SistemaNPCLookTarget(*obj, tpos);
            }
            // lab9
            SistemaColAABBMid(*jugador_ref, *obj, true);
        }
        //lab11 prefab
        for (auto &bala : jugador_ref->getComponente<IEmisor>()->getPool().getPool())
        {
            if (obstaculo)
            {
                if (SistemaColAABBMid(*bala, *obstaculo, true))
                {
                    ((Entidad &)*bala).setFSM(std::make_shared<BalasBoom>(0.33f));
                    bala->getComponente<IMaquinaEstado>()->congelar = true;
                    // daño una sola vez
                    bala->getTransformada()->velocidad = CE::Vector2D{0.f, 0.f};
                    if (obstaculo->getStats()->congelar == false)
                    {
                        float mitigar = obstaculo->getStats()->def / 255.f;
                        obstaculo->getStats()->hp -= (mitigar * bala->getStats()->str);
                        obstaculo->getStats()->congelar = true;
                    }
                }

                if (bala->getComponente<CE::ITimer>()->frame_actual >= bala->getComponente<CE::ITimer>()->frame_maximo)
                {
                    obstaculo->getStats()->congelar = false;
                }
            }
        }
        if (obstaculo)
        {
            static float acum_tiempo = 0;
            acum_tiempo += dt;
            obstaculo->getComponente<CE::IShader>()->setEscalar("dt", &acum_tiempo);
            //quitar la referencia, para que elimine el puntero
            if (obstaculo->getStats()->hp <= 0)
                obstaculo = nullptr;
        }
        // lab9 borra toda entidad que esta en el pool que hp sea <=0
        objetos.borrarPool();
    }

    void Escena_SpriteTiles::onInputs(const CE::Botones &accion)
    {
        switch (accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress: {
                if (accion.getNombre() == "arriba")
                {
                    jugador_ref->getComponente<CE::IControl>()->arr = true;
                }
                if (accion.getNombre() == "abajo")
                {
                    jugador_ref->getComponente<CE::IControl>()->abj = true;
                }
                if (accion.getNombre() == "derecha")
                {
                    jugador_ref->getComponente<CE::IControl>()->der = true;
                }
                if (accion.getNombre() == "izquierda")
                {
                    jugador_ref->getComponente<CE::IControl>()->izq = true;
                }

                if (accion.getNombre() == "aceptar")
                {
                }
                // lab9 colisiones y lab11 prefab
                if (accion.getNombre() == "atacar")
                {
                    jugador_ref->getComponente<CE::IControl>()->acc = true;
                    jugador_ref->getComponente<IEmisor>()->crearParticula();
                }
                break;
            }
            case CE::Botones::TipoAccion::OnRelease: {
                if (accion.getNombre() == "arriba")
                {
                    jugador_ref->getComponente<CE::IControl>()->arr = false;
                }
                if (accion.getNombre() == "abajo")
                {
                    jugador_ref->getComponente<CE::IControl>()->abj = false;
                }
                if (accion.getNombre() == "derecha")
                {
                    jugador_ref->getComponente<CE::IControl>()->der = false;
                }
                if (accion.getNombre() == "izquierda")
                {
                    jugador_ref->getComponente<CE::IControl>()->izq = false;
                }
                // lab9 colisiones
                if (accion.getNombre() == "atacar")
                {
                    jugador_ref->getComponente<CE::IControl>()->acc = false;
                }
                break;
            }
            case CE::Botones::TipoAccion::None: {
                break;
            }
        }
    }

    //render
    void Escena_SpriteTiles::onRender()
    {
        for (auto &al : tiles_layers)
            CE::Render::Get().AddToDraw(al);

        for (auto &obj : objetos.getPool())
        {
            // Lab 8 animaciones
            if (obj->tieneComponente<IRangoAggro>())
            {
                auto rango = obj->getComponente<IRangoAggro>();
                auto pos = obj->getTransformada()->posicion;
                rango->marcador.setPosition({pos.x, pos.y});
                CE::Render::Get().AddToDraw(rango->marcador);
            }
            CE::Render::Get().AddToDraw(*obj);
        }
        //lab11 prefab
        for (auto &p : jugador_ref->getComponente<IEmisor>()->getPool().getPool())
        {
            CE::Render::Get().AddToDraw(*p);
        }
        CE::Render::Get().AddToDraw(*jugador_ref);

        #if DEBUG
            auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
            auto csv = (CE::CamaraSnapVentana *)cam;
            auto csvpos = csv->getTransformada().posicion;
            sf::RectangleShape debugcam{{csv->m_vdim.x, csv->m_vdim.y}};
            debugcam.setOrigin({csv->m_vdim.x / 2.f, csv->m_vdim.y / 2.f});
            debugcam.setPosition({csvpos.x, csvpos.y});
            debugcam.setOutlineThickness(3.f);
            debugcam.setOutlineColor(sf::Color::Yellow);
            debugcam.setFillColor(sf::Color::Transparent);
            CE::Render::Get().AddToDraw(debugcam);
        #endif
    }
}