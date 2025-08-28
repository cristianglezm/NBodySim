#include "Particle.hpp"

Particle::Particle()
: sf::Vertex{{0.f,0.f}, sf::Color::White}
, mass(15)
, active(true)
, force(0.0,0.0)
, velocity(0.0,0.0){}
Particle::Particle(const sf::Vector2f& position, float mass)
: sf::Vertex{position, sf::Color::White}
, mass(mass)
, active(true)
, force(0.0,0.0)
, velocity(0.0,0.0){}
Particle& Particle::setActive(bool isActive) noexcept{
    active = isActive;
    return *this;
}
Particle& Particle::addMass(double amount) noexcept{
    mass += amount;
    return *this;
}
Particle& Particle::setPosition(const sf::Vector2f& pos) noexcept{
    sf::Vertex::position = pos;
    return *this;
}
Particle& Particle::setPosition(float x, float y) noexcept{
    sf::Vertex::position = sf::Vector2f(x,y);
    return *this;
}
Particle& Particle::setColor(const sf::Color& c) noexcept{
   sf::Vertex::color = c;
   return *this;
}
Particle& Particle::setMass(double mass) noexcept{
    this->mass = mass;
    return *this;
}
Particle& Particle::setVelocity(const sf::Vector2f& vel) noexcept{
    this->velocity = vel;
    return *this;
}
Particle& Particle::setForce(const sf::Vector2f& f) noexcept{
    force = f;
    return *this;
}
Particle& Particle::move(const sf::Vector2f& amount) noexcept{
    sf::Vertex::position += amount;
    return *this;
}
bool operator!=(const Particle& lhs, const Particle& rhs){
    return (lhs.mass != rhs.mass ||
            lhs.force != rhs.force ||
            lhs.velocity != rhs.velocity ||
            lhs.position != rhs.position);
}
double distance(const Particle& p1, const Particle& p2) noexcept{
    double dx = p1.getPosition().x - p2.getPosition().x;
    double dy = p1.getPosition().y - p2.getPosition().y;
    return std::sqrt((dx * dx) + (dy * dy));
}

double force(const Particle& p1, const Particle& p2, double G) noexcept{
    auto d = distance(p1,p2);
    static const double eps = 0.001;
    //return (G * ((p1.getMass() * p2.getMass()) / (d*d) + (eps*eps)));
    return (G * p1.getMass() * p2.getMass() / (d*d) + (eps*eps));
}
bool areColliding(const Particle& p1, const Particle& p2) noexcept{
    double dx = p1.getPosition().x - p2.getPosition().x;
    double dy = p1.getPosition().y - p2.getPosition().y;
    auto mass = p1.getMass() + p2.getMass();
    return ((dx * dx) + (dy * dy)) <= (mass * mass);
}
