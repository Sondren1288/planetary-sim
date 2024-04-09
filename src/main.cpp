#include <TCanvas.h>
#include <TGraph2D.h>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>

#include "model.hpp"
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"



struct pWithFunction {
    std::string name;
    std::vector<position> posArray;
    TGraph2D *graph;
};


std::vector<std::vector<std::string>> readData() {
    std::ifstream data("planetaryData.txt");

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
        // Name, mass, aphelion, periphelion, avg orbital vel, radius, eccentricity
        
        // Add the name to the objectColumn
        std::vector<std::string> name = {column[0]};
        objectColumn.push_back(name);

        // Create empty vector to store doubles
        // Could have used doubleVec.clear, but
        // the amount of times this is going to be
        // run makes this insignificant
        std::vector<double> doubleVec;
        // Flag to drop the first column
        bool dropFlag = true;
        for (std::string data_ : column) {
            // Drop the first column
            if (dropFlag) {
                dropFlag = false;
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


int main() {
    std::vector<std::vector<std::string>> tmpData, pNames;
    tmpData = readData();
    std::vector<std::vector<double>> planetaryData;
    planetaryData = transformData(tmpData);
    pNames = tmpData;
    // TODO some magic with aphelion and parahelion to
    // calculate position and velocity at this point


    // Init drawer here
    TCanvas *c1 = new TCanvas("Apple", "Bees", 6000, 6000);
    std::vector<pWithFunction> planetPlotter;


    // Init model to be able to add planets
    double deltaT = 60 * 60; // Seconds. Here an hour at a time 
    model::Model mod = model::Model(deltaT);

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

        TGraph2D *curPlot = new TGraph2D();
        std::vector<position> pos_ = {pos};
        planetPlotter.push_back({pNames[row][0], pos_, curPlot});
    }

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
    c1->Divide(2,2);
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
    // Uses the name of the canvas to create the pads
    TPad *pad_1 = (TPad*) c1->GetPrimitive("Apple_1");
    TPad *pad_2 = (TPad*) c1->GetPrimitive("Apple_2");
    TPad *pad_3 = (TPad*) c1->GetPrimitive("Apple_3");
    TPad *pad_4 = (TPad*) c1->GetPrimitive("Apple_4");

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
