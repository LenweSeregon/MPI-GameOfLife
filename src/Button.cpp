#include <iostream>

#include "Button.hpp"

Button::Button( unsigned int width, unsigned int height, 
            unsigned int positionX, unsigned int positionY,
            unsigned int textSize,
            sf::Color buttonColor, sf::Color textColor)
{
    button_ = sf::RectangleShape();
    button_.setPosition(positionX, positionY);
    button_.setSize(sf::Vector2f(width,height));
    button_.setFillColor(buttonColor);
    button_.setOutlineThickness(2);
    button_.setOutlineColor(sf::Color::Black);

    if(!font_.loadFromFile("res/Chunkfive.otf")) {
        std::cerr << "Error at loading ..." << std::endl;
    }

    text_ = sf::Text("Play",font_,textSize);

    unsigned int positionXText = positionX + ((width - text_.getLocalBounds().width) / 2);
    unsigned int positionYText = positionY + ((height - text_.getLocalBounds().height) / 2) - 5;

    text_.setColor(textColor);
    text_.setPosition(positionXText, positionYText);
    
}

bool Button::ContainMouse(sf::RenderWindow& window)
{
    int posX = sf::Mouse::getPosition(window).x;
    int posY = sf::Mouse::getPosition(window).y;

    return button_.getGlobalBounds().contains(posX, posY);
}

void Button::ManageMouseEvent(sf::RenderWindow& window)
{
    if(ContainMouse(window)) {
        sf::Color gray(180,180,180);
        button_.setFillColor(gray);
    } else {
        button_.setFillColor(sf::Color::White);
    }
}

void Button::ChangeText()
{
    if(text_.getString() == "Stop") {
        text_.setString("Play");
    } else {
        text_.setString("Stop");
    }
}

void Button::Draw(sf::RenderWindow& window)
{
    window.draw(button_);
    window.draw(text_);
}
