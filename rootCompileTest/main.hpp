#ifndef MAIN_CLASS
#define MAIN_CLASS


#include <TApplication.h>

class Main {
    public:
        void static someFunc();
        void static run(TApplication *app);
};
void someFunc();
void run();

#endif
