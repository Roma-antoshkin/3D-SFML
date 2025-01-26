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

class Objekt3D {
protected:
    sf::Shader* shader;
    sf::Vector3f rotvec, coord;
    sf::Transform rot, unrot;
public:
    Objekt3D(): coord(), rotvec(), rot() {}
    Objekt3D(sf::Vector3f cd, sf::Vector3f rt)
        :coord(cd), rotvec(rt) {
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }

    sf::Vector3f getRot() { return rotvec; }
    void setRot(sf::Vector3f rt) {
        rotvec = rt;
        rot = rotateMatrix(rt);
        unrot = unRotateMatrix(-rt);
    }

    void bindShader(sf::Shader* shader) { this->shader = shader; }
    
    void setInShader(const string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
    }

    void coordInShader(const string name) {
        shader->setUniform(name+".coord", coord);
    }

    void rotInShader(const string name) {
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
    }
};

class SizedObjekt3D : public Objekt3D {
protected:
    sf::Vector3f mas;
public:
    SizedObjekt3D(): Objekt3D(), mas(1., 1., 1.) {}
    SizedObjekt3D(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :Objekt3D(cd, rt), mas(ms) {}
    
    void setInShader(const string name) {
        shader->setUniform(name+".coord", coord);
        shader->setUniform(name+".mas", mas);
        shader->setUniform(name+".rot", sf::Glsl::Mat3(rot));
        shader->setUniform(name+".unrot", sf::Glsl::Mat3(unrot));
    }

    void masInShader(const string name) {
        shader->setUniform(name+".mas", mas);
    }
};

class Camera : public Objekt3D {
public:
    Camera(): Objekt3D() {}
    Camera(sf::Vector3f cd, sf::Vector3f rt)
        :Objekt3D(cd, rt) {}
};

class Light {
protected:
    sf::Shader* shader;
public:
    sf::Vector3f dir;
    sf::Glsl::Vec4 col;

    Light(): dir(), col() {};
    Light(sf::Vector3f dr, sf::Glsl::Vec4 cl): dir(dr/sqrt(dr.x*dr.x + dr.y*dr.y + dr.z*dr.z)), col(cl) {}

    void bindShader(sf::Shader* shader) { this->shader = shader; }

    void setInShader(const string name) {
        shader->setUniform(name+".dir", dir);
        shader->setUniform(name+".col", col);
    }

    void dirInShader(const string name) {
        shader->setUniform(name+".dir", dir);
    }

    void colInShader(const string name) {
        shader->setUniform(name+".col", col);
    }
};

class Elips : public SizedObjekt3D {
public:
    Elips(): SizedObjekt3D() {}
    Elips(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :SizedObjekt3D(cd, ms, rt) {}
};

class Box: public SizedObjekt3D {
public:
    Box(): SizedObjekt3D() {}
    Box(sf::Vector3f cd, sf::Vector3f ms, sf::Vector3f rt)
        :SizedObjekt3D(cd, ms, rt) {}
};

int main() {
    sf::Vector3f camPos(0., 0., 0.);
    float mouseX = 0;
	float mouseY = 0;
	float sens = 3.f/1000;
	float speed = 0.1f;
	bool mouseHidden = false;
    bool wasdUD[6] = { false, false, false, false, false, false };

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Title");
    window.setFramerateLimit(60);
    // window.setMouseCursorVisible(false);

    sf::Shader shader;
    sf::Clock clock;

    if (!shader.loadFromFile("SphereShader.frag", sf::Shader::Fragment)) {
        // error...
    }
    shader.setUniform("u_resolution", sf::Vector2f(WIDTH, HEIGHT));

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);

    sf::Sprite sprite(texture);

    // sf::Texture city;
    // city.loadFromFile("image.png");
    // shader.setUniform("texture1", city);
    Elips elp(
        sf::Vector3f(0., 0., 0.),
        sf::Vector3f(80., 150., 100.),
        sf::Vector3f(0., 0., 0.)
    );
    elp.bindShader(&shader);
    Box bx(
        sf::Vector3f(100., 150., 100.),
        sf::Vector3f(60., 100., 80.),
        sf::Vector3f(0., 0., 0.)
    );
    bx.bindShader(&shader);
    Camera cam(
        sf::Vector3f(-350., 0., 0.),
        sf::Vector3f(0., 0., 0.)
    );
    cam.bindShader(&shader);
    // Light light(
    //     sf::Vector3f(-1., 1., 1.),
    //     sf::Glsl::Vec4(1., 1., 1., 1.)
    // );
    // light.bindShader(&shader);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseMoved) {
				if (mouseHidden) {
					int mx = event.mouseMove.x - WIDTH / 2;
					int my = event.mouseMove.y - HEIGHT / 2;
					mouseX += mx*sens;
					mouseY += my*sens;
                    mouseY = max(min(mouseY, 3.14f/2), -3.14f/2);
					// sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);
				}
			}
            else if (event.type == sf::Event::MouseButtonPressed) {
				window.setMouseCursorVisible(false);
				mouseHidden = true;
			}
            else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.setMouseCursorVisible(true);
					mouseHidden = false;
				}
                else if (event.key.code == sf::Keyboard::W) wasdUD[0] = true;
				else if (event.key.code == sf::Keyboard::A) wasdUD[1] = true;
				else if (event.key.code == sf::Keyboard::S) wasdUD[2] = true;
				else if (event.key.code == sf::Keyboard::D) wasdUD[3] = true;
				else if (event.key.code == sf::Keyboard::Space) wasdUD[4] = true;
				else if (event.key.code == sf::Keyboard::LShift) wasdUD[5] = true;
			}
            else if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::W) wasdUD[0] = false;
				else if (event.key.code == sf::Keyboard::A) wasdUD[1] = false;
				else if (event.key.code == sf::Keyboard::S) wasdUD[2] = false;
				else if (event.key.code == sf::Keyboard::D) wasdUD[3] = false;
				else if (event.key.code == sf::Keyboard::Space) wasdUD[4] = false;
				else if (event.key.code == sf::Keyboard::LShift) wasdUD[5] = false;
			}
        }

        if (mouseHidden) {
            sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);
        }
        
        float time = clock.getElapsedTime().asSeconds();
        shader.setUniform("u_time", time);

        elp.setRot(sf::Vector3f(0., time, time));
        elp.setInShader("elp");

        bx.setRot(sf::Vector3f(0., time, time));
        bx.setInShader("bx");

        cam.setRot(sf::Vector3f(0., mouseY, mouseX));
        cam.setInShader("cam");

        window.clear(sf::Color(0, 100, 0, 255));
        window.draw(sprite, &shader);
        window.display();
    }

    return 0;
}