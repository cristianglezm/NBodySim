#ifndef BARNES_HUT_TREE_HPP
#define BARNES_HUT_TREE_HPP

#include <vector>
#include <array>
#include <memory>
#include "Particle.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class BarnesHutTree{
    public:
        using container = std::vector<Particle*>;
    public:
        BarnesHutTree(const sf::FloatRect& b);
        void insert(Particle* p) noexcept;
        container retrieve(Particle* p) noexcept;
        void retrieve(Particle* p,container& pList) noexcept;
        bool isSplit() const noexcept;
        void clear() noexcept;
        void setMaxCapacity(std::size_t maxCap);
        inline const std::size_t& getMaxCapacity() const noexcept;
        void setMaxLevel(std::size_t maxLvl);
        inline const std::size_t& getMaxLevel() const noexcept;
        sf::Vector2f calcForce(Particle* p, double G = 6.67e-11) noexcept;
        void parallelComputeMassDistribution() noexcept;
        void computeMassDistribution() noexcept;
        void setBounds(sf::FloatRect b) noexcept;
        void render(sf::RenderWindow& win) const noexcept;
        const double& getMass() const noexcept;
        const sf::Vector2f& getCenterOfMass() const noexcept;
        BarnesHutTree& operator[](const std::size_t index) noexcept;
        const BarnesHutTree& operator[](const std::size_t index) const noexcept;
        ~BarnesHutTree() = default;
    private:
        BarnesHutTree(std::size_t lvl, const sf::FloatRect& b);
        void split() noexcept;
        int getNodeIndex(Particle* p) const noexcept;
    private:
        double mass;
        container particles;
        sf::FloatRect bounds;
        std::size_t level;
        std::size_t maxLevel;
        std::size_t maxCapacity;
        sf::Vector2f centerOfMass;
        std::array<std::unique_ptr<BarnesHutTree>,4> nodes;
        static const double Theta;
};

#endif // BARNES_HUT_TREE_HPP
