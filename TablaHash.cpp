#include "TablaHash.h"
#include <iostream>

using namespace std;
/**
 * @brief Clase que representa una tabla hash para almacenar archivos y sus hashes MD5.
 * 
 */
void TablaHash::mostrarContenido() const {
    cout << "\n--- CONTENIDO DE LA TABLA HASH ---\n" << endl;
    bool vacia = true;
    for (size_t i = 0; i < tabla.size(); ++i) {
        if (!tabla[i].empty()) {
            vacia = false;
            cout << "Indice " << i << ":\n";
            for (const auto& par : tabla[i]) {
                cout << "  Archivo: " << par.first << " -> Hash MD5: " << par.second << "\n";
            }
        }
    }
    if (vacia) {
        cout << "La tabla hash esta vacia.\n" << endl;
    }
}