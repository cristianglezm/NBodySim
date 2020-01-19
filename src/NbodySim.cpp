#include "NbodySim.hpp"
#include <iostream>

NbodySim::NbodySim(std::size_t numParticles)
: mass(100.0)
, fastGen(false)
, renderQuadtree(false)
, blackHole(true)
, win(sf::VideoMode(1280,720), "NBodySim", sf::Style::Close)
, bounds(0,0,1280,720)
, bht(bounds)
, particles()
, diceForX(0, bounds.width)
, diceForY(0, bounds.height)
, diceForMass(1,20)
, FPSText()
, sizeText()
, FPSUpdateTime(sf::Time::Zero)
, FPSNumFrames(0)
, FPSFont()
, massText(){
    bht.setMaxCapacity(1);
    bht.setMaxLevel(100000);
    particles.reserve(numParticles);
    for(auto i=0u;i<numParticles;++i){
        particles.emplace_back(sf::Vector2f(diceForX(engine),diceForY(engine)),diceForMass(engine));
    }
    FPSFont.loadFromFile("data/Outwrite.ttf");
    FPSText.setFont(FPSFont);
    FPSText.setPosition(bounds.width-55,bounds.height-45);
    FPSText.setCharacterSize(25u);
    massText.setFont(FPSFont);
    sizeText.setFont(FPSFont);
    sizeText.setPosition(bounds.width - 255, 10);
    sizeText.setCharacterSize(25u);
    sizeText.setString("Particles: " + std::to_string(numParticles));
    massText.setPosition(15,bounds.height-45);
    massText.setCharacterSize(25u);
    massText.setString("Mass: " + std::to_string(mass));
}
void NbodySim::run() noexcept{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while(win.isOpen()){
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;
        while(timeSinceLastUpdate > TimePerFrame){
            timeSinceLastUpdate -= TimePerFrame;
            sf::Event event;
            while(win.pollEvent(event)){
                handleInput(event);
            }
            update(TimePerFrame);
        }
        updateFPS(dt);
        render();
    }
}
void NbodySim::handleInput(sf::Event& e) noexcept{
    switch(e.type){
        case sf::Event::Closed:
                win.close();
            break;
        case sf::Event::KeyReleased:
            if(e.key.code == sf::Keyboard::T){
                renderQuadtree = !renderQuadtree;
            }
            if(e.key.code == sf::Keyboard::Left){
                auto view = win.getView();
                view.move(-100,0);
                win.setView(view);
                bounds = updateBounds(view);
                bht.setBounds(bounds);
            }
            if(e.key.code == sf::Keyboard::Right){
                auto view = win.getView();
                view.move(100,0);
                win.setView(view);
                bounds = updateBounds(view);
                bht.setBounds(bounds);
            }
            if(e.key.code == sf::Keyboard::Down){
                auto view = win.getView();
                view.move(0,100);
                win.setView(view);
                bounds = updateBounds(view);
                bht.setBounds(bounds);
            }
            if(e.key.code == sf::Keyboard::Up){
                auto view = win.getView();
                view.move(0,-100);
                win.setView(view);
                bounds = updateBounds(view);
                bht.setBounds(bounds);
            }
            if(e.key.code == sf::Keyboard::F){
                fastGen = !fastGen;
            }
            if(e.key.code == sf::Keyboard::B){
                blackHole = !blackHole;
            }
            if(e.key.code == sf::Keyboard::Subtract){
                auto view = win.getView();
                view.zoom(1.1);
                win.setView(view);
                bounds = updateBounds(view);
                bht.setBounds(bounds);
            }
            if(e.key.code == sf::Keyboard::Add){
                auto view = win.getView();
                view.zoom(0.9);
                win.setView(view);
                bounds = updateBounds(view);
                bht.setBounds(bounds);
            }
            if(e.key.code == sf::Keyboard::S){
                for(auto i=0u;i<50;++i){
                    particles.emplace_back(sf::Vector2f(diceForX(engine),diceForY(engine)),diceForMass(engine));
                }
            }
            break;
        case sf::Event::KeyPressed:
            if(e.key.code == sf::Keyboard::LShift){
                mass += 0.5;
                massText.setString("mass: " + std::to_string(mass));
            }
            if(e.key.code == sf::Keyboard::LControl){
                if(mass > 0.5){
                    mass -= 0.5;
                }else{
                    mass = 0.5;
                }
                massText.setString("mass: " + std::to_string(mass));
            }
            break;
        case sf::Event::MouseButtonReleased:
            if(e.mouseButton.button == sf::Mouse::Button::Right){
                particles.clear();
            }
            if((e.mouseButton.button == sf::Mouse::Button::Left) && !fastGen){
                sf::Vector2f pos = win.mapPixelToCoords(sf::Mouse::getPosition(win));
                particles.emplace_back(pos,mass);
            }
            break;
        default:
            break;
    }
}
sf::FloatRect NbodySim::updateBounds(const sf::View& view) noexcept{
    sf::FloatRect b;
    b.left = view.getCenter().x - (view.getSize().x / 2.0);
    b.top = view.getCenter().y - (view.getSize().y / 2.0);
    b.width = view.getSize().x * view.getViewport().width;
    b.height = view.getSize().y * view.getViewport().height;
    std::cout << "[" << b.left << ", " << b.top << "]-[" << b.width << ", " << b.height << "]" << std::endl;
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
            return (!bounds.contains(p.getPosition().x,p.getPosition().y)) || (p.getMass() == 0.0);
    }),std::end(particles));
    sizeText.setString("Particles: " + std::to_string(particles.size()));
    bht.clear();
    for(auto& p:particles){
        if(bounds.contains(p.getPosition().x, p.getPosition().y)){
            bht.insert(&p);
        }
    }
    bht.parallelComputeMassDistribution();
    //bht.computeMassDistribution();
    if(blackHole && !particles.empty()){
        //particles[0].setPosition(bounds.width / 2, bounds.height /2);
        particles[0].setColor(sf::Color::Red);
        particles[0].setMass(5000000);
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
    ///*
    std::vector<sf::Vertex> va;
    va.reserve(particles.size());
    for(auto& p:particles){
        va.emplace_back(p.getPosition(),p.color);
    }
    win.draw(va.data(), va.size(),sf::Points);
    //*/
    //win.draw(particles.data(), particles.size(), sf::Points);
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
