#include <iostream>

namespace OopBootstrap {

void printMigrationStatus() {
    std::cout << "[OOP] Nuevo entrypoint creado: src/main_oop.cpp\n";
    std::cout << "[OOP] src/main.cpp queda como legacy temporal\n";
    std::cout << "[OOP] Este archivo sera el punto de inicio de la "
                 "arquitectura nueva\n";
}

} // namespace OopBootstrap
