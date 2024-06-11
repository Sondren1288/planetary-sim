#include <cmath>
#include <string>
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"

using namespace body;


Body::Body(double mass, position pos) {
    /*
     * A body with mass and initial position
     */
    this->mass = mass;
    this->pos = pos;
    this->radius = 0;
}

Body::Body(double mass, position pos, velocity v) {
    /*
     * A Body with mass, and initial position and velocity
     */
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
    this->radius = 0;
}

Body::Body(std::string name, double mass, position pos, velocity v, double radius) {
    /*
     * A body with a name, mass, and radius, as well as initial position and velocity.
     * Generally the one that is used
     */
    this->mass = mass;
    this->pos = pos;
    this->vel = v;
    this->radius = 0;
    setName(name);
}



force Body::getAttraction(Body &other) {
    /*
     * Find the attraction force between this body and another.
     */
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



void Body::updatePos(double timestep) {
    /*
     * Update position according to velocity verlet
     */
    pos = {
        pos.x + vel.x * timestep + acc.x * (timestep * timestep * 0.5),
        pos.y + vel.y * timestep + acc.y * (timestep * timestep * 0.5),
        pos.z + vel.z * timestep + acc.z * (timestep * timestep * 0.5)
    };
}



void Body::updateVerlet(double timestep, force totalForce) {
    /*
     * Update a body using the Velocity Verlet method for accuracy.
     * Due to the nature of celestial bodies, there is no drag or forces
     * depending on velocity of the body.
     * As such, the shortened version can be used.
     * https://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet
     *
     * Some of this logic is implemented in Model::updateBody
     */

    // x(t + dt) = x(t) + v(t)dt + 1/2 a(t)dt^2
    // v(t + dt) = v(t) + dt(a(t) + a(t + dt))/2 
    force newAcc = div(totalForce, mass);
    // Update velocity before updating acceleration
    vel = {
        vel.x + (acc.x + newAcc.x) * timestep * 0.5,
        vel.y + (acc.y + newAcc.y) * timestep * 0.5,
        vel.z + (acc.z + newAcc.z) * timestep * 0.5
    };
    acc = {newAcc.x, newAcc.y, newAcc.z};
}



/*************************************
 * Getters and setters
 *************************************/

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

