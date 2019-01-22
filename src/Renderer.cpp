#include <iostream>

#include "Renderer.hpp"

Renderer::Renderer( unsigned thicknessGrid, sf::Color colorGrid,
                    unsigned int sizeCell,
                    unsigned int totalLine, unsigned int totalColumn
                    ):
totalLine_(totalLine),
totalColumn_(totalColumn),
sizeCell_(sizeCell),
widthPixelGame_(totalColumn_ * sizeCell_),
heightPixelGame_(totalLine * sizeCell_),
widthPixelWindow_(widthPixelGame_ + 200),
heightPixelWindow_(heightPixelGame_ + 150),
positionX_((widthPixelWindow_ - widthPixelGame_) / 2),
window_(sf::VideoMode(widthPixelWindow_,heightPixelWindow_), "Game of Life"),
gridActivate_(false),
thicknessGrid_(thicknessGrid),
colorGrid_(colorGrid),
button_(nullptr)
{
    unsigned int titleTextSize = 40;
    unsigned int widthButton = 150;
    unsigned int heightButton = 60;

    window_.setFramerateLimit(60);

    /* Resources necessary */ 
    if(!font_.loadFromFile("res/Chunkfive.otf")) {
        std::cerr << "Error at loading ..." << std::endl;
    }

    /* Building the title */
    title_ = sf::Text("Game of life",font_, titleTextSize);
    title_.setColor(sf::Color::Black);
    unsigned int positionXTitle = (widthPixelWindow_ - title_.getLocalBounds().width) / 2 ;
    unsigned int positionYTitle = 10;
    title_.setPosition(positionXTitle,positionYTitle);

    positionY_ = (positionYTitle*3) + title_.getLocalBounds().height;

    /* Building the button stop / play */
    unsigned int positionXButton = (widthPixelWindow_ - widthButton) / 2;
    unsigned int positionYButton = heightPixelGame_ + positionY_ + 20;
    button_ = new Button(widthButton,heightButton,positionXButton,positionYButton, 30, sf::Color::White, sf::Color::Black);

    vertices_ = sf::VertexArray(sf::Quads, totalColumn_ * totalLine_ * 4);
    grid_ = sf::VertexArray(sf::Quads, (totalColumn_ + totalLine_ + 2) * 4);
    
    for(unsigned int i = 0; i < totalLine_; i++) {
        for(unsigned int j = 0; j < totalColumn_; j++) {
            int position = i * totalColumn_ + j;
            int x = positionX_ +  i * sizeCell_;
            int y = positionY_ + j * sizeCell_;

            vertices_[position * 4 + 0].position = sf::Vector2f(x,y);
            vertices_[position * 4 + 0].color = sf::Color::Black;

            vertices_[position * 4 + 1].position = sf::Vector2f(x + sizeCell_, y);
            vertices_[position * 4 + 1].color = sf::Color::Black;

            vertices_[position * 4 + 2].position = sf::Vector2f(x + sizeCell_, y + sizeCell_);
            vertices_[position * 4 + 2].color = sf::Color::Black;

            vertices_[position * 4 + 3].position = sf::Vector2f(x,y  + sizeCell_);
            vertices_[position * 4 + 3].color = sf::Color::Black;
        }
    }

    unsigned int it = 0;
    /* Initialize Grid lines */
    for(unsigned int i = 0; i < totalColumn_ + 1 ; i++) {
        grid_[it * 4 + 0].position = sf::Vector2f(0 + positionX_,i * sizeCell_ + positionY_);
        grid_[it * 4 + 0].color = colorGrid_;

        grid_[it * 4 + 1].position = sf::Vector2f(widthPixelGame_ + positionX_, i * sizeCell_ + positionY_);
        grid_[it * 4 + 1].color = colorGrid_;
                
        grid_[it * 4 + 2].position = sf::Vector2f(widthPixelGame_ + positionX_, i * sizeCell_ + thicknessGrid_ + positionY_);
        grid_[it * 4 + 2].color = colorGrid_;

        grid_[it * 4 + 3].position = sf::Vector2f(0 + positionX_, i * sizeCell_ + thicknessGrid_ + positionY_);
        grid_[it * 4 + 3].color = colorGrid_;

        it++;
    }

    /* Initialize Grid Columns */
    for(unsigned int i = 0; i < totalLine_ + 1; i++) {

        grid_[it * 4 + 0].position = sf::Vector2f(i * sizeCell_ + positionX_, 0 + positionY_);
        grid_[it * 4 + 0].color = colorGrid_;

        grid_[it * 4 + 1].position = sf::Vector2f(i * sizeCell_ + thicknessGrid_ + positionX_, 0 + positionY_);
        grid_[it * 4 + 1].color = colorGrid_;
                
        grid_[it * 4 + 2].position = sf::Vector2f(i * sizeCell_ + thicknessGrid_ + positionX_, heightPixelGame_ + positionY_);
        grid_[it * 4 + 2].color = colorGrid_;

        grid_[it * 4 + 3].position = sf::Vector2f(i * sizeCell_ + positionX_, heightPixelGame_ + positionY_);
        grid_[it * 4 + 3].color = colorGrid_;

        it++;
    }
}

