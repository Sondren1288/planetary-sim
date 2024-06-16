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
#include <TSystem.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <random>

#include "model.hpp"
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"
#include "main.hpp"



/*
 * Init of static variables.
 * These are defined in main.hpp 
 * and are static as to ensure their availability
 * for the lifetime of the program.
 */
double Main::deltaT = (60 * 60); // 1 hours each step
model::Model Main::mod = model::Model(Main::deltaT);
// How many steps each time you press the "X steps"
int Main::n_steps = 100000;
// Init of canvases and ROOT specific views
// We make them nullptrs because we want to have them in scope, but
// we don't want to make an actual TCanvas before we have a TApplication
TCanvas *Main::canv = nullptr;
TCanvas *Main::controls = nullptr;
TTimer *Main::timer = new TTimer(0);
TView3D *Main::view = nullptr; 
// Some iterative and changeable values
int Main::iteration = 0;
double Main::outer_range = 4'540'000'000'000;
double Main::zoom_factor = 1.0;
// The number of points after each planet to draw
int Main::limit = 500;
// Init of vector that will contain information on all planets
std::vector<pWithFunction> Main::planetPlotter = {};
// Init of vector to determine focus point / view of graph
std::vector<std::string> Main::foci = {"origo"};
// A button to be used with the focus point
TButton *Main::current_center = nullptr; 



std::vector<std::vector<std::string>> Main::readData() {
    /*
     * Function that reads the "planetaryData" csv and
     * stores the data in a vector which it then returns
     */
    // Open file
    std::ifstream data("planetaryData.csv");

    // Init vectors
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
     * Transforms the data into two seperate vectors.
     * The 1st column and the 8th column contain planet names and dependent body,
     * so they remain in the provided vector, whereas all other values are 
     * transformed to doubles and added to the returned vector. 
     * This function modifies the provided vector.
     */

    // Init of vector objects
    std::vector<std::vector<std::string>> objectColumn;
    std::vector<std::vector<double>> dataObject;

    // Iterate over each row (Corresponds to planets)
    for (std::vector<std::string> column : data) {
        // Name, mass, aphelion, periphelion, avg orbital vel, radius, eccentricity, dependant body, inclination
        
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
            if (counter != 8) { 
                // Transform the value into double
                // and then add it to doubleVec
                doubleVec.push_back(std::stod(data_));
            } else {
                // This is for inclination, and since it 
                // is given in degrees, transform to radians
                double degRot = std::stod(data_);
                degRot = degRot * 3.1415 / 180;
                doubleVec.push_back(degRot);
            }
        }
        // Add the list of values as a row to
        // dataObject
        dataObject.push_back(doubleVec);
    }
    // Swap data and objectColumn.
    // This will make the original vector
    // change into the one we created
    // containing names and dependent bodies.
    data.swap(objectColumn);
    // Return the vector of doubles
    return dataObject;
}



void Main::initControlPanel() {
    /*
     * Create the buttons and their layout,
     * as well as their functions.
     * The `controls` canvas needs to be already made.
     */
    controls->cd();
    // Grid-size
    int layers = 3;
    float padding = 0.05;
    float boxSize = 1.0 / layers - padding - padding / layers;
    std::cout << "Box size: " << boxSize << std::endl;

    // Bottom left == 0, 0
    // Upper row
    TButton *step = new TButton("Step", "Main::drawStep()", padding, 1 - (padding + boxSize), padding + boxSize, 1 - padding);
    TButton *allAfter = new TButton("10 000 Steps", "Main::drawAllAfter()", padding + boxSize + padding, 1 - (padding + boxSize), (padding + boxSize) * 2, 1 - padding); 
    TButton *realTime = new TButton("Real time", "Main::drawRealTime()", (padding + boxSize) * 2 + padding, 1 - (padding + boxSize), (padding + boxSize) * 3, 1 - padding);
    // Scale text to be readable on button
    step->SetTextSize(0.15);
    allAfter->SetTextSize(0.15);
    realTime->SetTextSize(0.15);

    // 2 lower rows, current_center spans 2 positions in each direction
    TButton *zoomIn = new TButton("+", "Main::zoomIn()", (padding + boxSize) * 2 + padding, 1 - (padding + boxSize) * 2, (padding + boxSize) * 3, 1 - (padding + boxSize) - padding);
    TButton *zoomOut = new TButton("-", "Main::zoomOut()", (padding + boxSize) * 2 + padding, 1 - (padding + boxSize) * 3, (padding + boxSize) * 3, 1 - (padding + boxSize) * 2 - padding);
    current_center = new TButton("origo", "Main::changeFocus()", padding, padding, (padding + boxSize) * 2, (padding + boxSize) * 2);
    // Scale text of current_center. + and - does not need to be scaled
    current_center->SetTextSize(0.22);
    
    // Draw the buttons to the canvas
    step->Draw();
    realTime->Draw();
    allAfter->Draw();
    zoomIn->Draw();
    zoomOut->Draw();
    current_center->Draw();
    controls->Update();
}



