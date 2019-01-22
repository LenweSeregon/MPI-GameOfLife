#ifndef __RENDERER__
#define __RENDERER__

#include <SFML/Graphics.hpp>

#include "Button.hpp"

class Renderer
{
    private:

    unsigned int totalLine_;
    unsigned int totalColumn_;
    unsigned int sizeCell_;

    unsigned int widthPixelGame_;
    unsigned int heightPixelGame_;

    unsigned int widthPixelWindow_;
    unsigned int heightPixelWindow_;

    unsigned int positionX_;
    unsigned int positionY_;

    sf::RenderWindow window_;

    bool gridActivate_;
    unsigned int thicknessGrid_;
    sf::Color colorGrid_;

    Button* button_;
    
    sf::Font font_;
    sf::Text title_;

    sf::VertexArray grid_;
    sf::VertexArray vertices_;

    public:

    Renderer(   unsigned thicknessGrid, sf::Color colorGrid,
                unsigned int sizeCell,
                unsigned int totalLine, unsigned int totalColumn);
    ~Renderer();
    
    sf::RenderWindow& GetWindow();
    Button& GetStopPlayButton();
    sf::Color GetColorAccordingState(int state) const;
    void UpdateColor(int i, int j, int state);
    void Draw();
    void ManageEvents(bool& isPaused);

    bool UpdateStartingScenario(unsigned int line, unsigned int column, int state, std::vector<std::vector<int>>& startingCells);
    bool GenerateU(unsigned int lineStart, unsigned int columnStart, std::vector<std::vector<int>>& startingCells);
    bool GenerateShip(unsigned int lineStart, unsigned int columnStart, std::vector<std::vector<int>>& startingCells);
};

#endif