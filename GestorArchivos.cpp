/**
 * @file GestorArchivos.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase GestorArchivos para manejo de archivos y hashes
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "GestorArchivos.h"
#include "GestorBusquedaMongo.h"
#include "NodoDoble.h"
#include "Persona.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <vector>

using namespace std;

GestorArchivos::GestorArchivos(TablaHash& hashTable, GestorBusquedaMongo* gestorMongo) 
    : hashes(hashTable), gestorBusquedaMongo(gestorMongo) {
}

GestorArchivos::~GestorArchivos() {
}

void GestorArchivos::setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo) {
    gestorBusquedaMongo = gestorMongo;
}

/**
 * @brief Guarda los titulares, sus cuentas bancarias y todos los movimientos en un archivo de texto.
 * Utiliza los datos completos desde MongoDB si hay conexion disponible.
 * 
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void GestorArchivos::guardarTitularesEnTxt(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GUARDAR TITULARES EN ARCHIVO TXT ---" << endl;

    // Obtener datos desde MongoDB si hay conexion disponible
    std::vector<Titular*> titularesCompletos;
    bool usandoDatosMongoDB = false;
    
    if (gestorBusquedaMongo) {
        cout << "\nObteniendo datos completos desde MongoDB..." << endl;
        titularesCompletos = gestorBusquedaMongo->obtenerTodosTitularesCompletos();
        
        if (!titularesCompletos.empty()) {
            usandoDatosMongoDB = true;
            cout << "Se obtuvieron " << titularesCompletos.size() << " titulares desde MongoDB." << endl;
        } else {
            cout << "No se pudieron obtener datos desde MongoDB. Usando datos locales." << endl;
        }
    }
    
    // Si no hay datos de MongoDB y la lista local esta vacia, no hay nada que guardar
    if (!usandoDatosMongoDB && titulares.vacia()) {
        cout << "\nNo hay titulares registrados para guardar.\n" << endl;
        system("pause");
        return;
    }

    ofstream archivo("titulares.txt");
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para escribir.\n" << endl;
        system("pause");
        return;
    }

    archivo << "=========================================" << endl;
    archivo << "       LISTADO COMPLETO DE TITULARES    " << endl;
    if (usandoDatosMongoDB) {
        archivo << "     (Datos obtenidos desde MongoDB)    " << endl;
    } else {
        archivo << "      (Datos de memoria local)          " << endl;
    }
    archivo << "=========================================" << endl << endl;

    int contadorTitular = 1;
    
    if (usandoDatosMongoDB) {
        // Procesar datos desde MongoDB
        for (Titular* titular : titularesCompletos) {
            procesarTitularParaArchivo(archivo, titular, contadorTitular);
            contadorTitular++;
        }
    } else {
        // Procesar datos desde lista local (codigo original)
        NodoDoble<Titular*>* actual = titulares.getCabeza();
        
        if (actual) {
            do {
                procesarTitularParaArchivo(archivo, actual->dato, contadorTitular);
                actual = actual->siguiente;
                contadorTitular++;
            } while (actual != titulares.getCabeza());
        }
    }

    archivo.close();
    cout << "\nTitulares, cuentas y movimientos guardados exitosamente en 'titulares.txt'.\n" << endl;

    // Generar y guardar el hash MD5
    generarYGuardarHashMD5();
    
    system("pause");
}

/**
 * @brief Procesa un titular individual y escribe su informacion al archivo
 * 
 * @param archivo Stream del archivo donde escribir
 * @param titular Puntero al titular a procesar
 * @param numeroTitular Numero del titular en la secuencia
 */
void GestorArchivos::procesarTitularParaArchivo(ofstream& archivo, Titular* titular, int numeroTitular) {
    Persona persona = titular->getPersona();

    cout << "Guardando titular: " << persona.getNombre() << " " << persona.getApellido() << endl;

    // Informacion del titular
    archivo << "TITULAR #" << numeroTitular << endl;
    archivo << "=========================================" << endl;
    archivo << "CI: " << persona.getCI() << endl;
    archivo << "Nombre Completo: " << persona.getNombre() << " " << persona.getApellido() << endl;
    archivo << "Telefono: " << persona.getTelefono() << endl;
    archivo << "Correo: " << persona.getCorreo() << endl;
    archivo << "Fecha de Nacimiento: " << persona.getFechaNa().getDia() << "/" 
            << persona.getFechaNa().getMes() << "/" << persona.getFechaNa().getAnio().getAnio() << endl;
    archivo << endl;

    // Procesar cuenta corriente
    procesarCuentaCorrienteParaArchivo(archivo, titular);
    
    // Procesar cuentas de ahorro
    procesarCuentasAhorroParaArchivo(archivo, titular);

    archivo << "=========================================" << endl << endl;
}

