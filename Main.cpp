#include <SFML/Graphics.hpp>
#include <cmath>
#include "3DObjekts.h"
using namespace std;

#include "Constants.h"

int main() {
    sf::Vector3f camPos(0., 0., 0.);
    float mouseX = 0;
	float mouseY = 0;
	float sens = 2.f/1000;
	float speed = 300.f;
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

    sf::Texture skybox;
    skybox.loadFromFile("img/skybox.jpg");
    shader.setUniform("skybox", skybox);
    Elips elp(
        sf::Vector3f(0., 0., 0.),
        sf::Vector3f(80., 150., 100.),
        sf::Vector3f(0., 0., 0.),
        sf::Glsl::Vec4(1., 0.5, 0., 1.)
    );
    elp.bindShader(&shader);
    Box bx(
        sf::Vector3f(0., 150., 200.),
        sf::Vector3f(60., 100., 80.),
        sf::Vector3f(0., 0., 0.),
        sf::Glsl::Vec4(0., 0.75, 1., 1.)
    );
    bx.bindShader(&shader);
    Camera cam(
        sf::Vector3f(350., 0., 0.),
        sf::Vector3f(0., 0., 0.)
    );
    cam.bindShader(&shader);
    Plane plane(
        sf::Vector3f(0., 0., -200),
        sf::Vector3f(0., 0., 1.),
        sf::Glsl::Vec4(.5, .5, .5, 1.)
    );
    plane.bindShader(&shader);
    plane.setInShader("pln");
    // Light light(
    //     sf::Vector3f(-1., 1., 1.),
    //     sf::Glsl::Vec4(1., 1., 1., 1.)
    // );
    // light.bindShader(&shader);
    float curTime = clock.getElapsedTime().asSeconds();
    while (window.isOpen()) {
        sf::Event event;
        float time = clock.getElapsedTime().asSeconds();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseMoved) {
				if (mouseHidden) {
					int mx = event.mouseMove.x - WIDTH / 2;
					int my = event.mouseMove.y - HEIGHT / 2;
					mouseX -= mx*sens;
					mouseY -= my*sens;
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

            sf::Vector3f dir = sf::Vector3f(0.0f, 0.0f, 0.0f);
			sf::Vector3f dirTemp;
			if (wasdUD[0]) dir = sf::Vector3f(-1.0f, 0.0f, 0.0f);
			else if (wasdUD[2]) dir = sf::Vector3f(1.0f, 0.0f, 0.0f);
			if (wasdUD[1]) dir += sf::Vector3f(0.0f, -1.0f, 0.0f);
			else if (wasdUD[3]) dir += sf::Vector3f(0.0f, 1.0f, 0.0f);
            if (wasdUD[4]) dir += sf::Vector3f(0.0f, 0.0f, 1.0f);
			else if (wasdUD[5]) dir += sf::Vector3f(0.0f, 0.0f, -1.0f);

            dirTemp.x = dir.x*cos(mouseX) - dir.y*sin(mouseX);
            dirTemp.y = dir.x*sin(mouseX) + dir.y*cos(mouseX);
            dirTemp.z = dir.z;
            cam.coord += dirTemp*speed*(time - curTime);
        }

        curTime = time;
        
        shader.setUniform("u_time", time);

        elp.setRot(sf::Vector3f(-time, 0., -time));
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