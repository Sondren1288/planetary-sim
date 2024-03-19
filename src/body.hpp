#ifndef BODY
#define BODY
#include <string>
#include "types.hpp"
namespace body {
    class Body {
        private:
            double mass;
            std::string name = "";
            position pos;
            velocity vel = {0, 0, 0};
            acceleration acc = {0, 0, 0};
            
        public:
            Body(double mass, position pos);
            Body(double mass, position pos, velocity v);
            Body(double mass, position pos, velocity v, acceleration acc);
            void setName(std::string);
            std::string getName();
            position getPos();
            velocity getVel();
            acceleration getAcc();
            double getMass();
            force getAttraction(Body &other);
            void update(double timeStep, force totalForce);
            position step(double timestop);
            void setPos(position);
    };
};
#endif