/**
 * @brief Procesa la cuenta corriente de un titular y escribe la informacion al archivo
 * 
 * @param archivo Stream del archivo donde escribir
 * @param titular Puntero al titular
 */
void GestorArchivos::procesarCuentaCorrienteParaArchivo(ofstream& archivo, Titular* titular) {
    CuentaBancaria* cuentaCorriente = titular->getCuentaCorriente();
    if (cuentaCorriente) {
        Persona persona = titular->getPersona();
        cout << "Guardando cuenta corriente y movimientos de " << persona.getNombre() << endl;
        
        archivo << "--- CUENTA CORRIENTE ---" << endl;
        archivo << "ID Cuenta: " << cuentaCorriente->getID() << endl;
        archivo << "Tipo: " << cuentaCorriente->getTipoCuenta() << endl;
        archivo << "Saldo Actual: $" << cuentaCorriente->getSaldo() << endl;
        
        // Procesar movimientos de cuenta corriente
        procesarMovimientosParaArchivo(archivo, cuentaCorriente->getMovimientos());
        archivo << endl;
    } else {
        archivo << "--- SIN CUENTA CORRIENTE ---" << endl << endl;
    }
}

/**
 * @brief Procesa las cuentas de ahorro de un titular y escribe la informacion al archivo
 * 
 * @param archivo Stream del archivo donde escribir
 * @param titular Puntero al titular
 */
void GestorArchivos::procesarCuentasAhorroParaArchivo(ofstream& archivo, Titular* titular) {
    NodoDoble<CuentaBancaria*>* nodoAhorro = titular->getCuentasAhorro().getCabeza();
    if (nodoAhorro) {
        Persona persona = titular->getPersona();
        cout << "Guardando cuentas de ahorro y movimientos de " << persona.getNombre() << endl;
        
        int contadorAhorro = 1;
        do {
            CuentaBancaria* cuentaAhorro = nodoAhorro->dato;
            archivo << "--- CUENTA DE AHORRO #" << contadorAhorro << " ---" << endl;
            archivo << "ID Cuenta: " << cuentaAhorro->getID() << endl;
            archivo << "Tipo: " << cuentaAhorro->getTipoCuenta() << endl;
            archivo << "Saldo Actual: $" << cuentaAhorro->getSaldo() << endl;
            
            // Procesar movimientos de cuenta de ahorro
            procesarMovimientosParaArchivo(archivo, cuentaAhorro->getMovimientos());
            archivo << endl;
            
            nodoAhorro = nodoAhorro->siguiente;
            contadorAhorro++;
        } while (nodoAhorro != titular->getCuentasAhorro().getCabeza());
    } else {
        archivo << "--- SIN CUENTAS DE AHORRO ---" << endl << endl;
    }
}

/**
 * @brief Procesa los movimientos de una cuenta y los escribe al archivo
 * 
 * @param archivo Stream del archivo donde escribir
 * @param movimientos Lista de movimientos de la cuenta
 */
void GestorArchivos::procesarMovimientosParaArchivo(ofstream& archivo, ListaDobleCircular<Movimiento*>& movimientos) {
    if (!movimientos.vacia()) {
        archivo << "MOVIMIENTOS:" << endl;
        NodoDoble<Movimiento*>* nodoMov = movimientos.getCabeza();
        int numMov = 1;
        do {
            Movimiento* mov = nodoMov->dato;
            archivo << "  " << numMov << ". ID: " << mov->getIDMovimiento() 
                    << " | Fecha: " << mov->getFechaMov().getDia() << "/" 
                    << mov->getFechaMov().getMes() << "/" << mov->getFechaMov().getAnio().getAnio()
                    << " | Tipo: " << (mov->getTipo() ? "DEPOSITO" : "RETIRO")
                    << " | Monto: $" << mov->getMonto() << endl;
            nodoMov = nodoMov->siguiente;
            numMov++;
        } while (nodoMov != movimientos.getCabeza());
    } else {
        archivo << "Sin movimientos registrados." << endl;
    }
}

/**
 * @brief Genera y guarda el hash MD5 del archivo de titulares
 */
void GestorArchivos::generarYGuardarHashMD5() {
    string hash = generarHashMD5("titulares.txt");
    if (!hash.empty()) {
        // Guardar en la tabla hash
        hashes.insertar("titulares.txt", hash);
        cout << "\nHash MD5 almacenado en la tabla hash para 'titulares.txt'.\n" << endl;

        // Guardar en el archivo de hash como respaldo
        ofstream hashFile("titulares.txt.md5");
        if (hashFile) {
            hashFile << hash;
            hashFile.close();
            cout << "\nHash MD5 guardado en 'titulares.txt.md5'.\n" << endl;
        } else {
            cout << "\nNo se pudo guardar el archivo de hash.\n" << endl;
        }
    } else {
        cout << "\nNo se pudo generar el hash MD5.\n" << endl;
    }
}

