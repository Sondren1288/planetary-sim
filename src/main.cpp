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

#include "model.hpp"
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"
#include "main.hpp"

double Main::deltaT = (60 * 60) * 6;
model::Model Main::mod = model::Model(Main::deltaT);
int Main::n_steps = 100000;
// Globals
// Init model to be able to add planets
TCanvas *Main::canv = new TCanvas("PLOTTER", "Plotter", 1500, 1500);
TCanvas *Main::controls = new TCanvas("CONTROLS", "Controls");
TTimer *Main::timer = new TTimer(0);
TView3D *Main::view = (TView3D*) TView::CreateView(1, 0, 0);
int Main::iteration = 0;
double Main::outer_range = 4'540'000'000'000;
double Main::zoom_factor = 1.0;
int Main::limit = 500;
std::vector<pWithFunction> Main::planetPlotter = {};
std::vector<std::string> Main::foci = {"origo"};
TApplication Main::app = TApplication("Orbitals", 0, 0);
TButton *Main::current_center = new TButton();

Main::Main() {
    // app = TApplication("Orbitals", 0, 0);
    // Globals
    // Init model to be able to add planets
    mainPad = new TPad("PAD", "Pad", 0.01, 0.01, 0.99, 0.99);
}

std::vector<std::vector<std::string>> Main::readData() {
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

std::vector<std::vector<double>> Main::transformData(std::vector<std::vector<std::string>> &data) {
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




void Main::initControlPanel() {
    controls->cd();
    int layers = 3;
    float padding = 0.05;
    float boxSize = 1.0 / layers - padding - padding / layers;
    std::cout << "Box size: " << boxSize << std::endl;
    float iterPos = padding + boxSize;
    // Top == 1.0
    // Bottom left == 0, 0
    //
    TButton *step = new TButton("Step", "Main::drawStep()", padding, 1 - (padding + boxSize), padding + boxSize, 1 - padding);
    TButton *allAfter = new TButton("10 000 Steps", "Main::drawAllAfter()", padding + boxSize + padding, 1 - (padding + boxSize), (padding + boxSize) * 2, 1 - padding); 
    TButton *realTime = new TButton("Real time", "Main::drawRealTime()", (padding + boxSize) * 2 + padding, 1 - (padding + boxSize), (padding + boxSize) * 3, 1 - padding);
    step->SetTextSize(0.15);
    allAfter->SetTextSize(0.15);
    realTime->SetTextSize(0.15);

    TButton *zoomIn = new TButton("+", "Main::zoomIn()", (padding + boxSize) * 2 + padding, 1 - (padding + boxSize) * 2, (padding + boxSize) * 3, 1 - (padding + boxSize) - padding);
    TButton *zoomOut = new TButton("-", "Main::zoomOut()", (padding + boxSize) * 2 + padding, 1 - (padding + boxSize) * 3, (padding + boxSize) * 3, 1 - (padding + boxSize) * 2 - padding);
    current_center = new TButton("origo", "Main::changeFocus()", padding, padding, (padding + boxSize) * 2, (padding + boxSize) * 2);
    current_center->SetTextSize(0.3);

    // ??
    TGTextButton *allAfter_ = new TGTextButton();
    
    step->Draw();
    realTime->Draw();
    allAfter->Draw();
    zoomIn->Draw();
    zoomOut->Draw();
    current_center->Draw();
}

void Main::printZoom() {
    std::cout << "Current zoom level: " << zoom_factor << std::endl;
}

void Main::zoomIn() {
    zoom_factor = zoom_factor * 0.95;
    printZoom();
}

void Main::zoomOut() {
    zoom_factor = zoom_factor * 1.05;
    printZoom();
}

void Main::drawSingularStepLimit(size_t limit=500) {
    canv->cd();
    canv->Clear();
    view = (TView3D*) TView::CreateView(1, 0, 0);
    double cur_zoom = outer_range * zoom_factor;
    if (strcmp(current_center->GetTitle(), "origo") != 0) {
        position p = mod.getBodyByName(current_center->GetTitle()).getPos();
        view->SetRange(-cur_zoom + p.x, -cur_zoom + p.y, -cur_zoom +p.z, cur_zoom + p.x, cur_zoom + p.y, cur_zoom + p.z);
    } else {
        view->SetRange(-cur_zoom, -cur_zoom, -cur_zoom, cur_zoom, cur_zoom, cur_zoom);
    }
    
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

void Main::changeFocus() {
    controls->cd();
    std::string name = current_center->GetTitle();
    std::vector<std::string>::iterator pos = std::find(foci.begin(), foci.end(), name);
    int index = 0;
    if (pos + 1 == foci.end()) {
        index = 0;
    } else {
        index = pos - foci.begin() + 1;
    };
    current_center->SetTitle(foci[index].c_str());
    current_center->Draw();
}

void Main::drawStep() {
    timer->TurnOff();
    drawSingularStepLimit(limit);
    view->ShowAxis();
}

void Main::drawRealTime() {
    std::string tmp = "Main::drawSingularStepLimit(" + std::to_string(limit) + ")";
    char *tmp_c = new char[tmp.length() + 1];
    std::strcpy(tmp_c, tmp.c_str());
    timer->SetCommand(tmp_c);
    timer->SetTime(10);
    timer->TurnOn();
}

void Main::drawAllAfter() {
    timer->TurnOff();
    n_steps = iteration + 10'000;
    if (iteration >= n_steps) {
        ;
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

int Main::main() {


    //canv = new TCanvas("PLOTTER", "Plotter", 1500, 1500);
    timer = new TTimer(0);
    view = (TView3D*) TView::CreateView(1, 0, 0);
    mainPad = new TPad("PAD", "Pad", 0.01, 0.01, 0.99, 0.99);

    if (canv->GetAspectRatio() == 1.0) {
        canv->SetFixedAspectRatio(true);
    } else {
        std::cout << "Aspect ratio not 1:1" << std::endl;
        std::cout << "Aspect ratio: " << canv->GetAspectRatio() << std::endl;
    }
    
    std::vector<std::vector<std::string>> tmpData, pNames;
    tmpData = readData();
    std::vector<std::vector<double>> planetaryData;
    planetaryData = transformData(tmpData);
    // pNames contain the name of the planet and the body it depends on
    pNames = tmpData;
    for (std::vector<std::string> s : pNames) {
        foci.push_back(s[0]);
    }
    // TODO some magic with aphelion and parahelion to
    // calculate position and velocity at this point

    initControlPanel();

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
            vel = plu(mod.getBodyByName(dependant_body).getVel(), {0, vel_min, 0});
            pos = plu(mod.getBodyByName(dependant_body).getPos(), pos);
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
    
    app.Run();

    std::cout << "Running, q to quit" << std::endl;
    std::string input;
    std::cin >> input;
    while ("q" != input && "Q" != input) {
        std::cin >> input;
    }

    return 0;
}

void runner() {
    Main program = Main();
    program.main();
}

int main() {
    Main program = Main();
    program.main();
}
