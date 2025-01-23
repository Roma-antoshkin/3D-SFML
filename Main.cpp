#include <SFML/Graphics.hpp>
using namespace std;

#include "Constants.h"

struct Elips {
    sf::Vector3f coord, mas, rot;
    Elips(): coord(), mas(1., 1., 1.), rot() {}
    Elips(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt): 
        coord(cd), mas(ms), rot(rt) {}
    void setInShader(sf::Shader* shader, const string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".mas", mas);
        shader->setUniform(name+".rot", rot);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Title");
    window.setFramerateLimit(60);
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
    Elips elp(
        sf::Vector3f(0., 0., 0.),
        sf::Vector3f(100., 100., 100.),
        sf::Vector3f(0., 0., 0.)
    );
    elp.setInShader(&shader, "elp");

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