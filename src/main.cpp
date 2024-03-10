#include <TCanvas.h>
#include <TGraph2D.h>

#include <cmath>
#include <cstdio>
#include <iostream>

#include "model.hpp"
#include "body.hpp"
#include "types.hpp"
#include "constants.hpp"

int main() {
    // Create a model and add 2 bodies
    // The sun
    body::Body sun = body::Body(1.9885 * pow(10, 30), {0, 0, 0});
    // Earth-like body. Mass is kg, distance is km
    body::Body earthLike = body::Body(5.972168 * pow(10, 24), {149'598'023'000, 0, 0}, {1000, 0, 0});
    sun.setName("Sun");
    earthLike.setName("Earth");
    
    double deltaT = 60 * 60; // Seconds. Here an hour at a time 
    // Init model
    model::Model mod = model::Model(deltaT);
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
    TCanvas *c1 = new TCanvas("Apple", "Bees", 600, 600);
    TGraph2D *sunPlot = new TGraph2D();
    TGraph2D *earthPlot = new TGraph2D();
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

    earthPlot->Draw("LINE PCOL");
    sunPlot->Draw("SAME PCOL");
    for (int i = 0; i < 20; i+=5) {
        std::cout << "Sun x; " << sunx[i] << "  Earth x; " << earthx[i] << std::endl; 
    }

    return 0;
}
