#define WIN32_LEAN_AND_MEAN
#include "Sistema.h"
#include "Marquesina.h"
#include "Menu.h"
#include <iostream>

int main() {
    Marquesina marquesina;
    marquesina.iniciar("BIENVENIDOS A MICHIBANK");

    // Menu principal de conexion
    Menu menu;
    const char* opciones[] = {
        "Ser cliente",
        "Ser Administrador",
        "Salir"
    };

    int opcion;
    do {
        opcion = menu.ingresarMenu("MICHIBANK - RED TCP", opciones, 3);
        
        Sistema sistema;
        
        switch(opcion) {
            case 1:
                std::cout << "\n=== MODO CLIENTE ===" << std::endl;
                sistema.configurarModoCliente();
                sistema.menuPrincipal();
                break;
                
            case 2:
                std::cout << "\n=== MODO ADMINISTRADOR ===" << std::endl;
                sistema.configurarModoAdministrador();
                break;
                
            case 3:
                std::cout << "Saliendo del programa..." << std::endl;
                break;
                
            default:
                std::cout << "Opcion no valida." << std::endl;
                break;
        }
        
    } while(opcion != 3);

    marquesina.detener();
    return 0;
}

