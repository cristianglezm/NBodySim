////////////////////////////////////////////////////////////
// Copyright 2016 Cristian Glez <Cristian.glez.m@gmail.com>
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

#ifndef NBODYSIM_HPP
#define NBODYSIM_HPP

#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <random>

#include "Particle.hpp"
#include "BarnesHutTree.hpp"

class NbodySim{
    public:
        NbodySim(std::size_t numParticles);
        void run() noexcept;
        void handleInput(sf::Event& e) noexcept;
        void update(sf::Time dt) noexcept;
        void updateFPS(sf::Time dt) noexcept;
        void render() noexcept;
        sf::FloatRect updateBounds(const sf::View& view) noexcept;
        ~NbodySim() = default;
    private:
        float mass;
        bool fastGen;
        bool renderQuadtree;
        bool blackHole;
        sf::RenderWindow win;
        sf::FloatRect bounds;
        BarnesHutTree bht;
        std::vector<Particle> particles;
        static std::mt19937 engine;
        std::uniform_real_distribution<> diceForX;
        std::uniform_real_distribution<> diceForY;
        std::uniform_real_distribution<> diceForMass;
        static const sf::Time TimePerFrame;
        sf::Text FPSText;
        sf::Time FPSUpdateTime;
        std::size_t	FPSNumFrames;
        sf::Font FPSFont;
};

#endif // NBODYSIM_HPP
