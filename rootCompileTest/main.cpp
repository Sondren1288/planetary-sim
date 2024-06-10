#include <TApplication.h>
#include <TCanvas.h>
#include <TButton.h>
#include <TSystem.h>
#include <TList.h>

#include <iostream>

#include "main.hpp"

//TCanvas *Main::canv = new TCanvas();

void Main::someFunc() {
    std::cout << "Pressed" << std::endl;
}

void someFunc() {
    std::cout << "Other pressed" << std::endl;
}

void Main::run(TApplication *app = nullptr) {
    
    TCanvas *canv = new TCanvas();
    canv->cd();
    TButton *but = new TButton("P1", "Main::someFunc()", 0.1, 0.1, 0.45, 0.9);
    TButton *but2 = new TButton("P2", "someFunc()", 0.55, 0.1, 0.9, 0.9);
    but->Draw();
    but2->Draw();
    while (1) {
        gSystem->ProcessEvents();
    }
    if (nullptr != app) {
        app->Run();
    }
}

void run() {
    Main::run();
};

int main() {
    TApplication *app = new TApplication("App", 0, 0);
    Main::run(app);
    return 0;
}

