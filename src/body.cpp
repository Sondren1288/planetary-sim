#include <cmath>
#include <string>
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"


using namespace body;


Body::Body(double mass, position pos) {
    // Stationary object with position and mass
    this->mass = mass;
    this->pos = pos;
}

Body::Body(double mass, position pos, velocity v) {
    // Velocity is constant (for now)
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
}

Body::Body(double mass, position pos, velocity v, acceleration acc) {
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
    this->acc = acc;
}


force Body::getAttraction(Body &other) {
    // Get attraction between this body and another
    double otherMass = other.mass;
        
    // Get directional vector
    position otherPos = other.getPos();
    force direction = {
        otherPos.x - pos.x, 
        otherPos.y - pos.y, 
        otherPos.z - pos.z
    };
    // The distance is the length of the vector
    double distance = length(direction);
    // Normalizing the vector means it is easier to calculate, following
    // https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
    direction = normalize(direction);
    double undirectedForce = GRAVITATIONAL * otherMass * mass / pow(distance, 2.0);
    return mul(direction, undirectedForce);
}

void Body::update(double timestep, force totalForce) {
    // Update acceleration first
    force newAcc = div(totalForce, mass);
    this->acc = {newAcc.x, newAcc.y, newAcc.z};
    acceleration accStep = mul(acc, timestep);
    velocity velStep = mul(vel, timestep);

    this->pos = {
        pos.x + velStep.x, 
        pos.y + velStep.y, 
        pos.z + velStep.z
    };
    this->vel = {
        vel.x + accStep.x,
        vel.y + accStep.y,
        vel.z + accStep.z
    };

}

void Body::setName(std::string newName) {
    name = newName;
}
std::string Body::getName() {
    return name;
}

position Body::getPos() {
    return pos;
}
velocity Body::getVel() {
    return vel;
}
acceleration Body::getAcc() {
    return acc;
}
double Body::getMass() {
    return mass;
}

