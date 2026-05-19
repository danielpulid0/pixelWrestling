#pragma once
#include <Juego/objetos/Entidad.hpp>

namespace IVJ {
  class EnteVibora : public IVJ::Entidad {
  public:
    EnteVibora();
    void onUpdate(float dt) override;
  };
} 