#include "NbodySim.hpp"
#include <iostream>

NbodySim::NbodySim(std::size_t numParticles)
: win(sf::VideoMode({1290,720}), "NBodySim", sf::Style::Close)
, bounds({0.f, 0.f}, {1290.f, 720.f})
, particles()
, bht(bounds)
, mass(100.0)
, FPSFont("data/Outwrite.ttf")
, FPSText(FPSFont, "", 25u)
, sizeText(FPSFont, "Particles: " + std::to_string(numParticles), 25u)
, massText(FPSFont, "Mass: " + std::to_string(mass), 25u)
, m_particleVertices(sf::VertexBuffer::Usage::Stream)
, fastGen(false)
, renderQuadtree(false)
, blackHole(true)
, diceForX(0, bounds.size.x)
, diceForY(0, bounds.size.y)
, diceForMass(10,mass)
, FPSUpdateTime(sf::Time::Zero)
, FPSNumFrames(0)
{
    bht.setMaxCapacity(1);
    bht.setMaxLevel(100000);
    particles.reserve(numParticles);
    for(auto i=0u;i<numParticles;++i){
        particles.emplace_back(sf::Vector2f(diceForX(engine),diceForY(engine)),diceForMass(engine));
    }
    FPSText.setPosition({bounds.size.x-55,bounds.size.y-45});
    sizeText.setPosition({bounds.size.x - 255, 10});
    massText.setPosition({15,bounds.size.y-45});
}
void NbodySim::run() noexcept{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while(win.isOpen()){
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;
        while(timeSinceLastUpdate > TimePerFrame){
            timeSinceLastUpdate -= TimePerFrame;
            while(const auto event = win.pollEvent()){
                handleInput(*event);
            }
            update(TimePerFrame);
        }
        updateFPS(dt);
        render();
    }
}
void NbodySim::handleInput(const sf::Event& e) noexcept{
    if(e.is<sf::Event::Closed>()){
        win.close();
    }else if(const auto* keyReleased = e.getIf<sf::Event::KeyReleased>()){
        if(keyReleased->scancode == sf::Keyboard::Scancode::T){
            renderQuadtree = !renderQuadtree;
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::Left){
            auto view = win.getView();
            view.move({-100,0});
            win.setView(view);
            bounds = updateBounds(view);
            bht.setBounds(bounds);
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::Right){
            auto view = win.getView();
            view.move({100,0});
            win.setView(view);
            bounds = updateBounds(view);
            bht.setBounds(bounds);
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::Down){
            auto view = win.getView();
            view.move({0,100});
            win.setView(view);
            bounds = updateBounds(view);
            bht.setBounds(bounds);
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::Up){
            auto view = win.getView();
            view.move({0,-100});
            win.setView(view);
            bounds = updateBounds(view);
            bht.setBounds(bounds);
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::F){
            fastGen = !fastGen;
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::B){
            blackHole = !blackHole;
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::Hyphen){
            auto view = win.getView();
            view.zoom(1.1);
            win.setView(view);
            bounds = updateBounds(view);
            bht.setBounds(bounds);
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::Equal){
            auto view = win.getView();
            view.zoom(0.9);
            win.setView(view);
            bounds = updateBounds(view);
            bht.setBounds(bounds);
        }
        if(keyReleased->scancode == sf::Keyboard::Scancode::S){
            for(auto i=0u;i<50;++i){
                particles.emplace_back(sf::Vector2f(diceForX(engine),diceForY(engine)),diceForMass(engine));
            }
        }
    }else if(const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()){
        if(keyPressed->scancode == sf::Keyboard::Scancode::LShift){
            mass += 10.0;
            massText.setString("mass: " + std::to_string(mass));
        }
        if(keyPressed->scancode == sf::Keyboard::Scancode::LControl){
            mass -= 10.0;
            mass = std::max(10.0f, mass);
            massText.setString("mass: " + std::to_string(mass));
        }
    }else if(const auto* mouseButtonReleased = e.getIf<sf::Event::MouseButtonReleased>()){
        if(mouseButtonReleased->button == sf::Mouse::Button::Right){
            particles.clear();
        }
        if((mouseButtonReleased->button == sf::Mouse::Button::Left) && !fastGen){
            sf::Vector2f pos = win.mapPixelToCoords(mouseButtonReleased->position);
            particles.emplace_back(pos,mass);
        }
    }
}
sf::FloatRect NbodySim::updateBounds(const sf::View& view) noexcept{
    sf::FloatRect b;
    b.position.x = view.getCenter().x - (view.getSize().x / 2.0f);
    b.position.y = view.getCenter().y - (view.getSize().y / 2.0f);
    b.size.x = view.getSize().x;
    b.size.y = view.getSize().y;
    std::cout << "[" << b.position.x << ", " << b.position.y << "]-[" << b.size.x << ", " << b.size.y << "]" << std::endl;
    return b;
}
void NbodySim::update(sf::Time dt) noexcept{
    if(fastGen){
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            sf::Vector2f pos = win.mapPixelToCoords(sf::Mouse::getPosition(win));
            particles.emplace_back(pos,diceForMass(engine));
        }
    }
    particles.erase(std::remove_if(std::begin(particles),std::end(particles),
        [&](const Particle& p){
            return (!bounds.contains(p.getPosition())) || (p.getMass() == 0.0);
    }),std::end(particles));
    sizeText.setString("Particles: " + std::to_string(particles.size()));
    bht.clear();
    for(auto& p:particles){
        if(bounds.contains(p.getPosition())){
            bht.insert(&p);
        }
    }
    bht.parallelComputeMassDistribution();
    //bht.computeMassDistribution();
    if(blackHole && !particles.empty()){
        particles[0].setColor(sf::Color::Red);
        particles[0].setMass(mass * 100);
    }
    for(auto& p1:particles){
        p1.setForce(sf::Vector2f(0.f,0.f));
        auto force = bht.calcForce(&p1);
        sf::Vector2f acceleration;
        acceleration.x = force.x / p1.getMass();
        acceleration.y = force.y / p1.getMass();
        sf::Vector2f velocity = p1.getVelocity();
        velocity.x += acceleration.x * dt.asMicroseconds();
        velocity.y += acceleration.y * dt.asMicroseconds();
        p1.setVelocity(velocity);
        p1.move(velocity);
    }
}
void NbodySim::updateFPS(sf::Time dt) noexcept{
        FPSUpdateTime += dt;
        FPSNumFrames += 1;
        if (FPSUpdateTime >= sf::seconds(1.0f)){
            if(FPSNumFrames < 10){
                FPSText.setFillColor(sf::Color::Red);
            }else if(FPSNumFrames > 59){
                FPSText.setFillColor(sf::Color::Green);
            }else if(FPSNumFrames < 30){
                FPSText.setFillColor(sf::Color::Yellow);
            }
            FPSText.setString(std::to_string(FPSNumFrames));
            FPSUpdateTime -= sf::seconds(1.0f);
            FPSNumFrames = 0;
        }
}
void NbodySim::render() noexcept{
    win.clear(sf::Color::Black);
    if(!particles.empty()){
        if(m_particleVertices.getVertexCount() != particles.size()){
            m_particleVertices.create(particles.size());
        }
        std::vector<sf::Vertex> vertexData;
        vertexData.reserve(particles.size());
        for(const auto& p : particles){
            vertexData.push_back(p);
        }
        m_particleVertices.update(vertexData.data());
        win.draw(m_particleVertices);
    }
    if(renderQuadtree){
        bht.render(win);
    }
    auto SIMView = win.getView();
    auto GUIView = win.getDefaultView();
    win.setView(GUIView);
    win.draw(FPSText);
    win.draw(massText);
    win.draw(sizeText);
    win.setView(SIMView);
    win.display();
}
std::mt19937 NbodySim::engine(std::random_device{}());
const sf::Time NbodySim::TimePerFrame = sf::seconds(1.f/60.f);