void Main::printZoom() {
    /*
     * Print the current zoom scale from "default"
     */
    std::cout << "Current zoom level: " << zoom_factor << std::endl;
}



void Main::zoomIn() {
    /*
     * Scale the current zoom-factor down
     */
    zoom_factor = zoom_factor * 0.85;
    printZoom();
}



void Main::zoomOut() {
    /*
     * Scale the current zoom-factor up
     */
    zoom_factor = zoom_factor * 1.15;
    printZoom();
}



void Main::drawSingularStepLimit(size_t limit=500) {
    /*
     * Draw a single step for each planet where the 
     * number of allowed points for each tail eaquals the
     * limit size_t.
     *
     * Will iterate the simulation one step forward.
     */
    
    // Clear canvas as to not draw ontop of the old graph
    canv->cd();
    canv->Clear();
    // When canvas was cleared, the view was deleted.
    // This has to be created again.
    view = (TView3D*) TView::CreateView(1, 0, 0);
    // Using the zoom_factor to set the current viewing distance.
    double cur_zoom = outer_range * zoom_factor;
    // If the current "selected" orbital is not origo, move to said location
    if (strcmp(current_center->GetTitle(), "origo") != 0) {
        position p = mod.getBodyByName(current_center->GetTitle()).getPos();
        view->SetRange(-cur_zoom + p.x, -cur_zoom + p.y, -cur_zoom +p.z, cur_zoom + p.x, cur_zoom + p.y, cur_zoom + p.z);
    } else {
        // Else, draw with origo as the center.
        view->SetRange(-cur_zoom, -cur_zoom, -cur_zoom, cur_zoom, cur_zoom, cur_zoom);
    }
    
    // Iterate the model
    mod.iterate();
    // Add one to the iteration count
    iteration++;
    if (iteration % 1000 == 0) {
        std::cout << "Iteration: " << iteration << std::endl;
    }

    bool first = true;
    // Iterate over each planet
    for (pWithFunction &pStruct : planetPlotter) {

        body::Body curBod = mod.getBodyByName(pStruct.name);
        position p = curBod.getPos();

        // Draw the sphere that is the current position of the body
        TPolyMarker3D *plt = new TPolyMarker3D();
        plt->SetPoint(0, p.x, p.y, p.z);
        plt->SetPoint(1, p.x, p.y, p.z);
        plt->SetMarkerStyle(20);
        plt->SetMarkerSize(2.3);
        plt->Draw("");
        first = false;

        if (pStruct.posArray.size() <= limit) {
            // Increase vector if not over or at the limit
            pStruct.posArray.push_back(p);
        } else {
            // If at the limit, erase the difference between length and limit, and add the new position
            pStruct.posArray.erase(pStruct.posArray.begin(), pStruct.posArray.end() - limit);
            pStruct.posArray.push_back(p);
            pStruct.graph->SetPolyLine(limit);
        }

        // Set the points to draw
        for (int i = 0; i < pStruct.posArray.size(); i++) {
            std::vector<position> &poses = pStruct.posArray;
            position tmpP = poses[i];
            // Replace points in graph
            pStruct.graph->SetPoint(i, tmpP.x, tmpP.y, tmpP.z);
        }

        // Draw the first without "SAME"
        if (first) {
            pStruct.graph->Draw();
            first = false;
        } else {
            pStruct.graph->Draw("SAME");
        }
    }
    // Update and draw the changes to the canvas
    canv->Resized();
    canv->Resize();
    canv->Modified();
    canv->Update();
}



