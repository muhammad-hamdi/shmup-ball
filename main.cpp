#include<SFML/Graphics.hpp>
#include<iostream>

// Handle interrupt events
void handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

int main()
{
    // Create Window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

    // Create a circle shape
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::White);
    sf::Vector2<float> position((window.getSize().x/2)-50.f, (window.getSize().y/2)-50.f);
    shape.setPosition(position);

    // Set fps to 60
    window.setFramerateLimit(60);

    // init clock for deltaTime
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        // Calculate deltaTime
        float deltaTime = deltaClock.restart().asSeconds();

        // events
        handleEvents(window);

        // Clear the canvas
        window.clear();
        
        // Do the drawing here
        window.draw(shape);

        // Display the frame
        window.display();
    }

    return 0;
}