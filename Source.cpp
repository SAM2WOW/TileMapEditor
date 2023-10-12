#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/View.hpp>
#include <iostream>
#include <filesystem>
#include <map>
#include <algorithm>
#include "Button.h"
#include "Tile.h"

const int CELLSIZE = 64;
const int TOOLSIZE = 7;
const int RADIOTOOLSIZE = 5;
const int TILESIZE = 9;
const int LAYERSIZE = 3;
const int HINTGRIDSIZE = 3;
const int PANNINGSPEED = 4;

enum Tool { draw, erase, eyedropper, fill, zoom};
int currentTool = Tool::draw;
int currentTile = 0;
int currentLayer = 0;
int brushSize = 0;

int zoomSize = 0;
sf::Vector2i zoomOffset(0, 0);

sf::View guiView(sf::FloatRect(0, 0, 1280, 720));
sf::View worldView(sf::FloatRect(0, 0, 1280, 720));

bool drawing = false;

sf::Vector2i lastGridPos(0, 0);

struct tileData {
    int tile;
    std::string meta;
};

std::map<int, std::map<int, std::map<int, tileData>>> tileMap;

bool isOverBtn = false;

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Super Tile Map Editor");

    // Load the font from a file2
    sf::Font MyFont;
    if (!MyFont.loadFromFile("src/assets/font.ttf"))
    {
        // Error...
    }

    sf::Text posText;
    posText.setFont(MyFont);
    posText.setCharacterSize(16);
    posText.setFillColor(sf::Color::White);
    posText.setPosition(20.f, 10.f);

    sf::Text brushText;
    brushText.setFont(MyFont);
    brushText.setCharacterSize(16);
    brushText.setFillColor(sf::Color::White);

    // add tool buttons
    Button toolBtn1(sf::Vector2f(20.f, 50.f), "Draw", MyFont, "src/assets/draw.png");
    Button toolBtn2(sf::Vector2f(20.f, 120.f), "Erase", MyFont, "src/assets/erase.png");
    Button toolBtn3(sf::Vector2f(20.f, 190.f), "Dropper", MyFont, "src/assets/dropper.png");
    Button toolBtn4(sf::Vector2f(20.f, 260.f), "Fill", MyFont, "src/assets/fill.png");
    Button toolBtn5(sf::Vector2f(20.f, 330.f), "Zoom", MyFont, "src/assets/zoom.png");
    Button saveBtn(sf::Vector2f(20.f, 400.f), "Save", MyFont, "src/assets/save.png");
    Button loadBtn(sf::Vector2f(20.f, 470.f), "Load", MyFont, "src/assets/load.png");

    // add tool bar buttons into an array
    Button toolBar[TOOLSIZE] = { toolBtn1, toolBtn2, toolBtn3, toolBtn4, toolBtn5, saveBtn, loadBtn};

    // default tool 1
    toolBar[0].press(true);

    // add tile buttons
    Button *tileBar = new Button[TILESIZE];
    for (int i = 0; i < TILESIZE; i++)
    {
        tileBar[i] = Button(sf::Vector2f(20.f + i * 70.f, 640.f), std::to_string(i+1), MyFont, "src/tiles/tile (" + std::to_string(i+1) + ").png");
        tileBar[i].setIcon("src/tiles/tile (" + std::to_string(i+1) + ").png");
    }
    tileBar[0].press(true);

    // add layer buttons
    Button layerBar[LAYERSIZE];
    for (int i = 0; i < LAYERSIZE; i++)
    {
        layerBar[i] = Button(sf::Vector2f(20.f + i * 70.f, 570.f), "Layer " + std::to_string(i), MyFont, "src/assets/layer.png");
        layerBar[i].setIcon("src/assets/layer.png");
    }
    layerBar[0].press(true);


    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseMoved)
            {
                isOverBtn = false;

                for (int i = 0; i < TOOLSIZE; i++)
                {
                    if (toolBar[i].isMouseOver(window))
                    {
                        isOverBtn = true;
                    }
                }

                for (int i = 0; i < TILESIZE; i++)
                {
                    if (tileBar[i].isMouseOver(window))
                    {
                        isOverBtn = true;
                    }
                }

                for (int i = 0; i < LAYERSIZE; i++)
                {
                    if (layerBar[i].isMouseOver(window))
                    {
                        isOverBtn = true;
                    }
                }

            }

            if (event.type == sf::Event::MouseWheelScrolled)
                {
                    std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;

                    if (currentTool == Tool::draw || currentTool == Tool::erase)
                    {
                        brushSize += event.mouseWheelScroll.delta;
                        if (brushSize < 0)
                        {
                            brushSize = 0;
                        }
                    }
                    else if (currentTool == Tool::zoom)
                    {
                        zoomSize += event.mouseWheelScroll.delta;

                        worldView.zoom(1 + event.mouseWheelScroll.delta / 10.f);
                    }
                }
            
            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    drawing = false;

                    if (toolBar[5].isMouseOver(window))
                    {
                        toolBar[5].press(false);

                    }

                    if (toolBar[6].isMouseOver(window))
                    {
                        toolBar[6].press(false);

                    }
                }
            }

            // check for mouse button event
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // convert the position to a grid cordinate
                    int gridX = floor(event.mouseButton.x / CELLSIZE);
                    int gridY = floor(event.mouseButton.y / CELLSIZE);

                    if (!isOverBtn) {
                        drawing = true;

                        // check for tool eyedropper
                        if (currentTool == Tool::eyedropper) 
                        {
                            if (tileMap[currentLayer].find(gridX) != tileMap[currentLayer].end())
                            {
                                if (tileMap[currentLayer][gridX].find(gridY) != tileMap[currentLayer][gridX].end())
                                {
                                    // set the current tile to the tile that is clicked
                                    currentTile = tileMap[currentLayer][gridX][gridY].tile;

                                    for (int i = 0; i < TILESIZE; i++)
                                    {
                                        tileBar[i].press(i == currentTile);
                                    }
                                }
                            }
                        }
                    }
                }

                // check for tool button press (only the first 4 is radio button)
                for (int i = 0; i < RADIOTOOLSIZE; i++)
                {
                    if (toolBar[i].isMouseOver(window))
                    {
                        // clear other button
                        for (int j = 0; j < RADIOTOOLSIZE; j++)
                        {
                            toolBar[j].press(j == i);
                        }
                        currentTool = i;

                        std::cout << "Tool: " << currentTool << std::endl;

                        // click zoom again to reset
                        if (i == Tool::zoom)
                        {
                            std::cout << "Reset Zoom" << std::endl;
                            zoomSize = 0;
                            worldView.zoom(1.f);
                        }
                    }
                }

                // check for other tool button
                if (toolBar[5].isMouseOver(window))
                {
                    std::cout << "Save" << std::endl;
                    toolBar[5].press(true);

                }

                if (toolBar[6].isMouseOver(window))
                {
                    std::cout << "Load" << std::endl;
                    toolBar[6].press(true);

                }

                // check for tile button press
                for (int i = 0; i < TILESIZE; i++)
                {
                    if (tileBar[i].isMouseOver(window))
                    {
                        // clear other button
                        for (int j = 0; j < TILESIZE; j++)
                        {
                            tileBar[j].press(j == i);
                        }
                        currentTile = i;

                        std::cout << "Tile: " << currentTile << std::endl;

                        // for quality of life, also changet the tool to draw
                        currentTool = Tool::draw;
                        for (int j = 0; j < RADIOTOOLSIZE; j++)
                        {
                            toolBar[j].press(j == currentTool);
                        }
                    }
                }

                // check for layer button press
                for (int i = 0; i < LAYERSIZE; i++)
                {
                    if (layerBar[i].isMouseOver(window))
                    {
                        // clear other button
                        for (int j = 0; j < LAYERSIZE; j++)
                        {
                            layerBar[j].press(j == i);
                        }
                        currentLayer = i;

                        std::cout << "Layer: " << currentLayer << std::endl;
                    }
                }
            }
        }

        window.clear(sf::Color(0x1f1f1f00));

        window.setView(worldView);

        // get mouse position in real time
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), worldView);
        lastGridPos.x = floor(round(worldPosition.x) / CELLSIZE);
        lastGridPos.y = floor(round(worldPosition.y) / CELLSIZE);

        posText.setString("Position\tx:" + std::to_string(lastGridPos.x) + "\ty:" + std::to_string(lastGridPos.y));

        ////////////////////////////////////////
        // draw all the drawn tiles from data
        // 
        // TODO: OPTIZE THIS
        ////////////////////////////////////////
        for (auto const& layer : tileMap)
        {
            for (auto const& x : layer.second)
            {
                for (auto const& y : x.second)
                {
                    Tile tile(sf::Vector2f(x.first * CELLSIZE, y.first * CELLSIZE), "src/tiles/tile (" + std::to_string(y.second.tile + 1) + ").png");
                    
                    if (layer.first != currentLayer)
                    {
                        tile.setOpacity(80);
                    }

                    tile.draw(window);
                }
            }
        }

        if (!isOverBtn)
        {
            if (drawing)
            {
                switch (currentTool)
                {
                    case Tool::draw:
                        for (int i = -brushSize; i <= brushSize; i++)
                        {
                            for (int j = -brushSize; j <= brushSize; j++)
                            {
                                tileMap[currentLayer][lastGridPos.x + i][lastGridPos.y + j].tile = currentTile;
                            }
                        }

                        break;
                    
                    case Tool::erase:
                        for (int i = -brushSize; i <= brushSize; i++)
                        {
                            for (int j = -brushSize; j <= brushSize; j++)
                            {
                                tileMap[currentLayer][lastGridPos.x + i].erase(lastGridPos.y + j);
                            }
                        }

                        break;
                    
                    case Tool::eyedropper:
                        break;
                    
                    case Tool::fill:
                        tileMap[currentLayer][lastGridPos.x][lastGridPos.y].tile = currentTile;
                        break;
                    
                    case Tool::zoom:
                        // https://stackoverflow.com/questions/64388688/javsscript-track-mouse-position-relative-to-the-middle-of-the-screen
                        worldView.move(sf::Vector2f(-1 + (localPosition.x / 1280.f) * 2, -1 + (localPosition.y / 720.f) * 2) * float(PANNINGSPEED));

                        break;
                    
                }
            }

            // display cool grid hints
            sf::RectangleShape hintBox(sf::Vector2f(64, 64));

            for (int i = -2 - brushSize; i < 3 + brushSize; i++)
            {
                for (int j = -2 - brushSize; j < 3 + brushSize; j++)
                {
                    hintBox.setPosition((lastGridPos.x + i) * CELLSIZE, (lastGridPos.y + j) * CELLSIZE);
                    hintBox.setFillColor(sf::Color(0, 0, 0, 0));
                    hintBox.setOutlineThickness(1);

                    // change the color of the hint box based on distance from the mouse
                    if (abs(i) < brushSize + 1 && abs(j) < brushSize + 1) {
                        hintBox.setOutlineColor(sf::Color(255, 255, 255, 255));
                    } 
                    else 
                    {
                        int distance = abs(i) + abs(j);
                        hintBox.setOutlineColor(sf::Color(255, 255, 255, std::max(0, 25 / (5 + brushSize) * ((5 + brushSize) - distance - 1))));
                    }

                    window.draw(hintBox);

                }
            }
        }
        
        window.setView(guiView);

        window.draw(posText);

        // draw hint text
        // set the position to the center top of the screen
        brushText.setPosition(window.getSize().x / 2 - brushText.getLocalBounds().width / 2, 10.f);

        if (currentTool == Tool::draw || currentTool == Tool::erase)
        {
            brushText.setString("Brush Size: " + std::to_string(brushSize) + " \t Scroll Mouse to Change");
            window.draw(brushText);
        }
        else if (currentTool == Tool::zoom)
        {
            brushText.setString("Scroll Mouse to Zoom\tHold Mouse Click to Pan\tClick Zoom Button to Reset");
            window.draw(brushText);
        }

        // draw tool buttons
        for (int i = 0; i < TOOLSIZE; i++)
        {
            toolBar[i].draw(window);
        }

        // draw tile buttons
        for (int i = 0; i < TILESIZE; i++)
        {
            tileBar[i].draw(window);
        }

        // draw layer buttons
        for (int i = 0; i < LAYERSIZE; i++)
        {
            layerBar[i].draw(window);
        }

        window.display();
    }

    return 0;
}