void Main::changeFocus() {
    /*
     * Change focus by iterationg over a list
     * of bodies. Will loop when all are passed
     */
    controls->cd();
    std::string name = current_center->GetTitle();
    // Find the position of the current focus
    std::vector<std::string>::iterator pos = std::find(foci.begin(), foci.end(), name);
    int index = 0;
    if (pos + 1 == foci.end()) {
        // If the next position is the end of the vector, 
        // the next element should be the first element
        index = 0;
    } else {
        // Get index of next body
        index = pos - foci.begin() + 1;
    };
    // Change title to the new planet 
    // (this is also how the information is passed to the draw call)
    current_center->SetTitle(foci[index].c_str());
    // Update the button to reflect the changes
    current_center->Draw();
}



void Main::drawStep() {
    /*
     * Draw a single step and pause simulation
     */
    timer->TurnOff();
    drawSingularStepLimit(limit);
    // Draw the axis
    view->ShowAxis();
}



void Main::drawRealTime() {
    /*
     * Draw continously and update simulation.
     * Disables rotation and axis.
     * Axies are disabled because the drawing will flicker
     * when enabling axis in real time.
     */
    // Using strings to get a changeable draw limit
    std::string tmp = "Main::drawSingularStepLimit(" + std::to_string(limit) + ")";
    // Copy the string to a char array pointer
    char *tmp_c = new char[tmp.length() + 1];
    std::strcpy(tmp_c, tmp.c_str());
    // Set the command to the newly made pointer
    timer->SetCommand(tmp_c);
    // Time interval for each run of the function
    timer->SetTime(10);
    timer->TurnOn();
}



void Main::drawAllAfter() {
    /*
     * Draw a total of 10'000 steps, then pause simulation
     */
    timer->TurnOff();
    n_steps = iteration + 10'000;
    if (iteration >= n_steps) {
        ;
    } else {
        while (iteration < n_steps) {
            for (pWithFunction &pStruct : planetPlotter) {
                // For every planet, in each iteration, add the current
                // position to the position vector
                body::Body curBod = mod.getBodyByName(pStruct.name);
                position p = curBod.getPos();
                pStruct.posArray.push_back(p);
            }
            // Do the iteration
            mod.iterate();
            iteration++;
            if (iteration % 1000 == 0) {
                std::cout << "Iteration " << iteration << std::endl;
            }
        }
        for (pWithFunction &pStruct : planetPlotter) {
            for (int i = 0; i < pStruct.posArray.size(); i++) {
                // Update the graph of each planet with the new positions
                position p = pStruct.posArray[i];
                pStruct.graph->SetPoint(i, p.x, p.y, p.z);
            }
        }
        // Draw the next step with a 10'000 limit
        drawSingularStepLimit(iteration);
        view->ShowAxis();
    }
}



