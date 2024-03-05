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
    body::Body earthLike = body::Body(5.972168 * pow(10, 24), {149'598'023, 0, 0});
    sun.setName("Sun");
    earthLike.setName("Earth");
    
    double deltaT = 60 * 60 * 24 * 100; // Seconds. Here a day at a time 
    // Init model
    model::Model mod = model::Model(deltaT);
    // Add bodies to model
    mod.addBody(sun);
    mod.addBody(earthLike);

    int n_steps = 20000;
    double simtime[n_steps];
    double sunx[n_steps];
    double suny[n_steps];
    double earthx[n_steps];
    double earthy[n_steps];

    simtime[0] = 0;
    sunx[0] = 0;
    suny[0] = 0;
    earthx[0] = earthLike.getPos().x;
    earthy[0] = 0;

    double time;
    TCanvas *c1 = new TCanvas("1", "1", 600, 600);
    TGraph2D *sunPlot = new TGraph2D();
    TGraph2D *earthPlot = new TGraph2D();

    std::cout << sunPlot << std::endl;
    std::cout << earthPlot << std::endl;

    for (int i = 1; i < n_steps; i++) {
        simtime[i] = mod.iterate();
        sun = mod.getBodyByName("Sun");
        earthLike = mod.getBodyByName("Earth");
        sunPlot->SetPoint(i, sun.getPos().x, sun.getPos().y, simtime[i]);
        earthPlot->SetPoint(i, earthLike.getPos().x, earthLike.getPos().y, simtime[i]);
        sunx[i] = sun.getPos().x;
        suny[i] = sun.getPos().y;
        earthx[i] = earthLike.getPos().x;
        earthy[i] = earthLike.getPos().y;
        std::cout << earthLike.getAcc().x << std::endl;
    }

    earthPlot->Draw("P");
    sunPlot->Draw("SAME P");
    for (int i = 0; i < 10; i++) {
        std::cout << "Sun x; " << sunx[i] << "  Earth x; " << earthx[i] << std::endl; 
    }

    return 0;
}
