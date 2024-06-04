#include <TApplication.h>
#include <TCanvas.h>
#include <TButton.h>

#include <iostream>

#include "main.hpp"

//TCanvas *Main::canv = new TCanvas();

void Main::someFunc() {
    std::cout << "Pressed" << std::endl;
}

int main() {
    TApplication *app = new TApplication("App", 0, 0);
    TCanvas *canv = new TCanvas();
    canv->cd();
    TButton *but = new TButton("Press", "Main::someFunc()", 0.1, 0.1, 0.9, 0.9);
    but->Draw();
    app->Run();
    
}