Renderer::~Renderer()
{
    delete button_;
}

void Renderer::ManageEvents(bool& isPaused) 
{
    sf::Event event;
    while (GetWindow().pollEvent(event))
    {
        GetStopPlayButton().ManageMouseEvent(GetWindow());

        if (event.type == sf::Event::Closed)
            GetWindow().close();
        if (event.type == sf::Event::MouseButtonReleased) {
            if(event.mouseButton.button == sf::Mouse::Left) {
                if(GetStopPlayButton().ContainMouse(GetWindow())) {
                    GetStopPlayButton().ChangeText();
                    if(isPaused) {
                        isPaused = false;
                    } else {
                        isPaused = true;
                    }
                }
            }
        }
    }              
}

sf::RenderWindow& Renderer::GetWindow()
{
    return window_;
}

Button& Renderer::GetStopPlayButton()
{
    return *button_;
}

sf::Color Renderer::GetColorAccordingState(int state) const
{
    if(state == 0) { // DEAD
        return sf::Color::Black;
    } else if(state == 1) { // STABLE
        return sf::Color::Blue;
    } else if(state == 2) { // BIRTH
        return sf::Color::Green;
    } else if(state == 3) {
        return sf::Color::Red;
    }else { // ERROR
        return sf::Color::White;
    }
}

void Renderer::UpdateColor(int i, int j, int state)
{
    unsigned int position = j * totalColumn_ + i;
    sf::Color color = GetColorAccordingState(state);

    vertices_[position * 4 + 0].color = color;
    vertices_[position * 4+ 1].color = color;
    vertices_[position * 4 + 2].color = color;
    vertices_[position * 4 + 3].color = color;
}

void Renderer::Draw()
{
    window_.draw(vertices_);
    window_.draw(title_);
    button_->Draw(window_);
    if(gridActivate_) {
        window_.draw(grid_);
    }
}


bool Renderer::UpdateStartingScenario(unsigned int line, unsigned int column, int state, std::vector<std::vector<int>>& startingCells)
{
    if((int)line < 0 || (int)column < 0 || line >= totalLine_ || column >= totalColumn_) {
        return false;
    }
    startingCells[line][column] = state;
    UpdateColor(line,column,state);
    return true;
}



bool Renderer::GenerateU(unsigned int lineStart, unsigned int columnStart, std::vector<std::vector<int>>& startingCells)
{
    if(!UpdateStartingScenario(lineStart,columnStart,2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 1,columnStart,2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 2,columnStart,2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 2,columnStart + 1,2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 2,columnStart + 2,2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 1,columnStart + 2,2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart,columnStart + 2,2,startingCells)){
        return false;
    }

    return true;
}

bool Renderer::GenerateShip(unsigned int lineStart, unsigned int columnStart, std::vector<std::vector<int>>& startingCells)
{
    if(!UpdateStartingScenario(lineStart,columnStart, 2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 1,columnStart + 1, 2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 2,columnStart + 1, 2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 2,columnStart, 2,startingCells)){
        return false;
    }
    if(!UpdateStartingScenario(lineStart + 2,columnStart - 1, 2,startingCells)){
        return false;
    }

    return true;
}
