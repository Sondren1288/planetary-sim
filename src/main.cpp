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
#include <TApplication.h>
#include <TGButton.h>
#include <TRootCanvas.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <iterator>

#include "model.hpp"
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"



struct pWithFunction {
    std::string name;
    std::vector<position> posArray;
    TPolyLine3D *graph;
};



const int n_steps = 100000;
// Globals
std::vector<pWithFunction> planetPlotter;
// Init model to be able to add planets
double deltaT = (60 * 60) * 6; // Seconds. Here an hour at a time * 6 
model::Model mod = model::Model(deltaT);
TCanvas *canv = new TCanvas("PLOTTER", "Plotter", 1500, 1500);
TTimer *timer = new TTimer(0);
TView3D *view = (TView3D*) TView::CreateView(1, 0, 0);
int iteration = 0;
TPad *mainPad = new TPad("PAD", "Pad", 0.01, 0.01, 0.99, 0.99);
double outer_range = 249'261'000'000;
double zoom_factor = 1.0;
int limit = 5000;
TApplication *app;


std::vector<std::vector<std::string>> readData() {
    std::ifstream data("planetaryData.csv");

    std::vector<std::vector<std::string>> tmpData;
    std::vector<std::string> strData;

    bool headerFlag = true;
    std::string tmp, line, col;

    while (data.good()) {
        // Clear strData for each iteration
        strData.clear();

        std::getline(data, line);
        
        // Skip header
        if (headerFlag) {
            headerFlag = false;
            continue;
        }

        std::stringstream sstram(line);

        while (std::getline(sstram, col, ',')) {
            strData.push_back(col);
        }
        if (!strData.empty()) {
            tmpData.push_back(strData);
        }
    }
    return tmpData;
}

std::vector<std::vector<double>> transformData(std::vector<std::vector<std::string>> &data) {
    /*
     * Transforms the data where the first column is a list of names
     * into a singular column, where the rest of the data is 
     * made into another table of doubles
     */
    // Man is this something to look at. But it works
    std::vector<std::vector<std::string>> objectColumn;
    std::vector<std::vector<double>> dataObject;

    // Iterate over each row (Corresponds to planets)
    for (std::vector<std::string> column : data) {
        // Name, mass, aphelion, periphelion, avg orbital vel, radius, eccentricity, dependant body
        
        // Add the name and dependent body to the objectColumn
        std::vector<std::string> name = {column[0], column[7]};
        objectColumn.push_back(name);
        

        // Create empty vector to store doubles
        // Could have used doubleVec.clear, but
        // the amount of times this is going to be
        // run makes this insignificant
        std::vector<double> doubleVec;
        // Flag to drop the first column
        bool dropFlag = true;
        int counter = -1;
        for (std::string data_ : column) {
            // Drop the first column
            counter++;
            if (dropFlag) {
                dropFlag = false;
                continue;
            }
            if (counter == 7) {
                continue;
            }
            // Transform the value into double
            // and then add it to doubleVec
            doubleVec.push_back(std::stod(data_));

        }
        // Add the list of values as a row to
        // dataObject
        dataObject.push_back(doubleVec);
    }
    // Swap data and objectColumn.
    // This will make the original vector
    // change into the one we created
    data.swap(objectColumn);
    return dataObject;
}




void initControlPanel(TCanvas *canv) {
    canv->cd();
    int layers = 4;
    float padding = 0.05;
    float boxSize = 1.0 / layers - padding - padding / layers;
    std::cout << "Box size: " << boxSize << std::endl;
    float iterPos = padding + boxSize;
    // Top == 1.0
    // Bottom left == 0, 0
    TButton *zoomIn = new TButton("+", "zoomIn()", 0.1, padding, 0.45, iterPos);
    TButton *zoomOut = new TButton("-", "zoomOut()", 0.55, padding, 0.9, iterPos);
    TButton *realTime = new TButton("REALTIME", "drawRealTime()", 0.1, iterPos + padding, 0.9, iterPos * 2);
    TButton *allAfter = new TButton("ALL", "drawAllAfter()", 0.1, iterPos * 2 + padding, 0.9, iterPos * 3);
    
    // ??
    TGTextButton *allAfter_ = new TGTextButton();
    
    TButton *step = new TButton("STEP", "drawStep()", 0.1, iterPos * 3 + padding, 0.9, iterPos * 4);
    step->Draw();
    realTime->Draw();
    allAfter->Draw();
    zoomIn->Draw();
    zoomOut->Draw();
}
void printZoom() {
    std::cout << "Current zoom level: " << zoom_factor << std::endl;
}

