#include <vector>

#include "types.hpp"
#include "body.hpp"
#include "model.hpp"


using namespace model;


Model::Model(double deltaT) {
    this->deltaT = deltaT;
    simTime = 0.0;
}

bool Model::addBody(body::Body newBody) {
    bodies.push_back(newBody);
    return true;
}

body::Body Model::getBodyByName(std::string toFind) {
    for (body::Body current : bodies) {
        if (current.getName() == toFind) {
            return current;
        }
    }
    return body::Body(0, {0, 0, 0});
}

double Model::iterate() {
    for (body::Body & body : bodies) {
        updateBody(body);
    }
    simTime = simTime + deltaT;
    return simTime;
}

void Model::updateBody(body::Body &body) {
    // Calculate the new position, and then do the the update?
    // This means the new position has to be calculated before force
    force totalForce = {0, 0, 0};
    //position curPos = body.getPos();
    //position newPos = body.step(deltaT);
    

    for (body::Body &other : bodies) {
        if (body.getName() != other.getName()) {
            totalForce = plu(totalForce, body.getAttraction(other));
        }
    }
    body.update(deltaT, totalForce);
}