/**
 * @brief Genera un archivo PDF completo con los titulares, cuentas y movimientos registrados.
 * 
 */
void GestorArchivos::generarPDFTitulares() {
    system("cls");
    cout << "\n--- GENERAR PDF COMPLETO DE TITULARES ---\n" << endl;

    ifstream archivo("titulares.txt");
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo 'titulares.txt'." << endl;
        cout << "Asegurese de guardar primero los titulares en archivo TXT.\n" << endl;
        system("pause");
        return;
    }

    string outputFile = "titulares_completo.pdf";
    ofstream pdf(outputFile, ios::binary);
    if (!pdf) {
        cout << "\nNo se pudo crear el archivo PDF.\n" << endl;
        archivo.close();
        system("pause");
        return;
    }

    cout << "Generando PDF con informacion completa de titulares, cuentas y movimientos..." << endl;

    // Parámetros de página
    const float pageTop = 800;
    const float pageBottom = 50;
    const float lineSpacing = 12;

    std::vector<std::string> paginas; // Cada página PDF (solo el contenido del stream)
    std::stringstream paginaActual;

    // Título principal (lo agregamos en cada página)
    auto agregarTitulo = [&paginaActual]() {
        paginaActual << "BT\n";
        paginaActual << "/F2 16 Tf\n1 0 0 1 50 800 Tm\n(SISTEMA BANCARIO MICHIBANK) Tj\n";
        paginaActual << "0 -20 Td\n";
        paginaActual << "/F2 14 Tf\n(Listado Completo de Titulares, Cuentas y Movimientos) Tj\n";
        paginaActual << "0 -10 Td\n";
        paginaActual << "/F1 10 Tf\n(Fecha de generacion: " << __DATE__ << " " << __TIME__ << ") Tj\n";
    };

    agregarTitulo();
    float yPos = pageTop - 42;

    string linea;
    while (getline(archivo, linea)) {
        if (yPos < pageBottom) {
            paginaActual << "ET\n";
            paginas.push_back(paginaActual.str());
            paginaActual.str("");
            paginaActual.clear();
            agregarTitulo();
            yPos = pageTop - 42;
        }

        // Escapar caracteres especiales para PDF
        string escapedLine = linea;
        for (size_t i = 0; i < escapedLine.size(); ++i) {
            if (escapedLine[i] == '(' || escapedLine[i] == ')' || escapedLine[i] == '\\') {
                escapedLine.insert(i, "\\");
                ++i;
            }
        }

        // Determinar el tipo de línea y aplicar formato apropiado
        if (escapedLine.find("TITULAR #") != string::npos) {
            paginaActual << "0 -" << lineSpacing * 2 << " Td\n"
                         << "/F2 12 Tf\n"
                         << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing * 2;
        } else if (escapedLine.find("CUENTA CORRIENTE") != string::npos ||
                   escapedLine.find("CUENTA DE AHORRO") != string::npos) {
            paginaActual << "0 -" << lineSpacing * 1.5 << " Td\n"
                         << "/F2 10 Tf\n"
                         << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing * 1.5;
        } else if (escapedLine.find("MOVIMIENTOS:") != string::npos) {
            paginaActual << "0 -" << lineSpacing << " Td\n"
                         << "/F2 9 Tf\n"
                         << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing;
        } else if (escapedLine.find("  ") == 0 && escapedLine.find(". ID:") != string::npos) {
            paginaActual << "0 -" << lineSpacing * 0.8 << " Td\n"
                         << "/F1 8 Tf\n"
                         << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing * 0.8;
        } else if (!escapedLine.empty() && escapedLine.find("===") == string::npos) {
            paginaActual << "0 -" << lineSpacing << " Td\n"
                         << "/F1 9 Tf\n"
                         << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing;
        } else if (escapedLine.find("===") != string::npos) {
            paginaActual << "0 -" << lineSpacing * 0.5 << " Td\n";
            yPos -= lineSpacing * 0.5;
        }
    }

    // Agregar la última página si tiene contenido
    if (paginaActual.str().size() > 0) {
        paginaActual << "ET\n";
        paginas.push_back(paginaActual.str());
    }

    archivo.close();

    // Calcular el número total de objetos
    int totalObjs = 4 + static_cast<int>(paginas.size()) * 2;

    // Vector para almacenar offsets de objetos (índice 0 no usado)
    std::vector<long long> offsets(totalObjs + 1, 0);

    // Escribir cabecera
    pdf << "%PDF-1.4\n";

    // Objeto 1: Catalog
    offsets[1] = static_cast<long long>(pdf.tellp());
    pdf << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";

    // Objeto 2: Pages (con Kids corregido)
    offsets[2] = static_cast<long long>(pdf.tellp());
    pdf << "2 0 obj\n<< /Type /Pages /Kids [";
    int pageObjNum = 6; // Primer objeto de página (contenido 5, página 6; contenido 7, página 8, etc.)
    for (size_t i = 0; i < paginas.size(); ++i) {
        pdf << pageObjNum << " 0 R ";
        pageObjNum += 2;
    }
    pdf << "] /Count " << paginas.size() << " >>\nendobj\n";

    // Objeto 3: Font Helvetica
    offsets[3] = static_cast<long long>(pdf.tellp());
    pdf << "3 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n";

    // Objeto 4: Font Helvetica-Bold
    offsets[4] = static_cast<long long>(pdf.tellp());
    pdf << "4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>\nendobj\n";

    // Escribir objetos de contenido y página para cada página
    int objNum = 5;
    for (size_t i = 0; i < paginas.size(); ++i) {
        std::string streamContent = paginas[i];
        size_t longitudReal = streamContent.length();

        // Objeto de contenido
        offsets[objNum] = static_cast<long long>(pdf.tellp());
        pdf << objNum << " 0 obj\n"
            << "<< /Length " << longitudReal << " >>\n"
            << "stream\n"
            << streamContent
            << "endstream\nendobj\n";

        // Objeto de página
        offsets[objNum + 1] = static_cast<long long>(pdf.tellp());
        pdf << (objNum + 1) << " 0 obj\n"
            << "<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 3 0 R /F2 4 0 R >> >> "
            << "/MediaBox [0 0 595 842] /Contents " << objNum << " 0 R >>\nendobj\n";

        objNum += 2;
    }

    // Escribir tabla de referencias cruzadas
    long long xref_offset = static_cast<long long>(pdf.tellp());
    pdf << "xref\n0 " << (totalObjs + 1) << "\n";
    pdf << "0000000000 65535 f \n";
    for (int i = 1; i <= totalObjs; ++i) {
        pdf << std::setw(10) << std::setfill('0') << offsets[i] << " 00000 n \n";
    }

    // Trailer
    pdf << "trailer\n<< /Size " << (totalObjs + 1) << " /Root 1 0 R >>\n";
    pdf << "startxref\n" << xref_offset << "\n%%EOF\n";

    pdf.close();

    cout << "\nPDF completo generado exitosamente: " << outputFile << endl;
    cout << "El archivo contiene informacion detallada de:" << endl;
    cout << "- Datos personales de titulares" << endl;
    cout << "- Informacion de cuentas corrientes y de ahorro" << endl;
    cout << "- Historial completo de movimientos por cuenta" << endl;
    cout << "- Fechas y montos de todas las transacciones\n" << endl;

    system("pause");
}