void zoomIn() {
    zoom_factor = zoom_factor * 0.95;
    printZoom();
}

void zoomOut() {
    zoom_factor = zoom_factor * 1.05;
    printZoom();
}

void drawSingularStepLimit(size_t limit=500) {
    canv->cd();
    canv->Clear();
    view = (TView3D*) TView::CreateView(1, 0, 0);
    double cur_zoom = outer_range * zoom_factor;
    view->SetRange(-cur_zoom, -cur_zoom, -1000, cur_zoom, cur_zoom, 1000);
    
    bool first = true;
    for (pWithFunction &pStruct : planetPlotter) {

        body::Body curBod = mod.getBodyByName(pStruct.name);
        position p = curBod.getPos();

        TPolyMarker3D *plt = new TPolyMarker3D();
        plt->SetPoint(0, p.x, p.y, p.z);
        plt->SetPoint(1, p.x, p.y, p.z);
        plt->SetMarkerStyle(20);
        //plt->SetMarkerStyle(53+(18*2));
        plt->SetMarkerSize(2.3);
        plt->Draw("");
        first = false;

        // Increase vector
        if (pStruct.posArray.size() <= limit) {
            pStruct.posArray.push_back(p);
        } else {
            pStruct.posArray.erase(pStruct.posArray.begin(), pStruct.posArray.end() - limit);
            pStruct.posArray.push_back(p);
            pStruct.graph->SetPolyLine(limit);
        }

        // Set the points to draw
        for (int i = 0; i < pStruct.posArray.size(); i++) {
            std::vector<position> &poses = pStruct.posArray;
            position tmpP = poses[i];
            pStruct.graph->SetPoint(i, tmpP.x, tmpP.y, tmpP.z);
        }

        // Draw the thing
        if (first) {
            pStruct.graph->Draw();
            first = false;
        } else {
            pStruct.graph->Draw("SAME");
        }
    }
    mod.iterate();
    iteration++;
    if (iteration % 1000 == 0) {
        std::cout << "Iteration: " << iteration << std::endl;
    }
    //canv->SetRealAspectRatio();
    canv->Modified();
    canv->Update();
}

void drawSingularStep() {
    canv->cd();
    canv->Clear();
    view = (TView3D*) TView::CreateView(1, 0, 0);
    view->SetRange(-249'261'000'000, -249'261'000'000, -1000, 249'261'000'000, 249'261'000'000, 1000);
    //view->ShowAxis();
    //mainPad->Clear();
    bool first = true;
    for (pWithFunction pStruct : planetPlotter) {
        body::Body curBod = mod.getBodyByName(pStruct.name);
        position p = curBod.getPos();
        pStruct.graph->SetPoint(iteration, p.x, p.y, p.z);
        if (first) {
            pStruct.graph->Draw();
            first = false;
        } else {
            pStruct.graph->Draw("SAME");
        }
    }
    mod.iterate();
    iteration++;
    if (iteration % 1000 == 0) {
        std::cout << "Iteration: " << iteration << std::endl;
    }
    //canv->SetRealAspectRatio();
    canv->Modified();
    canv->Update();
}

void drawStep() {
    timer->TurnOff();
    drawSingularStepLimit(limit);
    view->ShowAxis();
}

void drawRealTime() {
    std::string tmp = "drawSingularStepLimit(" + std::to_string(limit) + ")";
    char *tmp_c = new char[tmp.length() + 1];
    std::strcpy(tmp_c, tmp.c_str());
    timer->SetCommand(tmp_c);
    timer->SetTime(10);
    timer->TurnOn();
}