int Main::main(int argc = 0, char *argv[] = {}, TApplication *app = nullptr) {
    /*
     * The main function.
     * Here all that is run only once happens. 
     * Takes in arguments that would normally be passed to a main.
     */

    if (argc > 1) {
        // If there was an input argument
        try {
            // Try and turn it to a number
            int size = std::stoi(argv[1]);
            canv = new TCanvas("PLOTTER", "Plotter", size, size);
            std::cout << "Size; " << size << std::endl;
            std::cout << "argc; " << argc << std::endl;
        } catch (std::invalid_argument) {
            // If it could not be turned to a number, print error
            std::cout << "Could not convert to integer.\n" 
                      << "Should be size in pixels of main canvas. Usage:\n" 
                      << "~$ ./Project.out 700\n"
                      << "will create a main window of size 700 x 700.\n"
                      << "Default is 900 x 900"
                      << std::endl;
            return 1;
        }
    } else {
        // Redefining them here allows TApplication to be passed
        // as an argument to the function
        canv = new TCanvas("PLOTTER", "Plotter", 900, 900);
    }
    controls = new TCanvas("CONTROLS", "Controls");
    timer = new TTimer(0);
    view = (TView3D*) TView::CreateView(1, 0, 0);

    
    // Init of vectors to store data in
    std::vector<std::vector<std::string>> tmpData, pNames;
    tmpData = readData();
    std::vector<std::vector<double>> planetaryData;
    planetaryData = transformData(tmpData);
    // pNames contain the name of the planet and the body it depends on
    pNames = tmpData;

    for (std::vector<std::string> s : pNames) {
        // Add the planets to the foci vector, to make them focusable
        foci.push_back(s[0]);
    }

    // Create buttons and draw them here
    initControlPanel();

    // Initiate a random number generator
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distrib(0.0, 2*3.1415);
    // Why auto here? Because the return type of bind is not easily defined,
    // as seen here:
    // https://en.cppreference.com/w/cpp/utility/functional/bind 
    // but it allows us to simply call randRotation() when we want a new number.
    auto randRotation = std::bind(distrib, generator);

    // These lines are also relatively inefficient, 
    // but again it is fine, because it only runs at startup
    for (int row = 0; row < pNames.size(); row++) {

        // For each row 
        // mass, aphelion, periphelion, avg orbital vel, radius, eccentricity, inclination
        double mass = planetaryData[row][0];
        // Setting the position along the x-axis to the aphelion / apoapsis of the body it depends on
        position pos = {planetaryData[row][1], 0, 0};
        
        // Rotation around Z axis
        double currentRotation = randRotation();
        // Rotation around the Y-axis must happen BEFORE rotation around the Z axis
        // Rotate inclination around Y
        pos = rotateAroundY(pos, planetaryData[row][6]);
        // Rotate position around Z by some random amount
        pos = rotateAroundZ(pos, currentRotation);

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
            // Only body that does not depend on another is the sun.
            // Adding asteroids or wild objects would also fit here.
            // Anything can be added to the system as long as it is defined in `planetaryData.csv`
            vel = {0, planetaryData[row][3], 0};
        } else {
            // Get important values to calculate minimum velocity
            double eccentricity = planetaryData[row][5];
            double dependant_mass = mod.getBodyByName(dependant_body).getMass();
            double semi_major = (planetaryData[row][2] + planetaryData[row][1]) / 2.0;
            double gravitational_parameter = GRAVITATIONAL * (mass + dependant_mass);
            double vel_min = sqrt( (1-eccentricity) * gravitational_parameter  /  ( (1+eccentricity) * semi_major ) );

            // Velocity needs to have the same rotation as the position
            vel = {0, vel_min, 0};
            // Again, we rotate around Y first
            vel = rotateAroundY(vel, planetaryData[row][6]);
            // By saving the rotation we used for the position, we can 
            // use it now when we rotate the velocity around as well
            vel = rotateAroundZ(vel, currentRotation);
        
            std::cout << "Planet: " << pNames[row][0] << " minimum velocity: " << vel_min << std::endl;
            // Add the velocity and position of the body it depends on to the current body,
            // so that they actually orbit around said planet
            vel = plu(mod.getBodyByName(dependant_body).getVel(), vel);
            pos = plu(mod.getBodyByName(dependant_body).getPos(), pos);
            std::cout << "Position: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
        }

        // Init body with the above found parameters, and add it to the model
        body::Body bodyToAdd = body::Body(pNames[row][0], mass, pos, vel, radius);
        mod.addBody(bodyToAdd);

        // Create a PolyLine, and a position vector, and 
        // add the body to the planetPlotter vector, so we
        // can draw it at a later time
        TPolyLine3D *curPlot = new TPolyLine3D();
        curPlot->SetLineWidth(5);
        curPlot->SetLineStyle(1);
        std::vector<position> pos_ = {};
        planetPlotter.push_back({pNames[row][0], pos_, curPlot});
    }

    // Change to the main canvas to draw a single step
    canv->cd();
    drawStep();
    // SetRealAspectRatio forces axes to have equal spacing,
    // but disables window scaling
    canv->SetRealAspectRatio();

    // Magic that connect the root canvas, or the canvas 
    // currently referenced to by `canv`, to the application itself.
    // This will close the program when the main window,
    // the one displaying the solar system, is closed
    TRootCanvas *rc = (TRootCanvas*) canv->GetCanvasImp();
    rc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");

    // Only runs in demo mode. Creates gifs, but is quite cost-heavy.
    char *demo = new char[] {'d','e','m','o'};
    if (argc > 2 && **(argv+2) == *demo) {

        // Make tails
        for (int iter = 0; iter < 600; iter++) {
            for (int jter = 0; jter < 200; jter++) {
                mod.iterate();
            }
            drawSingularStepLimit();
        }

        // Draw simply from solar system
        zoom_factor =  0.6;
        for (int iter = 0; iter < 200; iter++) {
            for (int jter = 0; jter < 200; jter++) {
                mod.iterate();
            }
            drawSingularStepLimit();
            view->ShowAxis();
            canv->Print("graphics/normalOrbit.gif+10");
        }
        canv->Print("graphics/normalOrbit.gif++10++");


        // Demonstrate focus change
        for (int iter = 0; iter < 200; iter++) {
            for (int jter = 0; jter < 20; jter++) {
                mod.iterate();
            }
            if (iter % 20 == 0) {
                changeFocus();
                zoomIn();
                zoomIn();
                zoomIn();
            }
            drawSingularStepLimit();
            view->ShowAxis();
            canv->Print("graphics/changeFocus.gif+5");
        }
        canv->Print("graphics/changeFocus.gif++5++");

        // Jupiter orbitals
        while (strcmp(current_center->GetTitle(), "jupiter") != 0) {
            changeFocus();
        }
        // Zoom in to jupiter and moons
        zoom_factor =  0.0005;
        // Rotate view to be flat
        view->RotateView(45, 90, 0);
        for (int iter = 0; iter < 200; iter++) {
            // Get rid of lines that appear after zooming
            drawSingularStepLimit();
        }
        view->RotateView(45, 90, 0);
        // Draw 50 times while flat
        for (int iter = 0; iter < 100; iter++) {
            mod.iterate();
            drawSingularStepLimit();
            view->ShowAxis();
            canv->Print("graphics/jupiterOrbit.gif+4");
        }
        double tilt = 90.0 / 199.0;
        // Slowly climb to top view
        for (int iter = 0; iter < 200; iter++) {
            mod.iterate();
            drawSingularStepLimit();
            view->RotateView(45, 90+iter*tilt, 0);
            view->ShowAxis();
            canv->Print("graphics/jupiterOrbit.gif+4");
        }
        // Draw 50 times from top viw
        for (int iter = 0; iter < 100; iter++) {
            mod.iterate();
            drawSingularStepLimit();
            view->ShowAxis();
            canv->Print("graphics/jupiterOrbit.gif+4");
        }
        canv->Print("graphics/jupiterOrbit.gif++10++");

        return 0;
    }

    // Equivelent to `app->run()`, but will not
    // crash when running in a ROOT session
    int winW = canv->GetWindowWidth();
    int winH = canv->GetWindowHeight();
    while (true) {
        gSystem->ProcessEvents();
        // To scale window when it changes in size
        if (canv->GetWindowWidth() != winW || canv->GetWindowHeight() != winH) {
            canv->SetCanvasSize(0, 0);
            canv->SetRealAspectRatio();
            winW = canv->GetWindowWidth();
            winH = canv->GetWindowHeight();
        }
    }
    return 0;
}



void run() {
    /*
     * A function that runs the main function without a TApplication.
     * Meant to be run from ROOT, and is therefore listed in linkdef.h
     */
    Main program;
    program.main();
}



void demo() {
    /*
     *
     */
    Main program;
    int argc = 3;
    char a = 'p'; 
    char *size = new char[] {'1','0','0','0'};
    char *demo = new char[] {'d','e','m','o'};
    char *argv[] = {&a, size, demo};
    program.main(argc, argv);
}


int main(int argc, char *argv[]) {
    /*
     * Main of the compiled program.
     * Creates a TApplication so that
     * the windows can be shown. 
     */
    Main program;
    // Create a TApplication only when running as compiled
    TApplication *app = new TApplication("Orbitals", 0, 0);
    int ret = program.main(argc, argv, app);
    app->Delete();
    return ret;
}

