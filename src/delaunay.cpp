#include "delaunay.hpp"
#include <cmath>

/* Bowyer–Watson algorithm:
 *
 * subroutine triangulate
 * input : vertex list
 * output : triangle list
 *    initialize the triangle list
 *    determine the supertriangle
 *    add supertriangle vertices to the end of the vertex list
 *    add the supertriangle to the triangle list
 *    for each sample point in the vertex list
 *       initialize the edge buffer
 *       for each triangle currently in the triangle list
 *          calculate the triangle circumcircle center and radius
 *          if the point lies in the triangle circumcircle then
 *             add the three triangle edges to the edge buffer
 *             remove the triangle from the triangle list
 *          endif
 *       endfor
 *       delete all doubly specified edges from the edge buffer
 *          this leaves the edges of the enclosing polygon only
 *       add to the triangle list all triangles formed between the point
 *          and the edges of the enclosing polygon
 *    endfor
 *    remove any triangles from the triangle list that use the supertriangle vertices
 *    remove the supertriangle vertices from the vertex list
 * end
 */

DelaunayGraph::Circle DelaunayGraph::get_circumcircle(const Triangle& t) const
{
  const float a0 = t.a.x - t.b.x;
  const float a1 = t.a.y - t.b.y;
  const float c0 = t.c.x - t.b.x;
  const float c1 = t.c.y - t.b.y;
  const float det = 0.5f / (a0 * c1 - c0 * a1 + 1e-6f);

  const float asq = a0 * a0 + a1 * a1;
  const float csq = c0 * c0 + c1 * c1;
  const float ctr0 = det * (asq * c1 - csq * a1);
  const float ctr1 = det * (csq * a0 - asq * c0);
  const float rad2 = ctr0 * ctr0 + ctr1 * ctr1;

  return {{ctr0 + t.b.x, ctr1 + t.b.y}, std::sqrt(rad2)};
}

DelaunayGraph::DelaunayGraph(float width, float height)
{
  // add super-triangle
  Point a{0, +2 * height};
  Point b{-2 * width, -2 * height};
  Point c{+2 * width, -2 * height};
  vertices.push_back(a);
  vertices.push_back(b);
  vertices.push_back(c);
  triangles.push_back({a, b, c});
}

void DelaunayGraph::add_vertex(const Point& point)
{
}

void DelaunayGraph::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#if 1
  Triangle T{{-90,10},{90,-10},{40,-80}};
  auto C = get_circumcircle(T);

  sf::ConvexShape triangle;
  triangle.setPointCount(3);
  triangle.setPoint(0, T.a);
  triangle.setPoint(1, T.b);
  triangle.setPoint(2, T.c);
  triangle.setFillColor(sf::Color::Blue);
  target.draw(triangle, states);

  sf::CircleShape circle;
  circle.setPosition(C.center);
  circle.setRadius(C.radius);
  circle.setOrigin(C.radius, C.radius);
  circle.setFillColor(sf::Color::Transparent);
  circle.setOutlineColor(sf::Color::Red);
  circle.setOutlineThickness(1);
  target.draw(circle, states);
#endif
}
