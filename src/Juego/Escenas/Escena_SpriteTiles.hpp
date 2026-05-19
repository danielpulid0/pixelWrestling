#pragma once
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Primitivos/Escena.hpp>
#include <memory>
namespace IVJ
{
class Escena_SpriteTiles : public CE::Escena
{
  public:
    explicit Escena_SpriteTiles(std::shared_ptr<Entidad> &ref);
    virtual ~Escena_SpriteTiles() {};
    void onInit() override;
    void onFinal() override;
    void onUpdate(float dt) override;
    void onInputs(const CE::Botones &accion) override;
    void onRender() override;
    std::shared_ptr<Entidad> getJugador() override
    {
        return jugador_ref;
    }
  private:
    int inicializar{1};
    std::shared_ptr<Entidad> &jugador_ref;
    //se agrego esto
    std::shared_ptr<Entidad> obstaculo;
};
} // namespace IVJ