/**
 * @file Validaciones.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Validaciones.h"
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <cstring>
#include <cctype> // para isdigit

using namespace std;

int Validaciones::ingresarEntero(char msj[50]) {
    char c;
    int i = 0;
    char dato[11];
    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (c >= '0' && c <= '9') {
            if (i < 10) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) {
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return atoi(dato);
}

float Validaciones::ingresarFlotante(char msj[50]) {
    char c;
    int i = 0;
    char dato[15];
    bool hayPunto = false;

    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (c >= '0' && c <= '9') {
            if (i < 14) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == '.' && !hayPunto && i > 0) {
            dato[i++] = c;
            cout << c;
            hayPunto = true;
        } else if (c == 8 && i > 0) {
            if (dato[i - 1] == '.') hayPunto = false;
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return atof(dato);
}

string Validaciones::ingresarCadena(char msj[50]) {
    char c;
    int i = 0;
    char dato[50];
    cout << msj << endl;
    
    do {
        i = 0;
        while ((c = getch()) != 13) {
            if (isalpha(c)) {
                if (i < 49) {
                    dato[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) {
                i--;
                cout << "\b \b";
            }
        }
        dato[i] = '\0';
        
    } while (i == 0);
    return string(dato);
}

char Validaciones::ingresarCaracter(char msj[50]) {
    char c;
    cout << msj << endl;

    while (true) {
        c = getch();
        if (isalpha(c)) {
            cout << c;
            return c;
        }
    }
}

string Validaciones::ingresarOperacion(char msj[50]) {
    char c;
    int i = 0;
    char dato[50];

    cout << msj << endl;

    while ((c = getch()) != 13) {
        if (isdigit(c) || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
            if (i < 49) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) {
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return string(dato);
}

string Validaciones::ingresarCedula(char msj[50]) {
    char c;
    int i = 0;
    char cedula[11];  // 10 digitos + '\0'

    do {
        system("cls");
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) {
            if (c >= '0' && c <= '9') {
                if (i < 10) {
                    cedula[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) {
                i--;
                cout << "\b \b";
            }
        }

        cedula[i] = '\0';

        if (!esCedulaValida(string(cedula))) {
            cout << "\nCedula no valida Intente de nuevo\n";
            system("pause");
        }

    } while (!esCedulaValida(string(cedula)));

    return string(cedula);
}

bool Validaciones::esCedulaValida(string cedula) {
    return validarLongitud(cedula) &&
           validarNumeros(cedula) &&
           validarDigitoVerificador(cedula);
}

bool Validaciones::validarLongitud(string cedula) {
    return cedula.length() == 10;
}

bool Validaciones::validarNumeros(string cedula) {
    for (char c : cedula) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool Validaciones::validarDigitoVerificador(string cedula) {
    int coeficientes[9] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
    int suma = 0;

    for (int i = 0; i < 9; ++i) {
        int val = (cedula[i] - '0') * coeficientes[i];
        if (val > 9) val -= 9;
        suma += val;
    }

    int verificador = (10 - (suma % 10)) % 10;
    return verificador == (cedula[9] - '0');
}
string Validaciones::ingresarNumeroTelefonico(char msj[50]) {
    char c;
    int i = 0;
    char numero[11]; // 10 digitos + '\0'

    do {
        system("cls");
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) {
            if (c >= '0' && c <= '9') {
                if (i < 10) {
                    numero[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) {
                i--;
                cout << "\b \b";
            }
        }

        numero[i] = '\0';

        if (i != 10 || numero[0] != '0'|| numero[1] != '9') {
            cout << "\nNumero invalido Debe tener 10 digitos y comenzar con 09\n";
            system("pause");
        }

    } while (i != 10 || numero[0] != '0'|| numero[1] != '9');

    return string(numero);
}
float  Validaciones::ingresarMonto(const char* mensaje) {
    char cad[20];
    char c;
    int i = 0;
    bool tienePunto = false;
    int decimales = 0;

    printf("%s", mensaje);

    while ((c = _getch()) != 13) { // Enter
        if (c >= '0' && c <= '9') {
            if (tienePunto) {
                if (decimales < 2) {
                    printf("%c", c);
                    cad[i++] = c;
                    decimales++;
                }
            } else {
                printf("%c", c);
                cad[i++] = c;
            }
        }
        else if (c == '.' && !tienePunto && i > 0) {
            printf("%c", c);
            cad[i++] = c;
            tienePunto = true;
            decimales = 0;
        }
        else if (c == 8 && i > 0) { // Backspace
            if (cad[i - 1] == '.') {
                tienePunto = false;
            } else if (tienePunto && decimales > 0) {
                decimales--;
            }
            printf("\b \b");
            i--;
        }
    }

    cad[i] = '\0';

    if (strlen(cad) == 0 || cad[0] == '.') {
        printf("\nEntrada invalida.\n");
        return ingresarMonto(mensaje); // Reintenta
    }

    float monto = atof(cad);

    if (monto > 10000.0f) {
        printf("\nEl monto no puede ser mayor a 10000.\n");
        return ingresarMonto(mensaje); // Reintenta
    }

    return monto;
}

string Validaciones::ingresarNumeros(char msj[50]) {
    char c;
    int i = 0;
    char dato[12]; // Ajusta el tamaño según la longitud máxima de tu ID

    cout << msj << endl;

    while ((c = getch()) != 13) { // Enter
        if (c >= '0' && c <= '9') {
            if (i < 12) {
                dato[i++] = c;
                cout << c;
            }
        } else if (c == 8 && i > 0) { // Backspace
            i--;
            cout << "\b \b";
        }
    }

    dato[i] = '\0';
    return string(dato);
}

string Validaciones::ingresarCodigoBak(char msj[50]) {
    char c;
    int i = 0;
    char dato[20];
    cout << msj << endl;

    do {
        i = 0;
        int guionBajo = 0;
        while ((c = getch()) != 13) { // Enter
            // Solo permite números en las posiciones 0-7 y 9-14
            if ((i < 8 || (i > 8 && i < 15)) && c >= '0' && c <= '9') {
                if (i < 19) {
                    dato[i++] = c;
                    cout << c;
                }
            }
            // Solo permite un '_' en la posición 8
            else if (i == 8 && c == '_') {
                dato[i++] = c;
                cout << c;
                guionBajo++;
            }
            // Permite borrar
            else if (c == 8 && i > 0) {
                if (dato[i-1] == '_') guionBajo--;
                i--;
                cout << "\b \b";
            }
            // Permite escribir ".bak" solo al final
            else if (i >= 15 && i < 19) {
                const char* bak = ".bak";
                if (c == bak[i - 15]) {
                    dato[i++] = c;
                    cout << c;
                }
            }
        }
        dato[i] = '\0';

        // Validación final de formato
        string strDato(dato);
        if (i == 19 && dato[8] == '_' && strDato.substr(15, 4) == ".bak") {
            return strDato;
        } else {
            cout << "\n\nFormato invalido. Ejemplo correcto: 20240602_153045.bak\n";
            i = 0;
        }
    } while (true);
}

string Validaciones::ingresarCorreo(char msj[50]) {
    char c;
    int i = 0;
    char correo[51]; // Máximo 50 caracteres + '\0'

    do {
        system("cls");
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) { // 13 = ENTER
            if (isalnum(c) || c == '@' || c == '.' || c == '-' || c == '_') {
                if (i < 50) {
                    correo[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) { // Backspace
                i--;
                cout << "\b \b";
            }
        }
        correo[i] = '\0';

        string correoStr = string(correo);

        // ===== Validaciones adicionales =====
        size_t arroba = correoStr.find('@');
        size_t punto = correoStr.find('.', arroba);

        bool tieneSoloUnArroba = (arroba != string::npos) && (correoStr.find('@', arroba + 1) == string::npos);
        bool puntoDespuesDeArroba = (punto != string::npos && punto > arroba + 1);
        bool puntoNoAlFinal = (punto != correoStr.length() - 1);
        bool noEmpiezaNiTerminaConPunto = (correoStr[0] != '.' && correoStr.back() != '.');
        bool sinPuntosConsecutivos = (correoStr.find("..") == string::npos);
        bool noHayPuntoJustoAntesDeArroba = (arroba > 0 && correoStr[arroba - 1] != '.');
        bool noHayPuntoJustoDespuesDeArroba = (arroba < correoStr.length() - 1 && correoStr[arroba + 1] != '.');

        if (!tieneSoloUnArroba ||
            !puntoDespuesDeArroba ||
            !puntoNoAlFinal ||
            !noEmpiezaNiTerminaConPunto ||
            !sinPuntosConsecutivos ||
            !noHayPuntoJustoAntesDeArroba ||
            !noHayPuntoJustoDespuesDeArroba) {
            cout << "\nCorreo invalido. Formato incorrecto.\n";
            system("pause");
        } else {
            return correoStr;
        }

    } while (true);
}
string Validaciones::ingresarTextoLibre(char msj[50]) {
    char c;
    int i = 0;
    char dato[50];
    cout << msj << endl;
    do {
        i = 0;
        while ((c = getch()) != 13) { // Enter
            if (isalnum(c)) { // Permite letras y números
                if (i < 49) {
                    dato[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) { // Backspace
                i--;
                cout << "\b \b";
            }
        }
        dato[i] = '\0';
    } while (i == 0);
    return string(dato);
}

string Validaciones::ingresarCodigoSucursal(char msj[50]) {
    char c;
    int i = 0;
    char codigo[4]; // Ajusta el tamaño según la longitud máxima de tu código
    
    do {
        
        i = 0;
        cout << msj << endl;

        while ((c = getch()) != 13) { // Enter
            if (isdigit(c)) { // Solo dígitos
                if (i < 3) {
                    codigo[i++] = c;
                    cout << c;
                }
            } else if (c == 8 && i > 0) { // Backspace
                i--;
                cout << "\b \b";
            }
        }
        codigo[i] = '\0';

        if (i != 3) { // Debe tener exactamente 3 caracteres
            cout << "\nCodigo invalido. Debe tener exactamente 3 digitos numericos.\n";
            system("pause");
        } else {
            return string(codigo);
        }
    } while (true);
}
float Validaciones:: ingresarCoordenada(char msj[50], bool esLatitud ) {
        char c;
        int i = 0;
        char dato[11]; // Máximo: -XX.XXXX (9 caracteres: signo, 2 dígitos, punto, 4 dígitos, '\0')
        bool haySigno = false;
        bool hayPunto = false;
        int digitosEnteros = 0;
        int digitosDecimales = 0;

        std::cout << msj << std::endl;

        while ((c = getch()) != 13 || i == 0) { // Evitar aceptar entrada vacía
            // Permitir signo negativo solo al inicio
            if (c == '-' && i == 0 && !haySigno) {
                dato[i++] = c;
                std::cout << c;
                haySigno = true;
            }
            // Permitir dígitos para la parte entera (máximo 2)
            else if (c >= '0' && c <= '9' && !hayPunto && digitosEnteros < 3) {
                if (i < 9) { // Asegurar espacio para el resto
                    dato[i++] = c;
                    std::cout << c;
                    digitosEnteros++;
                }
            }
            // Permitir punto decimal solo si hay al menos un dígito entero
            else if (c == '.' && !hayPunto && digitosEnteros > 0) {
                if (i < 9) {
                    dato[i++] = c;
                    std::cout << c;
                    hayPunto = true;
                }
            }
            // Permitir dígitos para la parte decimal (exactamente 4)
            else if (c >= '0' && c <= '9' && hayPunto && digitosDecimales < 4) {
                if (i < 9) {
                    dato[i++] = c;
                    std::cout << c;
                    digitosDecimales++;
                }
            }
            // Permitir backspace
            else if (c == 8 && i > 0) {
                i--;
                if (dato[i] == '.') {
                    hayPunto = false;
                    digitosDecimales = 0;
                } else if (dato[i] == '-') {
                    haySigno = false;
                } else if (hayPunto) {
                    digitosDecimales--;
                } else {
                    digitosEnteros--;
                }
                std::cout << "\b \b";
            }
            // Rechazar otros caracteres
            else {
                continue;
            }

            // Si se han ingresado 4 dígitos decimales, permitir Enter
            if (hayPunto && digitosDecimales == 4 && c == 13) {
                break;
            }
        }

        dato[i] = '\0';
        float valor = atof(dato);

        // Validar rango según si es latitud o longitud
        if (esLatitud) {
            if (valor < -90.0f || valor > 90.0f) {
                std::cout << "\nError: La latitud debe estar entre -90 y 90. Intente de nuevo.\n";
                system("pause");
                return ingresarCoordenada(msj, esLatitud); // Reintentar
            }
        } else {
            if (valor < -180.0f || valor > 180.0f) {
                std::cout << "\nError: La longitud debe estar entre -180 y 180. Intente de nuevo.\n";
                system("pause");
                return ingresarCoordenada(msj, esLatitud); // Reintentar
            }
        }

        return valor;
}
std::string Validaciones::ingresarNombreArchivo(const char* mensaje) {
    string entrada;
    bool valido = false;
    do {
        cout << mensaje;
        getline(cin, entrada);

        // Verificar que la entrada no esté vacía
        if (entrada.empty()) {
            cout << "\nEl nombre del archivo no puede estar vacio. Intente de nuevo.\n" << endl;
            continue;
        }

        // Verificar que contenga al menos un punto (para la extensión)
        if (entrada.find('.') == string::npos) {
            cout << "\nEl nombre del archivo debe incluir una extension (ejemplo: .txt o .md5). Intente de nuevo.\n" << endl;
            continue;
        }

        // Verificar que solo contenga caracteres permitidos: letras, números, puntos, guiones, guiones bajos
        bool caracteresValidos = true;
        for (char c : entrada) {
            if (!isalnum(c) && c != '.' && c != '-' && c != '_') {
                caracteresValidos = false;
                break;
            }
        }

        if (!caracteresValidos) {
            cout << "\nEl nombre del archivo contiene caracteres no permitidos. Use letras, numeros, puntos, guiones o guiones bajos. Intente de nuevo.\n" << endl;
            continue;
        }

        // Verificar que el nombre no sea demasiado largo (límite de 255 caracteres)
        if (entrada.length() > 255) {
            cout << "\nEl nombre del archivo es demasiado largo. Intente de nuevo.\n" << endl;
            continue;
        }

        valido = true;
    } while (!valido);

    return entrada;
}