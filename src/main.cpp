#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

const float32 timeStep = 1.0f / 60.0f;

namespace random {
std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> nd(0.0, 1.0); // mu = 0, sigma = 1
std::uniform_real_distribution<> ud(-1.0, std::nextafter(1.0, 2.0)); // [-1, 1]
double normal() { return nd(gen); }
double uniform() { return ud(gen); }
} // namespace random

////////////////////////////////////////////////////////////////

sf::Vector2f getPointInCircle(float a, float b)
{
  static const double pi = 3.14159265358979323846;
  const float theta = static_cast<float>(2.0 * pi * random::normal());
  const float rho = static_cast<float>(random::uniform());
  return sf::Vector2f(a * rho * cos(theta), b * rho * sin(theta));
}

sf::Vector2f align(const sf::Vector2f& v, float step = 8.0f)
{
  return sf::Vector2f((int)((v.x + step - 1) / step) * step, (int)((v.y + step - 1) / step) * step);
}

int main(int argc, char* argv[])
{
  sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
  const auto ratio = (float)desktop.height / desktop.width;

  sf::RenderWindow window(desktop, "Dungeons", sf::Style::Default);
  window.setVerticalSyncEnabled(true);
  window.setMouseCursorVisible(false);
  window.setView(sf::View(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(desktop.width, desktop.height)));

  b2Vec2 gravity(0.0f, 0.0f);
  b2World world(gravity);

  int32 velocityIterations = 6;
  int32 positionIterations = 2;

  const int NB_ROOMS = 100;
  std::vector<std::pair<sf::RectangleShape, b2Body*>> rooms;
  std::normal_distribution<> width_dist(50, 10);
  std::normal_distribution<> height_dist(50 * ratio, 10 * ratio);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape) {
          window.close();
        }
      }
    }

    if (rooms.size() < NB_ROOMS) {
      auto origin = align(getPointInCircle(300, 100));
      auto size = align({
        (float)width_dist(random::gen),
        (float)height_dist(random::gen)
      });

      sf::RectangleShape room;
      room.setSize(size);
      room.setPosition(origin);
      room.setFillColor(sf::Color::Blue);
      room.setOutlineColor(sf::Color::White);
      room.setOutlineThickness(1.0f);

      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position.Set(origin.x, origin.y);
      bodyDef.fixedRotation = true;
      b2Body* body = world.CreateBody(&bodyDef);

      b2PolygonShape dynamicBox;
      dynamicBox.SetAsBox(size.x/2, size.y/2);
      b2FixtureDef fixtureDef;
      fixtureDef.shape = &dynamicBox;
      body->CreateFixture(&fixtureDef);

      rooms.push_back({ room, body });
    }

    world.Step(timeStep, velocityIterations, positionIterations);
    world.ClearForces();

    window.clear();
    for (auto& p : rooms) {
      auto pos = align({ p.second->GetPosition().x, p.second->GetPosition().y });
      p.first.setPosition(pos);
      window.draw(p.first);
    }
    window.display();
  }

  return 0;
}
