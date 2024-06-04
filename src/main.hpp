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

struct pWithFunction {
    std::string name;
    std::vector<position> posArray;
    TPolyLine3D *graph;
};

class Main {
        static double deltaT;
        //TApplication app;
        //static TApplication app;// = TApplication("Orbitals", 0, 0);
        static int n_steps;
        static std::vector<pWithFunction> planetPlotter;
        static TCanvas *canv;
        static TCanvas *controls;
        static TTimer *timer;
        static TView3D *view;
        static int iteration;
        TPad *mainPad = nullptr;
        static double outer_range;
        static double zoom_factor;
        static int limit;
        static TButton* current_center;
        static std::vector<std::string> foci;

    public:
        static model::Model mod; // = model::Model(deltaT);

        Main();
        // Data input
        std::vector<std::vector<std::string>> readData();
        std::vector<std::vector<double>> transformData(std::vector<std::vector<std::string>> &data);

        void initControlPanel();

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

        int main();

        
};

void runner();
#endif
