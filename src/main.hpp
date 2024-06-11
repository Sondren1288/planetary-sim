#ifndef MAIN_CLASS
#define MAIN_CLASS

#include <TCanvas.h>
#include <TGFrame.h>
#include <TMultiGraph.h>
#include <TGraph2D.h>
#include <TPolyMarker3D.h>
#include <TPolyLine3D.h>
#include <TView3D.h>
#include <TButton.h>
#include <TTimer.h>
#include <TPad.h>
#include <TMath.h>
#include "TApplication.h"
#include <TGButton.h>
#include <TRootCanvas.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "model.hpp"
#include "types.hpp"

// Simple struct used for storing
// planets and their most recent positions,
// as well as a plottable line
struct pWithFunction {
    std::string name;
    std::vector<position> posArray;
    TPolyLine3D *graph;
};

class Main {
        static double deltaT;
        static int n_steps;
        static std::vector<pWithFunction> planetPlotter;
        static TCanvas *canv;
        static TCanvas *controls;
        static TTimer *timer;
        static TView3D *view;
        static int iteration;
        static double outer_range;
        static double zoom_factor;
        static int limit;
        static TButton* current_center;
        static std::vector<std::string> foci;

    public:
        static model::Model mod; // = model::Model(deltaT);

        // Data input
        std::vector<std::vector<std::string>> readData();
        std::vector<std::vector<double>> transformData(std::vector<std::vector<std::string>> &data);

        void initControlPanel();

        // Static funtion ensure they are valid for the entirety of 
        // the lifetime of the program, ensuring that they can be used 
        // by ROOT in buttons and similar
        void static printZoom();
        void static zoomIn();
        void static zoomOut();
        void static changeFocus();

        // Drawing
        void static drawSingularStepLimit(size_t);
        void static drawSingularStep();
        void static drawStep();
        void static drawRealTime();
        void static drawAllAfter();

        int main(int argc, char *argv[], TApplication *app);
};

// `run()` is defined here to be usable in `linkdef.h`
// If `run` was only defined in `main.cpp`, then it would not
// be visible in `linkdef.h` 
void run();
#endif
