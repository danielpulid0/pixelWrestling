#pragma once

#include "../../Motor/Primitivos/Objetos.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    class Texto : public CE::Objeto
    {
        public:
            Texto(const sf::Font& f, const std::string &t);
            void onUpdate(float dt) override;
            void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
            void setFontSize(unsigned int size)
            {
                font_size = size;
                texto.m_texto.setCharacterSize(font_size);
            }
             void setColor(const sf::Color &color)
            {
                texto.m_texto.setFillColor(color);
            };
            //le metí este para la escena epidemia
            void setString(const std::string& str)
            {
                texto.m_texto.setString(str);
            };

            void setPosicion(float x, float y)
            {
                CE::Objeto::setPosicion(x, y);
                texto.m_texto.setPosition({x, y});
            }

            void setOriginCenter()
            {
                auto bounds = texto.m_texto.getLocalBounds();
                // En SFML 3, los miembros son position.x/y y size.x/y en lugar de left/top/width/height
                texto.m_texto.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
            }

        private:
            CE::ITexto texto;
            unsigned int font_size{60u};
    };
}
