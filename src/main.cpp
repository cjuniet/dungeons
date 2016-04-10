#include <SFML/Graphics.hpp>
#include <random>

namespace rng {
std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> nd(0.0, 1.0);                             // mu = 0, sigma = 1
std::uniform_real_distribution<> ud(-1.0, std::nextafter(1.0, 2.0)); // [-1, 1]
double normal() { return nd(gen); }
double uniform() { return ud(gen); }
} // namespace rng

////////////////////////////////////////////////////////////////

constexpr int fast_floor(const float x) { return x > 0 ? (int)x : (int)x - 1; }

sf::Vector2f align(const sf::Vector2f& v, const float step = 8.0f)
{
  // return sf::Vector2f(fast_floor((v.x + step - 1) / step) * step, fast_floor((v.y + step - 1) / step) * step);
  return sf::Vector2f(std::round((v.x  + step - 1) / step) * step, std::round((v.y + step - 1) / step) * step);
}

sf::Vector2f getPointInCircle(float a, float b)
{
  static const double pi = 3.14159265358979323846264338327950288;
  const float theta = static_cast<float>(2.0 * pi * rng::normal());
  const float rho = static_cast<float>(rng::uniform());
  return sf::Vector2f((float)(a * rho * cos(theta)), (float)(b * rho * sin(theta)));
}

sf::Rect<float> getAlignedBounds(const sf::RectangleShape& rs)
{
  auto offset = 1;
  auto bounds = rs.getGlobalBounds();
  auto aabb = align({bounds.left - offset, bounds.top - offset});
  bounds.left = aabb.x;
  bounds.top = aabb.y;
  bounds.width += offset + offset;
  bounds.height += offset + offset;
  return bounds;
}

int main(int argc, char* argv[])
{
  sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
  const auto ratio = (float)desktop.height / desktop.width;

  sf::RenderWindow window(desktop, "Dungeons", sf::Style::Fullscreen);
  window.setVerticalSyncEnabled(true);
  window.setMouseCursorVisible(false);
  window.setView(sf::View(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(desktop.width, desktop.height)));

  const int NB_ROOMS = 100;
  std::vector<sf::RectangleShape> rooms;
  std::normal_distribution<> width_dist(60, 20);
  std::normal_distribution<> height_dist(60 * ratio, 20 * ratio);
  std::uniform_int_distribution<> color_dist(0, 0xffffff);
  bool marked = false;     //
  bool all_spread = false; // TODO: FSM?
  bool done = false;       //
  float width_mean = 0;
  float height_mean = 0;
  float speed = 1.0f;

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
      auto origin = align(getPointInCircle(desktop.width / 6.0f, desktop.height / 6.0f));
      auto size = align({(float)width_dist(rng::gen), (float)height_dist(rng::gen)});
      if (size.x < 8) size.x = 8;
      if (size.y < 8) size.y = 8;
      width_mean += size.x / NB_ROOMS;
      height_mean += size.y / NB_ROOMS;

      sf::RectangleShape room;
      room.setSize(size);
      room.setPosition(origin);
      room.setFillColor(sf::Color(color_dist(rng::gen) << 8 | 0xff));
      room.setOutlineColor(sf::Color::White);
      room.setOutlineThickness(1.0f);
      rooms.push_back(room);
    } else if (!marked) {
      for (auto& lhs : rooms) {
        for (const auto& rhs : rooms) {
          if (&lhs == &rhs) continue;
          if (lhs.getGlobalBounds().intersects(rhs.getGlobalBounds())) {
            lhs.setFillColor(rhs.getFillColor());
          }
        }
      }
      marked = true;
    } else if (!all_spread) {
      all_spread = true;
      for (auto& lhs : rooms) {
        auto bounds = getAlignedBounds(lhs);
        for (auto& rhs : rooms) {
          if (&lhs == &rhs) continue;
          auto v = rhs.getPosition() - lhs.getPosition();
          v /= (1.0f + (float)sqrt(v.x * v.x + v.y * v.y));
          if (bounds.intersects(rhs.getGlobalBounds())) {
            lhs.move(-speed * v.x, -speed * v.y);
            rhs.move(+speed * v.x, +speed * v.y);
            bounds = getAlignedBounds(lhs);
            all_spread = false;
          }
        }
      }
    } else if (!done) {
      width_mean *= 1.1f;
      height_mean *= 1.1f;
      sf::Color grey(0x808080ff);
      sf::Color navy(0x000080ff);
      for (auto& room : rooms) {
        room.setPosition(align(room.getPosition()));
        room.setOutlineColor(grey);
        if (room.getSize().x >= width_mean && room.getSize().y >= height_mean) {
          room.setFillColor(sf::Color::Red);
        } else {
          room.setFillColor(navy);
        }
      }
      done = true;
    }

    window.clear();
    for (auto& room : rooms) window.draw(room);
    window.display();
  }

  return 0;
}