/**
 * @brief Genera un hash MD5 simple del contenido de un archivo.
 * 
 * @param nombreArchivo Nombre del archivo para calcular el hash
 * @return std::string Hash MD5 generado
 */
std::string GestorArchivos::generarHashMD5(const std::string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo para calcular el hash.\n" << endl;
        return "";
    }

    // Leer todo el contenido del archivo
    string contenido;
    char c;
    while (archivo.get(c)) {
        contenido += c;
    }
    archivo.close();

    // Calculo de un hash simple (simulacion de MD5)
    unsigned long hash = 0;
    for (char ch : contenido) {
        hash += (unsigned char)ch;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    // Convertir a hexadecimal (32 caracteres)
    stringstream ss;
    ss << hex << setw(32) << setfill('0') << hash;
    string resultado = ss.str().substr(0, 32);
    return resultado;
}

/**
 * @brief Compara el hash MD5 actual de un archivo con el hash almacenado.
 * 
 * @param nombreArchivo Nombre del archivo a verificar
 * @return true Si los hashes coinciden
 * @return false Si los hashes no coinciden o hay error
 */
bool GestorArchivos::compararHashArchivo(const std::string& nombreArchivo) {
    // Generar el hash MD5 actual del archivo de texto
    string hashActual = generarHashMD5(nombreArchivo);
    if (hashActual.empty()) {
        cout << "\nNo se pudo calcular el hash del archivo.\n" << endl;
        return false;
    }

    // Buscar el hash en la tabla hash
    string hashAlmacenado;
    if (!hashes.buscar(nombreArchivo, hashAlmacenado)) {
        cout << "\nNo se encontro un hash registrado para '" << nombreArchivo << "' en la tabla hash.\n" << endl;
        return false;
    }

    // Comparar los hashes
    bool coincide = hashActual == hashAlmacenado;
    if (coincide) {
        cout << "\nHash actual: " << hashActual << "\nHash almacenado: " << hashAlmacenado << endl;
    } else {
        cout << "\nHash actual: " << hashActual << "\nHash almacenado: " << hashAlmacenado << "\nLos hashes no coinciden.\n";
    }
    return coincide;
}
