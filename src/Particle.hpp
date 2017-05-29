////////////////////////////////////////////////////////////
// Copyright 2014-2016 Cristian Glez <Cristian.glez.m@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
////////////////////////////////////////////////////////////

#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <string>
#include <cmath>

class Particle final{
    public:
        Particle();
        Particle(sf::Vector2f position, float mass);
        inline const bool& isActive() const noexcept{ return active; }
        Particle& setActive(bool isActive) noexcept;
        inline const sf::Vector2f& getPosition() const noexcept{ return v.position; }
        Particle& setPosition(sf::Vector2f pos) noexcept;
        Particle& setPosition(float x, float y) noexcept;
        inline const double& getMass() const noexcept{ return mass; }
        Particle& setMass(double mass) noexcept;
        Particle& addMass(double amount) noexcept;
        Particle& setVelocity(sf::Vector2f vel) noexcept;
        inline const sf::Vector2f& getVelocity() const noexcept{ return velocity; }
        Particle& setForce(sf::Vector2f f) noexcept;
        inline const sf::Vector2f& getForce() const noexcept{ return force; }
        Particle& move(sf::Vector2f amount) noexcept;
        Particle& setCircleShape(const sf::CircleShape& cs) noexcept;
        inline const sf::CircleShape& getCircleShape() const noexcept{ return cs; }
        inline sf::CircleShape& getCircleShape() noexcept{ return cs; }
        inline const sf::Vertex getVertex() const noexcept{ return v; }
        friend bool operator!=(const Particle& rhs, const Particle& lhs);
        inline const std::string toString() const{
            return  "mass: " + std::to_string(mass) + 
                    " force: " + std::to_string(force.x) + "," + std::to_string(force.y) + 
                    " Velocity: "  + std::to_string(velocity.x) + "," + std::to_string(velocity.y);
        }
        ~Particle() = default;
    private:
        double mass;
        bool active;
        sf::Vector2f force;
        sf::Vector2f velocity;
        sf::CircleShape cs;
        sf::Vertex v;
};

double distance(const Particle& p1, const Particle& p2) noexcept;

double force(const Particle& p1, const Particle& p2, double G = 6.67e-11) noexcept;

bool areColliding(const Particle& p1, const Particle& p2) noexcept;

#endif /// PARTICLE_HPP
