#ifndef BODY
#define BODY
#include <string>
#include "types.hpp"
namespace body {
    class Body {
        protected:
            double mass;
            std::string name = "";
            double radius;
            position pos;
            velocity vel = {0, 0, 0};
            acceleration acc = {0, 0, 0};
            
        public:
            Body(double mass, position pos);
            Body(double mass, position pos, velocity v);
            Body(std::string name, double mass, position pos, velocity v, double radius);
            void setName(std::string);
            std::string getName();
            position getPos();
            velocity getVel();
            acceleration getAcc();
            double getMass();
            force getAttraction(Body &other);
            void updatePos(double timestep);
            void updateVerlet(double timeStep, force totalForce);
            void setPos(position);
            double getRadius();
            void setRadius(double);
    };
};
#endif

