#pragma once
#include "../Maquinas/FSM.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../../Motor/Primitivos/CEPool.hpp"
namespace IVJ
{
    class FSM; //refefencia circular
    //Maquina estado componente
    class IMaquinaEstado : public CE::IComponentes
    {
        public:
            explicit IMaquinaEstado();
            ~IMaquinaEstado() override{};
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IMaquinaEstado>(*this);
            };
        public:
            std::shared_ptr<FSM> fsm;
            bool congelar{false}; //
    };

    //componente QUEST con un arreglo de int de 8 bit

    class IRayo : public CE::IComponentes
    {
        public:
            explicit IRayo(CE::Vector2D& pos,float& sig,const float magnitud=1.f );
            ~IRayo() override{};
        private:
            float magnitud;
            float& dir;
            CE::Vector2D& lp1;
            CE::Vector2D lp2;
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IRayo>(*this);
            };
        public:
            CE::Vector2D& getP1() const;
            CE::Vector2D& getP2();
            float getMagnitud() const {return magnitud;}
    };

    class IInteractuable : public CE::IComponentes
    {
        public:
            IInteractuable()=default;
            virtual ~IInteractuable() =default;
            virtual void onInteractuar(CE::Objeto& obj)=0;

            std::shared_ptr<IComponentes> clonar() const override = 0;

        public:
            bool interactuado{false};
    };

    class IIndicador : public IInteractuable
    {
        public:
            explicit IIndicador(const sf::Texture &ref,float escala,Entidad *p, const sf::Color& c);
            virtual ~IIndicador() =default;
            void onInteractuar(CE::Objeto& obj) override;
            void onRender();
            std::shared_ptr<IComponentes> clonar() const override
            {
                return nullptr;
            };
        public:
            sf::Sprite sprite;
            sf::Shader shader;
            sf::Color color;
            float escala;
            int frame_activo{0};
            bool activo{false};
        private:
            Entidad* parent;
    };

    class IDialogo : public IInteractuable
    {
        public:
            IDialogo();
            virtual ~IDialogo() = default;
            void onInteractuar(CE::Objeto& obj) override;
            void onRender();
            std::shared_ptr<IComponentes> clonar() const override
            {
                return nullptr;
            };
        private:
            std::wstring agregarSaltoLinea(const std::wstring& str, size_t max_len);
        public:
            std::wstring texto;
            int id_texto;
            bool activo{false};
    };

    class IGirar : public CE::IComponentes
    {
        public:
            explicit IGirar(const float ang,const float r);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IGirar>(*this);
            };
        public:
            float angulo;
            float radio;
    };

    class IUpDown : public CE::IComponentes
    {
        public:
            explicit IUpDown(const float vel, const float lim_sup, const float lim_inf);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IUpDown>(*this);
            };
        public:
            float velocidad;
            int direccion;
            float limite_superior;
            float limite_inferior; 
    };
   
    class IOnda : public CE::IComponentes
    {
        public:
            explicit IOnda(float amp, float freq, float vel_x, float lim_izq, float lim_der);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IOnda>(*this);
            };
        public:
            float amplitud;     
            float frecuencia;   
            float velocidad_x;  
            float tiempo;        
            float pos_y_inicial; 
            float limite_izquierdo;
            float limite_derecho;
            int direccion_x;
    };

    enum class EstadoSalud { NORMAL, ENFERMO, RECUPERACION, INMUNE, MUERTO };

    class IEstadoEpidemia : public CE::IComponentes
    {
        public:
            explicit IEstadoEpidemia(int x, int y) 
                : grid_x(x), grid_y(y), estado_actual(EstadoSalud::NORMAL), 
                  tiempo_transmision(0.0f), tiempo_recuperacion(0.0f), 
                  inmunidad_parcial(0.0f), tick_segundo(0.0f),
                  // Shader uniform values viven aquí para evitar dangling pointers
                  shader_color{0.2f, 0.1f, 0.4f, 1.0f},
                  shader_thickness(0.1f),
                  shader_size{40.0f, 40.0f} {}
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IEstadoEpidemia>(*this);
            };
        public:
            int grid_x;
            int grid_y;
            EstadoSalud estado_actual;
            float tiempo_transmision;
            float tiempo_recuperacion;
            float inmunidad_parcial;
            float tick_segundo;
            // Valores de shader con vida útil segura
            sf::Glsl::Vec4 shader_color;
            float shader_thickness;
            sf::Glsl::Vec2 shader_size;
    };

    class ITriangulo : public CE::IComponentes{
        public:
            explicit ITriangulo(float r, float ang);
            ~ITriangulo() override {};
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<ITriangulo>(*this);
            };
        public: 
            sf::CircleShape tri_shape;
            float angulo;
    };

    class ITarget : public CE::IComponentes
    {
        public:
            //puede ser nula por eso *
            explicit ITarget(CE::Objeto * target);
            void setTarget(CE::Objeto& t);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<ITarget>(*this);
            };
        public:
            CE::Vector2D* pos;
            CE::Objeto* target_obj{nullptr};
    };
    
    class IRangoAggro : public CE::IComponentes
    {
        public:
            explicit IRangoAggro(float radio);
            virtual ~IRangoAggro(){};
            bool estaDentroRango(CE::Objeto& parent, CE::Vector2D& target);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IRangoAggro>(*this);
            };
        public:
            float radio;
            sf::CircleShape marcador;
    };

    //=== COMPONENTES DE COMBATE (Lucha Libre) ===

    class ILuchadorStats : public CE::IComponentes
    {
        public:
            explicit ILuchadorStats(int vel, int fuerza, int resistencia);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<ILuchadorStats>(*this);
            };
        public:
            int velocidad;    // 1-10
            int fuerza;       // 1-10
            int resistencia;  // 1-10
    };

    class ICombate : public CE::IComponentes
    {
        public:
            ICombate();
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<ICombate>(*this);
            };
        public:
            bool esta_atacando{false};
            bool fue_golpeado{false};
            bool esta_derribado{false};
            bool esta_aturdido{false};
            bool en_caida{false};
            bool en_levantarse{false};
            bool pinfall_listo{false};  // solo se activa tras un finisher knockdown
            int causa_caida{0};         // 0=ninguna, 1=comun, 3=sillazo, 4=remate
            float cooldown_ataque{0.f};
            float tiempo_derribo{0.f};
            float tiempo_aturdido{0.f};
            // 0=ninguno, 1=golpe, 2=patada, 3=item, 4=remate
            int tipo_ataque{0};
            int golpes_consecutivos{0};
            static const int MAX_GOLPES_CAIDA = 10;
            int danio_base_golpe{5};
            int danio_base_patada{5};
            int danio_base_item{10};
            int danio_base_remate{25};
            bool tiene_silla{false};
    };

    class IDefensa : public CE::IComponentes
    {
        public:
            IDefensa();
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IDefensa>(*this);
            };
        public:
            bool defendiendo{false};
            int golpes_bloqueados{0};
            static const int MAX_BLOQUEOS = 3;
            float tiempo_rotura{2.f};
            float cooldown_defensa{0.f};
            float flash_timer{0.f};         // timer del parpadeo al bloquear
    };

    class IMomentum : public CE::IComponentes
    {
        public:
            IMomentum();
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IMomentum>(*this);
            };
        public:
            float valor{0.f};
            static constexpr float MAX_MOMENTUM = 100.f;
            bool remate_disponible{false};
    };

    class IItem : public CE::IComponentes
    {
        public:
            enum TipoItem { SILLA = 0 };
            explicit IItem(TipoItem tipo);
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IItem>(*this);
            };
        public:
            TipoItem tipo;
            float danio_extra{40.f};
            bool recogido{false};
    };

    //=== CONTEO (Pinfall) ===
    class IConteo : public CE::IComponentes
    {
        public:
            IConteo();
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IConteo>(*this);
            };
        public:
            bool en_conteo{false};
            int cuenta{0};           // 0, 1, 2, 3
            float timer_cuenta{0.f}; // tiempo acumulado por cuenta
            float intervalo{1.5f};   // segundos entre cada cuenta
            float barra_escape{0.f}; // para el mash del jugador (0-100)
    };
    
    class IEmisor : public CE::IComponentes
    {
        public:
            explicit IEmisor(Entidad &parent, Entidad &prefab);
            ~IEmisor(void) {};
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IEmisor>(*this);
            };
            void onUpdate(float dt);
            void crearParticula(void);
            CE::Pool &getPool(void)
            {
                return pool;
            }

        private:
            CE::Pool pool;
            Entidad &parent; // quien tiene el emisor
            Entidad &prefab; // el objeto que se va copiar
    };
}
