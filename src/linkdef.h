#ifdef __CLING__

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;


// All header-files
#include "constants.hpp"
#include "body.hpp"
#include "model.hpp"
#include "types.hpp"
#include "main.hpp"

// All classes
#pragma link C++ class Model+;
#pragma link C++ class Body+;
#pragma link C++ class Main+;

#pragma link C++ function run();

#endif
