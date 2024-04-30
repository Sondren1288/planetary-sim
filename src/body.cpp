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
    this->radius = 0;
}

Body::Body(double mass, position pos, velocity v) {
    // Velocity is constant (for now)
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
    this->radius = 0;
}

Body::Body(std::string name, double mass, position pos, velocity v, double radius) {
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
    this->radius = 0;
    setName(name);
}

Body::Body(double mass, position pos, velocity v, acceleration acc) {
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
    this->acc = acc;
    this->radius = 0;
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

void Body::updatePos(double timestep) {
    pos = {
        pos.x + vel.x * timestep + acc.x * (timestep * timestep * 0.5),
        pos.y + vel.y * timestep + acc.y * (timestep * timestep * 0.5),
        pos.z + vel.z * timestep + acc.z * (timestep * timestep * 0.5)
    };
}

void Body::updateVerlet(double timestep, force totalForce) {
    // x(t + dt) = x(t) + v(t)dt + 1/2 a(t)dt^2
    // v(t + dt) = v(t) + dt(a(t) + a(t + dt))/2 
    force newAcc = div(totalForce, mass);
    vel = {
        vel.x + (acc.x + newAcc.x) * timestep * 0.5,
        vel.y + (acc.y + newAcc.y) * timestep * 0.5,
        vel.z + (acc.z + newAcc.z) * timestep * 0.5
    };
    acc = {newAcc.x, newAcc.y, newAcc.z};
}

position Body::step(double timestep) {
    // Update acceleration first
    velocity velStep = mul(vel, timestep);

    position tPos = {
        pos.x + velStep.x, 
        pos.y + velStep.y, 
        pos.z + velStep.z
    };

    return tPos;
}

void Body::setPos(position p) {
    pos = p;
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
double Body::getRadius() {
    return radius;
}
void Body::setRadius(double newRad) {
    radius = newRad;
}

