#include <iostream>
#include "Application.h"
#include "Exceptions.h"

int main() {
    try {
        Application app;
        app.run();
    } catch (const GameException& error) {
        std::cerr << "Eroare de joc: " << error.what() << "\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Eroare neasteptata: " << error.what() << "\n";
        return 2;
    }
    return 0;
}