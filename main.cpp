#include "Sistema.h"
#include "Marquesina.h"

int main() {
    Marquesina marquesina;
    marquesina.iniciar("BIENVENIDOS A MICHIBANK");

    Sistema sistema;
    sistema.menuPrincipal();

    marquesina.detener();
    return 0;
}

