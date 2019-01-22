#ifndef __BUTTON__
#define __BUTTON__

#include <SFML/Graphics.hpp>

class Button
{

    private:

    sf::RectangleShape button_;
    sf::Text text_;
    sf::Font font_;

    public:

    Button( unsigned int width, unsigned int height, 
            unsigned int positionX, unsigned int positionY,
            unsigned int textSize,
            sf::Color buttonColor, sf::Color textColor);

    bool ContainMouse(sf::RenderWindow& window);
    bool HasBeenPressed(sf::RenderWindow& window);

    void ManageMouseEvent(sf::RenderWindow& window);
    void ChangeText();
    void Draw(sf::RenderWindow& window);
};

#endif