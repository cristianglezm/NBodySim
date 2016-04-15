#include "Particle.hpp"

Particle::Particle()
: mass(15)
, active(true)
, force(0.0,0.0)
, velocity(0.0,0.0)
, cs(1.0,15){}
Particle::Particle(sf::Vector2f position, float mass)
: mass(mass)
, active(true)
, force(0.0,0.0)
, velocity(0.0,0.0)
, cs(1.0,15){
    cs.setOrigin(cs.getLocalBounds().width/2,cs.getLocalBounds().height/2);
    cs.setPosition(position);
}
Particle& Particle::setActive(bool isActive) noexcept{
    active = isActive;
    return *this;
}
Particle& Particle::addMass(double amount) noexcept{
    mass += amount;
    return *this;
}
Particle& Particle::setPosition(sf::Vector2f pos) noexcept{
    cs.setPosition(pos);
    return *this;
}
Particle& Particle::setPosition(float x, float y) noexcept{
    cs.setPosition(x,y);
    return *this;
}
Particle& Particle::setMass(double mass) noexcept{
    this->mass = mass;
    //cs.setRadius(mass);
    return *this;
}
Particle& Particle::setVelocity(sf::Vector2f vel) noexcept{
    velocity = vel;
    return *this;
}
Particle& Particle::setForce(sf::Vector2f f) noexcept{
    force = f;
    return *this;
}
Particle& Particle::move(sf::Vector2f amount) noexcept{
    cs.move(amount);
    return *this;
}
Particle& Particle::setCircleShape(const sf::CircleShape& cs) noexcept{
    this->cs = cs;
    return *this;
}
bool operator!=(const Particle& rhs, const Particle& lhs){
    return (rhs.mass != lhs.mass ||
            rhs.force != lhs.force ||
            rhs.velocity != lhs.velocity ||
            rhs.cs.getPosition() != lhs.cs.getPosition());
}
double distance(const Particle& p1, const Particle& p2) noexcept{
    double dx = p1.getPosition().x - p2.getPosition().x;
    double dy = p1.getPosition().y - p2.getPosition().y;
    return std::sqrt((dx * dx) + (dy * dy));
}

double force(const Particle& p1, const Particle& p2, double G) noexcept{
    auto d = distance(p1,p2);
    static const double eps = 0.5;
    return (G * ((p1.getMass() * p2.getMass()) / (d*d) + (eps*eps)));
}
bool areColliding(const Particle& p1, const Particle& p2) noexcept{
    double dx = p1.getPosition().x - p2.getPosition().x;
    double dy = p1.getPosition().y - p2.getPosition().y;
    auto mass = p1.getMass() + p2.getMass();
    return ((dx * dx) + (dy * dy)) <= (mass * mass);
}