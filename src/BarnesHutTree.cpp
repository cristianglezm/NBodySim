#include "BarnesHutTree.hpp"

#include <algorithm>
#include <thread>
#include <memory>
#include <mutex>

    const double BarnesHutTree::Theta = 6.5;
    BarnesHutTree::BarnesHutTree(const sf::FloatRect& b)
    : mass(0.0)
    , particles()
    , bounds(b)
    , level(0)
    , maxLevel(15000)
    , maxCapacity(1)
    , centerOfMass()
    , nodes(){
        for(auto i=0u;i<nodes.size();++i){
            nodes[i] = nullptr;
        }
    }
    void BarnesHutTree::insert(Particle* p) noexcept{
        if(isSplit()){
            int index = getNodeIndex(p);
            if(index != -1 ){
                nodes[index]->insert(p);
                return;
            }
        }
        particles.emplace_back(p);
        mass = p->getMass();
        centerOfMass = p->getPosition();
        if(particles.size() > maxCapacity && level < maxLevel){
            if(!isSplit()){
                split();
            }
            particles.erase(std::remove_if(std::begin(particles),std::end(particles),
                [&](Particle* i){
                    int index = getNodeIndex(i);
                    if(index != -1){
                        nodes[index]->insert(i);
                        mass = 0.0;
                        centerOfMass = sf::Vector2f();
                        return true;
                    }
                    return false;
                }),std::end(particles));
        }
    }
    BarnesHutTree::container BarnesHutTree::retrieve(Particle* p) noexcept{
        container pList;
        retrieve(p,pList);
        return pList;
    }
    void BarnesHutTree::retrieve(Particle* p,container& pList) noexcept{
        container internDst;
        int index = getNodeIndex(p);
        if(index != -1 && nodes[0] != nullptr){
            nodes[index]->retrieve(p,internDst);
        }
        std::sort(std::begin(particles),std::end(particles));
        std::sort(std::begin(internDst),std::end(internDst));
        std::merge(std::begin(particles),std::end(particles), 
                    std::begin(internDst),std::end(internDst),
                    std::back_inserter(pList));
    }
    bool BarnesHutTree::isSplit() const noexcept{
        return (nodes[0] != nullptr);
    }
    void BarnesHutTree::clear() noexcept{
        particles.clear();
        if(isSplit()){
            for(auto i=0u;i<nodes.size();++i) {
                nodes[i]->clear();
                nodes[i] = nullptr;
            }
        }
    }
    void BarnesHutTree::setMaxCapacity(std::size_t maxCap){
        maxCapacity = maxCap;
        if(isSplit()){
            for(auto i=0u;i<nodes.size();++i){
                nodes[i]->setMaxCapacity(maxCapacity);
            }
        }
    }
    const std::size_t& BarnesHutTree::getMaxCapacity() const noexcept{
        return maxCapacity;
    }
    void BarnesHutTree::setMaxLevel(std::size_t maxLvl){
        maxLevel = maxLvl;
        if(isSplit()){
            for(auto i=0u;i<nodes.size();++i){
                nodes[i]->setMaxLevel(maxLevel);
            }
        }
    }
    const std::size_t& BarnesHutTree::getMaxLevel() const noexcept{
        return maxLevel;
    }
    sf::Vector2f BarnesHutTree::calcForce(Particle* p, double G) noexcept{
        sf::Vector2f f;
        if(particles.size() == 1){
            if(p!=particles[0]){
                auto dist = distance(*p,*particles[0]);
                if(dist > 0){
                    auto dx = particles[0]->getPosition().x - p->getPosition().x;
                    auto dy = particles[0]->getPosition().y - p->getPosition().y;
                    auto fnet = force(*p,*particles[0],G);
                    f = p->getForce();
                    f.x += fnet * dx / dist;
                    f.y += fnet * dy / dist;
                }
            }
        }else if(isSplit()){
            auto tmp = Particle(centerOfMass,mass);
            auto r = distance(*p,tmp);
            auto d = bounds.size.y;
            if((d/r) <= Theta){
                    auto dx = centerOfMass.x - p->getPosition().x;
                    auto dy = centerOfMass.y - p->getPosition().y;
                    auto fnet = force(*p,tmp,G);
                    f = p->getForce();
                    f.x += fnet * dx / r;
                    f.y += fnet * dy / r;
            }else{
                f = p->getForce();
                for(auto i=0u;i<nodes.size();++i){
                    f += nodes[i]->calcForce(p,G);
                }
            }
        }
        return f;
    }
    void BarnesHutTree::parallelComputeMassDistribution() noexcept{
        std::vector<std::thread> threads;
        centerOfMass = sf::Vector2f();
        mass = 0.0;
        if(isSplit()){
            auto size = 3u;
            for(auto i=0u;i<size;++i){
                threads.emplace_back([=](std::array<std::unique_ptr<BarnesHutTree>,4>& nodes){
                    nodes[i]->computeMassDistribution();
                },std::ref(nodes));
            }
            nodes[3]->computeMassDistribution();
            for(auto i=0u;i<size;++i){
                threads[i].join();
            }
            for(const auto& n:nodes){
                mass += n->getMass();
                centerOfMass += n->getCenterOfMass() * static_cast<float>(n->getMass());
            }
            centerOfMass /= static_cast<float>(mass);
        }
    }
    void BarnesHutTree::computeMassDistribution() noexcept{
        centerOfMass = sf::Vector2f();
        mass = 0.0;
        if(particles.size() == 1){
            centerOfMass = particles[0]->getPosition();
            mass = particles[0]->getMass();
        }else if(isSplit()){
            for(auto i=0u;i<nodes.size();++i){
                nodes[i]->computeMassDistribution();
                mass += nodes[i]->getMass();
                centerOfMass += nodes[i]->getCenterOfMass() * static_cast<float>(nodes[i]->getMass());
            }
            centerOfMass /= static_cast<float>(mass);
        }
    }
    void BarnesHutTree::setBounds(sf::FloatRect b) noexcept{
        bounds = b;
        if(isSplit()){
            float subWidth = bounds.size.x / 2.f;
            float subHeight = bounds.size.y / 2.f;
            float x = bounds.position.x;
            float y = bounds.position.y;
            nodes[0]->setBounds({{x + subWidth, y}, {subWidth, subHeight}});
            nodes[1]->setBounds({{x, y}, {subWidth, subHeight}});
            nodes[2]->setBounds({{x, y + subHeight}, {subWidth, subHeight}});
            nodes[3]->setBounds({{x + subWidth, y + subHeight}, {subWidth, subHeight}});
        }
    }
    void BarnesHutTree::render(sf::RenderWindow& win) const noexcept{
        sf::RectangleShape boundsShape(bounds.size);
        boundsShape.setPosition(bounds.position);
        int b,g,r;
        b = level > 255 ? 255: level+mass;
        g = level > 255 ? 255: level+ static_cast<int>(bounds.position.y);
        r = level > 255 ? 255: level+( static_cast<int>(bounds.position.y)+ static_cast<int>(bounds.size.y));
        if(b> 255 || b <0){b=255;}
        if(g> 255 || g <0){g=255;}
        if(r> 255 || r <0){r=255;}
        sf::Color color(r,b,g,255);
        boundsShape.setOutlineThickness(0.5);
        boundsShape.setFillColor(sf::Color::Transparent);
        boundsShape.setOutlineColor(color);
        win.draw(boundsShape);
        if(isSplit()){
            for(auto i=0u;i<nodes.size();++i){
                nodes[i]->render(win);
            }
        }
    }
    const double& BarnesHutTree::getMass() const noexcept{
        return mass;
    }
    const sf::Vector2f& BarnesHutTree::getCenterOfMass() const noexcept{
        return centerOfMass;
    }
    BarnesHutTree::BarnesHutTree(std::size_t lvl, const sf::FloatRect& b)
    : mass(0.0)
    , particles()
    , bounds(b)
    , level(lvl)
    , maxLevel(15000)
    , maxCapacity(1)
    , centerOfMass()
    , nodes(){
        for(auto i=0u;i<nodes.size();++i){
            nodes[i] = nullptr;
        }
    }
    void BarnesHutTree::split() noexcept{
        float subWidth = bounds.size.x / 2.f;
        float subHeight = bounds.size.y / 2.f;
        float x = bounds.position.x;
        float y = bounds.position.y;
        nodes[0].reset(new BarnesHutTree((level+1), {{x + subWidth, y}, {subWidth, subHeight}}));
        nodes[1].reset(new BarnesHutTree((level+1), {{x, y}, {subWidth, subHeight}}));
        nodes[2].reset(new BarnesHutTree((level+1), {{x, y + subHeight}, {subWidth, subHeight}}));
        nodes[3].reset(new BarnesHutTree((level+1), {{x + subWidth, y + subHeight}, {subWidth, subHeight}}));
        for(auto i=0u;i<nodes.size();++i){
            nodes[i]->setMaxCapacity(maxCapacity);
            nodes[i]->setMaxLevel(maxLevel);
        }
    }
    int BarnesHutTree::getNodeIndex(Particle* p) const noexcept{
        int index = -1;
        float subWidth = bounds.size.x / 2.f;
        float subHeight = bounds.size.y / 2.f;
        float x = bounds.position.x;
        float y = bounds.position.y;
        sf::FloatRect rightTop({x + subWidth,y},{subWidth,subHeight});
        sf::FloatRect leftTop({x,y},{subWidth,subHeight});
        sf::FloatRect leftBottom({x,y+subHeight},{subWidth,subHeight});
        sf::FloatRect rightBottom({x+subWidth,y+subHeight},{subWidth,subHeight});

        if(rightTop.contains(p->getPosition())){
            index = 0;
        }else if(leftTop.contains(p->getPosition())){
            index = 1;
        }else if(leftBottom.contains(p->getPosition())){
            index = 2;
        }else if(rightBottom.contains(p->getPosition())){
            index = 3;
        }
        return index;
    }
    BarnesHutTree& BarnesHutTree::operator[](const std::size_t index) noexcept{
        return *nodes[index].get();
    }
    const BarnesHutTree& BarnesHutTree::operator[](const std::size_t index) const noexcept{
        return *nodes[index].get();
    }