void drawAllAfter() {
    timer->TurnOff();
    if (iteration >= n_steps) {
        return;
    } else {
        while (iteration < n_steps) {
            for (pWithFunction &pStruct : planetPlotter) {
                body::Body curBod = mod.getBodyByName(pStruct.name);
                position p = curBod.getPos();
                //pStruct.graph->SetPoint(iteration, p.x, p.y, p.z);
                pStruct.posArray.push_back(p);
                mod.iterate();
                iteration++;
                if (iteration % 1000 == 0) {
                    std::cout << "Iteration " << iteration << std::endl;
                }
            }
        }
        for (pWithFunction &pStruct : planetPlotter) {
            for (int i = 0; i < pStruct.posArray.size(); i++) {
                position p = pStruct.posArray[i];
                pStruct.graph->SetPoint(i, p.x, p.y, p.z);
            }
        }
        drawSingularStepLimit(iteration);
        view->ShowAxis();
    }
    //drawRemainder();
}


// Need an update function that is equal for all of them
// , but I have that already
void update(model::Model model) {
    model.iterate();
}

int main(int argc, char **argv) {

    app = new TApplication("Orbitals", &argc, argv);

    canv = new TCanvas("PLOTTER", "Plotter", 1500, 1500);
    timer = new TTimer(0);
    view = (TView3D*) TView::CreateView(1, 0, 0);
    mainPad = new TPad("PAD", "Pad", 0.01, 0.01, 0.99, 0.99);

    std::vector<std::vector<std::string>> tmpData, pNames;
    tmpData = readData();
    std::vector<std::vector<double>> planetaryData;
    planetaryData = transformData(tmpData);
    // pNames contain the name of the planet and the body it depends on
    pNames = tmpData;
    // TODO some magic with aphelion and parahelion to
    // calculate position and velocity at this point

    TCanvas *controls = new TCanvas("CONTROLS", "Controls");
    initControlPanel(controls);

    // These lines are also relatively inefficient, 
    // but again it is fine, because it only runs at startup
    for (int row = 0; row < pNames.size(); row++) {

        // For each row 
        // mass, aphelion, periphelion, avg orbital vel, radius, eccentricity
        double mass = planetaryData[row][0];
        // Currently using aphelion as distance 
        // Will be using the aphelion and finding the minumum velocity
        // that a planet has in its orbit
        position pos = {planetaryData[row][1], 0, 0};
        double radius = planetaryData[row][4];

        // To find min velocity, we use the formulae listed at
        // https://en.wikipedia.org/wiki/Apsis#Mathematical_formulae
        // v_per = sqrt[ (1-e)mu / (1+e)a ]
        // a = (r_per + r_ap) / 2 // semi major axis
        // mu = G(m_1 + m_2) // mass_1 and mass_2, so this does not work for the sun
        // e: eccentricity. Given in data
        velocity vel;
        std::string dependant_body = pNames[row][1];
        if (dependant_body == "none") {
            vel = {0, planetaryData[row][3], 0};
        } else {
            double eccentricity = planetaryData[row][5];
            double dependant_mass = mod.getBodyByName(dependant_body).getMass();
            double semi_major = (planetaryData[row][2] + planetaryData[row][1]) / 2.0;
            double gravitational_parameter = GRAVITATIONAL * (mass + dependant_mass);
            double vel_min = sqrt( (1-eccentricity) * gravitational_parameter  /  ( (1+eccentricity) * semi_major ) );
        
            std::cout << "Planet: " << pNames[row][0] << " minimum velocity: " << vel_min << std::endl;
            vel = {0, vel_min, 0};
            std::cout << "Position: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
        }
        // Init body and add to model
        body::Body bodyToAdd = body::Body(pNames[row][0], mass, pos, vel, radius);
        mod.addBody(bodyToAdd);

        //TPolyMarker3D *curPlot = new TPolyMarker3D();
        TPolyLine3D *curPlot = new TPolyLine3D();
        curPlot->SetLineWidth(8);
        curPlot->SetLineStyle(1);
        //curPlot->SetMarkerSize(50000);
        //curPlot->SetMarkerStyle(50+(18*3));
        std::vector<position> pos_ = {};
        planetPlotter.push_back({pNames[row][0], pos_, curPlot});
    }

    canv->cd();
    //mainPad->SetCanvas(canv);
    //canv->SetPadSave(mainPad);
    //view->SetRange(-249'261'000'0, -249'261'000'0, -1000, 249'261'000'0, 249'261'000'0, 1000);
    //view->SetRange(-249'261'000'000, -249'261'000'000, -1000, 249'261'000'000, 249'261'000'000, 1000);
    view->ShowAxis();
    drawStep();

    TRootCanvas *rc = (TRootCanvas*) canv->GetCanvasImp();
    rc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
    
    app->Run();

    std::cout << "Running, q to quit" << std::endl;
    std::string input;
    std::cin >> input;
    while ("q" != input && "Q" != input) {
        std::cin >> input;
    }

    return 0;
}

