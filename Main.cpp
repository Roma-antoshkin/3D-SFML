#include <SFML/Graphics.hpp>
// g
#include "Constants.h"
// D
// e
// f
// A
// B
// C
int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Title");
    sf::Shader shader;
    sf::Clock clock;

    if (!shader.loadFromFile("SphereShader.frag", sf::Shader::Fragment)) {
        // error...
    }
    shader.setUniform("u_resolution", sf::Vector2(WIDTH, HEIGHT));

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);

    sf::Sprite sprite(texture);

    // sf::Texture city;
    // city.loadFromFile("image.png");
    // shader.setUniform("texture1", city);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        shader.setUniform("u_time", clock.getElapsedTime().asSeconds());

        window.clear(sf::Color(0, 100, 0, 255));
        window.draw(sprite, &shader);
        window.display();
    }

    return 0;
}