#ifndef MODEL
#define MODEL
#include <vector>
#include <string>
#include "types.hpp"
#include "body.hpp"
namespace model {
    class Model {
        private:
            double deltaT;
            double simTime;
            std::vector<body::Body> bodies;
            
            void updateBody(body::Body &body);
        public:
            Model(double deltaT);
            bool addBody(body::Body); 
            double iterate();  // Move simulation one step, return total sim-time
            void setDeltaT(double);
            double getDeltaT();
            body::Body getBodyByName(std::string);

            // Iterate over bodies?
            std::vector<body::Body> getBodies();
    };
};

#endif
