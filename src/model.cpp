#include <vector>

#include "types.hpp"
#include "body.hpp"
#include "model.hpp"

using namespace model;



Model::Model(double deltaT) {
    /*
     * Init function which takes a deltaT.
     * deltaT represents the time passing between each plotted point
     */
    this->deltaT = deltaT / 2;
    simTime = 0.0;
}



bool Model::addBody(body::Body newBody) {
    /*
     * Add a body to the model.
     * Can be obtained with getBodyByName after
     */
    bodies.push_back(newBody);
    return true;
}



body::Body Model::getBodyByName(std::string toFind) {
    /*
     * Return a body with the given name, if it exists.
     * Else, returns an empty body
     */
    for (body::Body current : bodies) {
        if (current.getName() == toFind) {
            return current;
        }
    }
    return body::Body(0, {0, 0, 0});
}



double Model::iterate() {
    /*
     * Do one iteration of Velocity Verlet
     * (Actually 2, since drawing speed is the main limiting factor)
     */
    for (body::Body &body : bodies) {
        updateBody(body);
    }
    simTime = simTime + deltaT;
    // To improve accuraccy, we update twice with half the time interval instead
    // of only once, as the speed is mostly limited by the speed at which it draws
    // the plot instead of the actual calculations
    for (body::Body &body : bodies) {
        updateBody(body);
    }
    simTime = simTime + deltaT;
    return simTime;
}



void Model::updateBody(body::Body &body) {
    /*
     * Do an update of given body with the Velocity Verlet method.
     * First, updates position with the forces already acting upon the body.
     * Then, finds the total force now acting upon the body.
     * Then updates velocity and acceleration.
     */
    body.updatePos(deltaT);

    force totalForce = {0, 0, 0};
    // Sums total forces acting on body
    for (body::Body &other : bodies) {
        if (body.getName() != other.getName()) {
            totalForce = plu(totalForce, body.getAttraction(other));
        }
    }
    // Updates velocity given Velocity Verlet, then acceleration
    body.updateVerlet(deltaT, totalForce);
}

