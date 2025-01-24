#include <SFML/Graphics.hpp>
#include <cmath>
using namespace std;

#include "Constants.h"

sf::Transform rotateMatrix(sf::Vector3f r) {
    return sf::Transform(
        1.f, 0.f, 0.f, 
        0.f, cos(r.x), -sin(r.x),
        0.f, sin(r.x), cos(r.x)
    ) * sf::Transform(
        cos(r.y), 0.f, sin(r.y),
        0.f, 1.f, 0.f,
        -sin(r.y), 0, cos(r.y)
    ) * sf::Transform(
        cos(r.z), -sin(r.z), 0.f,
        sin(r.z), cos(r.z), 0.f,
        0.f, 0.f, 1.f
    );
}

sf::Transform unRotateMatrix(sf::Vector3f r) {
    return sf::Transform(
        cos(r.z), -sin(r.z), 0.f,
        sin(r.z), cos(r.z), 0.f,
        0.f, 0.f, 1.f
    ) * sf::Transform(
        cos(r.y), 0.f, sin(r.y),
        0.f, 1.f, 0.f,
        -sin(r.y), 0, cos(r.y)
    ) * sf::Transform(
        1.f, 0.f, 0.f, 
        0.f, cos(r.x), -sin(r.x),
        0.f, sin(r.x), cos(r.x)
    );
}

struct Elips {
private:
    sf::Vector3f rotvec;
    sf::Transform rot, unrot;
public:
    sf::Vector3f coord, mas;

    Elips(): coord(), mas(1., 1., 1.), rotvec(), rot() {}
    Elips(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :coord(cd), mas(ms), rotvec(rt) {
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }

    sf::Vector3f getRot() { return rotvec; }
    void setRot(sf::Vector3f rt) {
        rotvec = rt;
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }
    

    void setInShader(sf::Shader* shader, const string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".mas", mas);
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
    }
};

struct Box {
private:
    sf::Vector3f rotvec;
    sf::Transform rot, unrot;
public:
    sf::Vector3f coord, mas;

    Box(): coord(), mas(1., 1., 1.), rotvec(), rot() {}
    Box(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :coord(cd), mas(ms), rotvec(rt) {
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }

    sf::Vector3f getRot() { return rotvec; }
    void setRot(sf::Vector3f rt) {
        rotvec = rt;
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }
    

    void setInShader(sf::Shader* shader, const string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".mas", mas);
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
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
        sf::Vector3f(150., 100., 80.),
        sf::Vector3f(0., 0., 0.)
    );
    Box bx(
        sf::Vector3f(150., 100., 100.),
        sf::Vector3f(100., 80., 60.),
        sf::Vector3f(0., 0., 0.)
    );
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        float time = clock.getElapsedTime().asSeconds();
        shader.setUniform("u_time", time);

        elp.setRot(sf::Vector3f(time, time, 0.));
        elp.setInShader(&shader, "elp");

        bx.setRot(sf::Vector3f(time, time, 0.));
        bx.setInShader(&shader, "bx");


        window.clear(sf::Color(0, 100, 0, 255));
        window.draw(sprite, &shader);
        window.display();
    }

    return 0;
}