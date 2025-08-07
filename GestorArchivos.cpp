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
 * @brief Guarda los titulares y sus cuentas bancarias en un archivo de texto.
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

    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual) {
        do {
            Titular* titular = actual->dato;
            Persona persona = titular->getPersona();

            cout << "Guardando titular: " << persona.getNombre() << " " << persona.getApellido() << endl;

            archivo << "CI: " << persona.getCI() << " - Nombre: " << persona.getNombre() << " " 
                    << persona.getApellido() << " - Tel: " << persona.getTelefono() << " - Correo: " 
                    << persona.getCorreo() << " - Fecha Nacimiento: " 
                    << persona.getFechaNa().getDia() << "/" << persona.getFechaNa().getMes() << "/" 
                    << persona.getFechaNa().getAnio().getAnio() << "\n";

            CuentaBancaria* cuentaCorriente = titular->getCuentaCorriente();
            if (cuentaCorriente) {
                cout << "Guardando cuenta corriente de " << persona.getNombre() << endl;
                archivo << "--- CUENTA CORRIENTE ---" << endl;
                archivo << "ID Cuenta: " << cuentaCorriente->getID() << " - Tipo: " 
                        << cuentaCorriente->getTipoCuenta() << " - Saldo: " 
                        << cuentaCorriente->getSaldo() << "\n";
            } else {
                cout << "No tiene cuenta corriente." << endl;
            }

            NodoDoble<CuentaBancaria*>* nodoAhorro = titular->getCuentasAhorro().getCabeza();
            if (nodoAhorro) {
                cout << "Guardando cuentas de ahorro de " << persona.getNombre() << endl;
                do {
                    archivo << "--- CUENTA DE AHORRO ---" << endl;
                    archivo << "ID Cuenta: " << nodoAhorro->dato->getID() << " - Tipo: " 
                            << nodoAhorro->dato->getTipoCuenta() << " - Saldo: " 
                            << nodoAhorro->dato->getSaldo() << "\n";
                    nodoAhorro = nodoAhorro->siguiente;
                } while (nodoAhorro != titular->getCuentasAhorro().getCabeza());
            } else {
                cout << "No tiene cuentas de ahorro." << endl;
            }

            archivo << "\n";
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }

    archivo.close();
    cout << "\nTitulares guardados exitosamente en 'titulares.txt'.\n" << endl;

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
 * @brief Genera un archivo PDF con los titulares registrados.
 * 
 */
void GestorArchivos::generarPDFTitulares() {
    system("cls");
    cout << "\n--- GENERAR PDF DE TITULARES ---\n" << endl;

    // Open the input text file
    ifstream archivo("titulares.txt");
    if (!archivo) {
        cout << "\nNo se pudo abrir el archivo 'titulares.txt'.\n" << endl;
        system("pause");
        return;
    }

    // Output PDF file
    string outputFile = "titulares.pdf";
    ofstream pdf(outputFile, ios::binary);
    if (!pdf) {
        cout << "\nNo se pudo crear el archivo PDF.\n" << endl;
        archivo.close();
        system("pause");
        return;
    }

    // Write PDF header
    pdf << "%PDF-1.4\n";

    // Object 1: Catalog
    pdf << "1 0 obj\n"
        << "<< /Type /Catalog /Pages 2 0 R >>\n"
        << "endobj\n";

    // Object 2: Pages
    pdf << "2 0 obj\n"
        << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
        << "endobj\n";

    // Object 3: Page
    pdf << "3 0 obj\n"
        << "<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\n"
        << "endobj\n";

    // Object 4: Font
    pdf << "4 0 obj\n"
        << "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\n"
        << "endobj\n";

    // Object 5: Content stream
    stringstream contenido;
    contenido << "5 0 obj\n"
              << "<< /Length " << /* Length placeholder */ " >>\n"
              << "stream\n"
              << "BT\n"
              << "/F1 12 Tf\n"
              << "1 0 0 1 50 792 Tm\n" // Set text matrix (start at top-left, 50,792)
              << "(Listado de Titulares) Tj\n";

    // Adjust text position
    float yPos = 772; // Start below the title
    const float lineSpacing = 14; // Space between lines
    const float pageBottom = 50; // Bottom margin

    // Read the text file line by line
    string linea;
    while (getline(archivo, linea)) {
        // Check if we need a new page (simple pagination)
        if (yPos < pageBottom) {
            contenido << "ET\n"
                      << "endstream\n"
                      << "endobj\n";
            // Update length and write current content
            string contenidoStr = contenido.str();
            string longitud = to_string(contenidoStr.size() - string("5 0 obj\n<< /Length  >> stream\n").size() - string("endstream\nendobj\n").size());
            contenidoStr.replace(contenidoStr.find("/Length ") + 8, 0, longitud);
            pdf << contenidoStr;

            // Start a new page (simplified: new content stream)
            contenido.str(""); // Clear stream
            yPos = 792; // Reset Y position
            contenido << "5 0 obj\n"
                      << "<< /Length " << /* Length placeholder */ " >>\n"
                      << "stream\n"
                      << "BT\n"
                      << "/F1 12 Tf\n"
                      << "1 0 0 1 50 792 Tm\n";
        }

        // Escape special characters for PDF
        string escapedLine = linea;
        for (size_t i = 0; i < escapedLine.size(); ++i) {
            if (escapedLine[i] == '(' || escapedLine[i] == ')' || escapedLine[i] == '\\') {
                escapedLine.insert(i, "\\");
                ++i;
            }
        }

        // Write line to PDF
        contenido << "0 -" << lineSpacing << " Td\n"
                  << "(" << escapedLine << ") Tj\n";
        yPos -= lineSpacing;
    }

    contenido << "ET\n"
              << "endstream\n"
              << "endobj\n";

    // Update content length
    string contenidoStr = contenido.str();
    string longitud = to_string(contenidoStr.size() - string("5 0 obj\n<< /Length  >> stream\n").size() - string("endstream\nendobj\n").size());
    contenidoStr.replace(contenidoStr.find("/Length ") + 8, 0, longitud);
    pdf << contenidoStr;

    // Cross-reference table
    pdf << "xref\n"
        << "0 6\n"
        << "0000000000 65535 f \n";
    stringstream xref;
    xref << setfill('0') << setw(10);
    size_t offset = 9; // Length of "%PDF-1.4\n"
    xref << offset << " 00000 n \n";
    offset += string("1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R >> >> /MediaBox [0 0 595 842] /Contents 5 0 R >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    offset += string("4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n").size();
    xref << offset << " 00000 n \n";
    pdf << xref.str();

    // Trailer
    pdf << "trailer\n"
        << "<< /Size 6 /Root 1 0 R >>\n"
        << "startxref\n"
        << offset << "\n"
        << "%%EOF\n";

    pdf.close();
    archivo.close();
    cout << "\nPDF generado exitosamente: " << outputFile << "\n" << endl;
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
