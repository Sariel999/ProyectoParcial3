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
#include "NodoDoble.h"
#include "Persona.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

GestorArchivos::GestorArchivos(TablaHash& hashTable) : hashes(hashTable) {
}

GestorArchivos::~GestorArchivos() {
}

/**
 * @brief Guarda los titulares, sus cuentas bancarias y todos los movimientos en un archivo de texto.
 * 
 * @param titulares Lista de titulares del sistema
 */
void GestorArchivos::guardarTitularesEnTxt(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GUARDAR TITULARES EN ARCHIVO TXT ---" << endl;

    if (titulares.vacia()) {
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
    archivo << "=========================================" << endl << endl;

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    int contadorTitular = 1;
    
    if (actual) {
        do {
            Titular* titular = actual->dato;
            Persona persona = titular->getPersona();

            cout << "Guardando titular: " << persona.getNombre() << " " << persona.getApellido() << endl;

            // Información del titular
            archivo << "TITULAR #" << contadorTitular << endl;
            archivo << "=========================================" << endl;
            archivo << "CI: " << persona.getCI() << endl;
            archivo << "Nombre Completo: " << persona.getNombre() << " " << persona.getApellido() << endl;
            archivo << "Telefono: " << persona.getTelefono() << endl;
            archivo << "Correo: " << persona.getCorreo() << endl;
            archivo << "Fecha de Nacimiento: " << persona.getFechaNa().getDia() << "/" 
                    << persona.getFechaNa().getMes() << "/" << persona.getFechaNa().getAnio().getAnio() << endl;
            archivo << endl;

            // Cuenta corriente y sus movimientos
            CuentaBancaria* cuentaCorriente = titular->getCuentaCorriente();
            if (cuentaCorriente) {
                cout << "Guardando cuenta corriente y movimientos de " << persona.getNombre() << endl;
                archivo << "--- CUENTA CORRIENTE ---" << endl;
                archivo << "ID Cuenta: " << cuentaCorriente->getID() << endl;
                archivo << "Tipo: " << cuentaCorriente->getTipoCuenta() << endl;
                archivo << "Saldo Actual: $" << cuentaCorriente->getSaldo() << endl;
                
                // Movimientos de cuenta corriente
                ListaDobleCircular<Movimiento*>& movimientos = cuentaCorriente->getMovimientos();
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
                archivo << endl;
            } else {
                archivo << "--- SIN CUENTA CORRIENTE ---" << endl << endl;
            }

            // Cuentas de ahorro y sus movimientos
            NodoDoble<CuentaBancaria*>* nodoAhorro = titular->getCuentasAhorro().getCabeza();
            if (nodoAhorro) {
                cout << "Guardando cuentas de ahorro y movimientos de " << persona.getNombre() << endl;
                int contadorAhorro = 1;
                do {
                    CuentaBancaria* cuentaAhorro = nodoAhorro->dato;
                    archivo << "--- CUENTA DE AHORRO #" << contadorAhorro << " ---" << endl;
                    archivo << "ID Cuenta: " << cuentaAhorro->getID() << endl;
                    archivo << "Tipo: " << cuentaAhorro->getTipoCuenta() << endl;
                    archivo << "Saldo Actual: $" << cuentaAhorro->getSaldo() << endl;
                    
                    // Movimientos de cuenta de ahorro
                    ListaDobleCircular<Movimiento*>& movimientosAhorro = cuentaAhorro->getMovimientos();
                    if (!movimientosAhorro.vacia()) {
                        archivo << "MOVIMIENTOS:" << endl;
                        NodoDoble<Movimiento*>* nodoMovAhorro = movimientosAhorro.getCabeza();
                        int numMovAhorro = 1;
                        do {
                            Movimiento* movAhorro = nodoMovAhorro->dato;
                            archivo << "  " << numMovAhorro << ". ID: " << movAhorro->getIDMovimiento() 
                                    << " | Fecha: " << movAhorro->getFechaMov().getDia() << "/" 
                                    << movAhorro->getFechaMov().getMes() << "/" << movAhorro->getFechaMov().getAnio().getAnio()
                                    << " | Tipo: " << (movAhorro->getTipo() ? "DEPOSITO" : "RETIRO")
                                    << " | Monto: $" << movAhorro->getMonto() << endl;
                            nodoMovAhorro = nodoMovAhorro->siguiente;
                            numMovAhorro++;
                        } while (nodoMovAhorro != movimientosAhorro.getCabeza());
                    } else {
                        archivo << "Sin movimientos registrados." << endl;
                    }
                    archivo << endl;
                    
                    nodoAhorro = nodoAhorro->siguiente;
                    contadorAhorro++;
                } while (nodoAhorro != titular->getCuentasAhorro().getCabeza());
            } else {
                archivo << "--- SIN CUENTAS DE AHORRO ---" << endl << endl;
            }

            archivo << "=========================================" << endl << endl;
            actual = actual->siguiente;
            contadorTitular++;
        } while (actual != titulares.getCabeza());
    }

    archivo.close();
    cout << "\nTitulares, cuentas y movimientos guardados exitosamente en 'titulares.txt'.\n" << endl;

    // Generar y guardar el hash MD5
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

    system("pause");
}

/**
 * @brief Genera un archivo PDF completo con los titulares, cuentas y movimientos registrados.
 * 
 */
void GestorArchivos::generarPDFTitulares() {
    system("cls");
    cout << "\n--- GENERAR PDF COMPLETO DE TITULARES ---\n" << endl;

    // Verificar que existe el archivo de titulares
    ifstream archivo("titulares.txt");
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo 'titulares.txt'." << endl;
        cout << "Asegurese de guardar primero los titulares en archivo TXT.\n" << endl;
        system("pause");
        return;
    }

    // Archivo PDF de salida
    string outputFile = "titulares_completo.pdf";
    ofstream pdf(outputFile, ios::binary);
    if (!pdf) {
        cout << "\nNo se pudo crear el archivo PDF.\n" << endl;
        archivo.close();
        system("pause");
        return;
    }

    cout << "Generando PDF con informacion completa de titulares, cuentas y movimientos..." << endl;

    // Escribir cabecera del PDF
    pdf << "%PDF-1.4\n";

    // Objeto 1: Catálogo
    pdf << "1 0 obj\n"
        << "<< /Type /Catalog /Pages 2 0 R >>\n"
        << "endobj\n";

    // Objeto 2: Páginas
    pdf << "2 0 obj\n"
        << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
        << "endobj\n";

    // Objeto 3: Página
    pdf << "3 0 obj\n"
        << "<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R /F2 5 0 R >> >> /MediaBox [0 0 595 842] /Contents 6 0 R >>\n"
        << "endobj\n";

    // Objeto 4: Fuente normal
    pdf << "4 0 obj\n"
        << "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\n"
        << "endobj\n";

    // Objeto 5: Fuente negrita
    pdf << "5 0 obj\n"
        << "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>\n"
        << "endobj\n";

    // Objeto 6: Contenido
    stringstream contenido;
    contenido << "6 0 obj\n"
              << "<< /Length ";
    
    // Placeholder para la longitud
    size_t longitudPos = contenido.tellp();
    contenido << "XXXX >>\n"
              << "stream\n"
              << "BT\n";

    // Título principal
    contenido << "/F2 16 Tf\n"  // Fuente negrita, tamaño 16
              << "1 0 0 1 50 800 Tm\n"  // Posición inicial
              << "(SISTEMA BANCARIO MICHIBANK) Tj\n"
              << "0 -20 Td\n"
              << "/F2 14 Tf\n"
              << "(Listado Completo de Titulares, Cuentas y Movimientos) Tj\n"
              << "0 -10 Td\n"
              << "/F1 10 Tf\n"
              << "(Fecha de generacion: " << __DATE__ << " " << __TIME__ << ") Tj\n";

    float yPos = 750; // Posición Y actual
    const float lineSpacing = 12; // Espaciado entre líneas
    const float pageBottom = 50; // Margen inferior
    const float pageTop = 800; // Margen superior

    // Leer y procesar el archivo línea por línea
    string linea;
    bool esTitulo = false;
    bool esCuenta = false;
    bool esMovimiento = false;

    while (getline(archivo, linea)) {
        // Verificar si necesitamos una nueva página
        if (yPos < pageBottom) {
            contenido << "ET\n"
                      << "endstream\n"
                      << "endobj\n";
            // En una implementación completa, aquí crearías una nueva página
            yPos = pageTop; // Reiniciar posición
            contenido << "BT\n"
                      << "/F1 10 Tf\n"
                      << "1 0 0 1 50 " << yPos << " Tm\n";
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
            // Título de titular
            contenido << "0 -" << lineSpacing * 2 << " Td\n"
                      << "/F2 12 Tf\n"  // Negrita
                      << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing * 2;
        } else if (escapedLine.find("CUENTA CORRIENTE") != string::npos || 
                   escapedLine.find("CUENTA DE AHORRO") != string::npos) {
            // Títulos de cuentas
            contenido << "0 -" << lineSpacing * 1.5 << " Td\n"
                      << "/F2 10 Tf\n"  // Negrita, tamaño menor
                      << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing * 1.5;
        } else if (escapedLine.find("MOVIMIENTOS:") != string::npos) {
            // Título de movimientos
            contenido << "0 -" << lineSpacing << " Td\n"
                      << "/F2 9 Tf\n"   // Negrita, tamaño pequeño
                      << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing;
        } else if (escapedLine.find("  ") == 0 && escapedLine.find(". ID:") != string::npos) {
            // Movimientos individuales (líneas que empiezan con espacios)
            contenido << "0 -" << lineSpacing * 0.8 << " Td\n"
                      << "/F1 8 Tf\n"   // Fuente normal, pequeña
                      << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing * 0.8;
        } else if (!escapedLine.empty() && escapedLine.find("===") == string::npos) {
            // Líneas normales de información
            contenido << "0 -" << lineSpacing << " Td\n"
                      << "/F1 9 Tf\n"   // Fuente normal
                      << "(" << escapedLine << ") Tj\n";
            yPos -= lineSpacing;
        } else if (escapedLine.find("===") != string::npos) {
            // Líneas separadoras - saltar sin imprimir
            contenido << "0 -" << lineSpacing * 0.5 << " Td\n";
            yPos -= lineSpacing * 0.5;
        }
    }

    contenido << "ET\n"
              << "endstream\n"
              << "endobj\n";

    // Calcular la longitud real del contenido
    string contenidoStr = contenido.str();
    string streamContent = contenidoStr.substr(contenidoStr.find("stream\n") + 7);
    streamContent = streamContent.substr(0, streamContent.find("\nendstream"));
    size_t longitudReal = streamContent.length();

    // Reemplazar el placeholder con la longitud real
    string longitudStr = to_string(longitudReal);
    while (longitudStr.length() < 4) longitudStr = " " + longitudStr;
    contenidoStr.replace(contenidoStr.find("XXXX"), 4, longitudStr);

    pdf << contenidoStr;

    // Tabla de referencias cruzadas
    pdf << "xref\n"
        << "0 7\n"
        << "0000000000 65535 f \n";

    // Calcular offsets (simplificado)
    size_t offset = 9; // Longitud de "%PDF-1.4\n"
    pdf << setfill('0') << setw(10) << offset << " 00000 n \n";
    
    offset += string("1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n").size();
    pdf << setfill('0') << setw(10) << offset << " 00000 n \n";
    
    offset += string("2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n").size();
    pdf << setfill('0') << setw(10) << offset << " 00000 n \n";
    
    offset += string("3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R /F2 5 0 R >> >> /MediaBox [0 0 595 842] /Contents 6 0 R >>\nendobj\n").size();
    pdf << setfill('0') << setw(10) << offset << " 00000 n \n";
    
    offset += string("4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n").size();
    pdf << setfill('0') << setw(10) << offset << " 00000 n \n";
    
    offset += string("5 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>\nendobj\n").size();
    pdf << setfill('0') << setw(10) << offset << " 00000 n \n";

    // Trailer
    pdf << "trailer\n"
        << "<< /Size 7 /Root 1 0 R >>\n"
        << "startxref\n"
        << offset << "\n"
        << "%%EOF\n";

    pdf.close();
    archivo.close();
    
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