int main_() {
    std::vector<std::vector<std::string>> tmpData, pNames;
    tmpData = readData();
    std::vector<std::vector<double>> planetaryData;
    planetaryData = transformData(tmpData);
    pNames = tmpData;
    // TODO some magic with aphelion and parahelion to
    // calculate position and velocity at this point
    // Init drawer here
    TCanvas *c1 = new TCanvas("Apple", "Bees", 6000, 6000);
    TCanvas *c2 = new TCanvas("Some", "Plot", 1500, 1500); 

    // Controls and timer
    TCanvas *controls = new TCanvas("CONTROLS", "Controls");

    TMultiGraph *mg = new TMultiGraph();
    c1->Divide(2,2);
    // Uses the name of the canvas to create the pads
    TPad *pad_1 = (TPad*) c1->GetPrimitive("Apple_1");
    TPad *pad_2 = (TPad*) c1->GetPrimitive("Apple_2");
    TPad *pad_3 = (TPad*) c1->GetPrimitive("Apple_3");
    TPad *pad_4 = (TPad*) c1->GetPrimitive("Apple_4");


    // These lines are also relatively inefficient, 
    // but again it is fine, because it only runs at startup
    for (int row = 0; row < pNames.size(); row++) {

        // For each row 
        // mass, aphelion, periphelion, avg orbital vel, radius, eccentricity
        double mass = planetaryData[row][0];
        // Currently using perihelion as distance TODO
        position pos = {planetaryData[row][2], 0, 0};
        velocity vel = {0, planetaryData[row][3], 0};
        double radius = planetaryData[row][4];

        // Init body and add to model
        body::Body bodyToAdd = body::Body(pNames[row][0], mass, pos, vel, radius);
        mod.addBody(bodyToAdd);

        TPolyMarker3D *curPlot = new TPolyMarker3D();
        //TPolyLine3D *curPlot = new TPolyLine3D();
        //curPlot->SetLineWidth(5);
        curPlot->SetMarkerSize(50);
        std::vector<position> pos_;
        //planetPlotter.push_back({pNames[row][0], pos_, curPlot});
    }

    bool sameFlag = false;
    c2->cd();
    // Create 3D view
    TView3D *view = (TView3D*) TView::CreateView(1);
    view->SetRange(-249'261'000'000, -249'261'000'000, -1000, 249'261'000'000, 249'261'000'000, 1000);
    view->ShowAxis();


    std::cout << planetPlotter.size() << std::endl;
    for (int step = 0; step < n_steps; step++) {

        for (pWithFunction pStruct : planetPlotter) {
            std::cout << "Iteration: " << step << std::endl;
            body::Body curBod = mod.getBodyByName(pStruct.name);
            position p = curBod.getPos();
            pStruct.graph->SetPoint(step, p.x, p.y, p.z);
            if (sameFlag) {
                pStruct.graph->Draw("SAME");
            } else {
                pStruct.graph->Draw("");
                sameFlag = true;
            }
        }
        mod.iterate();
        c2->Modified();
        c2->Update();
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    
    }
    sameFlag = false;
    for (pWithFunction pStruct : planetPlotter) {
        //mg->Add(pStruct.graph, "pl");
        if (sameFlag) {
            pStruct.graph->Draw("SAME");
        } else {
            pStruct.graph->Draw();
        }
        std::cout << pStruct.graph << std::endl;
    }
    c2->Modified();
    c2->Update();

    return 0;
    c1->cd();

    // Create a model and add 2 bodies
    // The sun
    body::Body sun = body::Body(1.9885 * pow(10, 30), {0, 0, 0});
    sun.setRadius(696'340'000);
    // Can also be written as
    //body::Body sun_(1.9885 * pow(10, 30), {0, 0, 0});
    // Earth-like body. Mass is kg, distance is km
    body::Body earthLike = body::Body(5.972168 * pow(10, 24), {149'598'023'000, 0, 0}, {1000, 20000, 0});
    earthLike.setRadius(6'371'000);
    sun.setName("Sun");

    earthLike.setName("Earth");
    
    // Init model
    // Add bodies to model
    mod.addBody(sun);
    mod.addBody(earthLike);

    // Ensure getBodyByName works
    std::cout << "Should now print \"Sun\": "
        << mod.getBodyByName("Sun").getName() 
        << std::endl;


    int n_steps = 20000;
    double *simtime = new double[n_steps];
    double *sunx = new double[n_steps];
    double *suny = new double[n_steps];
    double *earthx = new double[n_steps];
    double *earthy = new double[n_steps];

    simtime[0] = 0;
    sunx[0] = 0;
    suny[0] = 0;
    earthx[0] = earthLike.getPos().x;
    earthy[0] = 0;

    double time;
    // TODO update plotting to auto-divide into 4 plots
    // and have the plotting iterate over a list instead of doing so manually
    TGraph2D *sunPlot = new TGraph2D();
    TGraph2D *earthPlot = new TGraph2D();
    TGraph2D *sunPlot21 = new TGraph2D();
    TGraph2D *earthPlot21 = new TGraph2D();
    earthPlot->SetTitle("GraphTitle; Pos x; Pos y; Sim time");

    std::cout << sunPlot << std::endl;
    std::cout << earthPlot << std::endl;

    for (int i = 0; i < n_steps; i++) {
        sun = mod.getBodyByName("Sun");
        earthLike = mod.getBodyByName("Earth");
        // Error handling
        if (earthLike.getPos().x >= 1e+12 || earthLike.getPos().x <= -1e+12) {
            std::cout << "Planet moving an order of magnitude away" << std::endl;
            std::cout << "Current position on x-axis: " << earthLike.getPos().x << std::endl;
            break;
        }
        // Add points to plot
        sunPlot->SetPoint(i, sun.getPos().x, sun.getPos().y, simtime[i]);
        earthPlot->SetPoint(i, earthLike.getPos().x, earthLike.getPos().y, simtime[i]);
        
        earthPlot21->SetPoint(i, earthLike.getPos().x, earthLike.getPos().z, simtime[i]);
        sunPlot21->SetPoint(i, sun.getPos().x, sun.getPos().z, simtime[i]);

        sunx[i] = sun.getPos().x;
        suny[i] = sun.getPos().y;
        earthx[i] = earthLike.getPos().x;
        earthy[i] = earthLike.getPos().y;
        
        if (i == n_steps) {
            break;
        }
        simtime[i+1] = mod.iterate();
        //std::cout << earthLike.getAcc().x << std::endl;
    }
    
    for (TObject *obj : *c1->GetListOfPrimitives()) {
        std::cout << obj->GetName() << std::endl;
    }

    pad_1->cd();
    earthPlot->Draw("LINE PCOL");
    sunPlot->Draw("SAME LINE");
    pad_2->cd();
    for (int i = 0; i < 20; i+=5) {
        std::cout << "Sun x; " << sunx[i] << "  Earth x; " << earthx[i] << std::endl; 
    }
    earthPlot21->Draw("LINE PCOL");
    sunPlot21->Draw("SAME LINE");
    c1->SaveAs("plot.png");

    return 0;
}
