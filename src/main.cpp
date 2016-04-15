#include <iostream>
#include "NbodySim.hpp"

int main(int argc, char* argv[]){
    std::size_t numParticles = 55;
    if(argc > 1){
        numParticles = std::stoul(std::string(argv[1]));
    }
    NbodySim app(numParticles);
    app.run();
    return 0;
}
