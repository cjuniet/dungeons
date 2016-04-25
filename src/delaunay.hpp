#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class DelaunayGraph : public sf::Drawable
{
public:
  typedef sf::Vector2f Point;
  struct Circle { Point center; float radius; };
  struct Triangle { Point a, b, c; };

public:
  DelaunayGraph(float width, float height);

  void add_vertex(const Point& point);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
  Circle get_circumcircle(const Triangle& triangle) const;

  std::vector<Point> vertices;
  std::vector<Triangle> triangles;
  std::vector<std::pair<int, std::vector<int>>> adjlist;
};
