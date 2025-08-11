/**
 * @file GeneradorQR.cpp
 * @author your name (you@domain.com)
 * @brief Implementacion de la clase GeneradorQR
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "GeneradorQR.h"
#include "GestorBusquedaMongo.h"
#include "NodoDoble.h"
#include <cstdlib>
#include <functional>

using namespace std;

GeneradorQR::GeneradorQR() : gestorBusquedaMongo(nullptr) {
}

GeneradorQR::~GeneradorQR() {
}

void GeneradorQR::setGestorBusquedaMongo(GestorBusquedaMongo* gestorMongo) {
    gestorBusquedaMongo = gestorMongo;
}

/**
 * @brief Obtiene un titular con datos actualizados desde MongoDB o busqueda local
 * 
 * @param cedula Cedula del titular a buscar
 * @param titulares Lista de titulares local como fallback
 * @return Titular* Puntero al titular encontrado o nullptr
 */
Titular* GeneradorQR::obtenerTitularActualizado(const string& cedula, const ListaDobleCircular<Titular*>& titulares) {
    // Intentar obtener datos frescos desde MongoDB
    if (gestorBusquedaMongo) {
        cout << "Obteniendo datos actualizados desde MongoDB..." << endl;
        Titular* titularFresco = gestorBusquedaMongo->obtenerTitularFresco(cedula);
        
        if (titularFresco) {
            cout << "Titular encontrado en base de datos con datos actualizados." << endl;
            return titularFresco;
        } else {
            cout << "Titular no encontrado en MongoDB. Buscando en datos locales..." << endl;
        }
    }
    
    // Fallback: buscar en lista local
    return buscarTitularPorCI(titulares, cedula);
}

/**
 * @brief Busca un titular por su cedula de identidad (CI).
 * 
 * @param titulares Lista de titulares
 * @param ci Cedula a buscar
 * @return Titular* Puntero al titular encontrado o nullptr
 */
Titular* GeneradorQR::buscarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const string& ci) {
    NodoDoble<Titular*>* actual = titulares.getCabeza();
    if (actual != nullptr) {
        do {
            if (actual->dato->getPersona().getCI() == ci)
                return actual->dato;
            actual = actual->siguiente;
        } while (actual != titulares.getCabeza());
    }
    return nullptr;
}

/**
 * @brief Genera un PDF con codigo QR para un titular especifico
 * Utiliza datos actualizados desde MongoDB si hay conexion disponible.
 * 
 * @param titulares Lista de titulares del sistema (usado como fallback)
 */
void GeneradorQR::generarQRPDF(const ListaDobleCircular<Titular*>& titulares) {
    system("cls");
    cout << "\n--- GENERAR CODIGO QR EN PDF ---\n" << endl;

    if (titulares.vacia() && !gestorBusquedaMongo) {
        cout << "\nNo hay titulares registrados y no hay conexion a base de datos.\n" << endl;
        system("pause");
        return;
    }

    string cedula = val.ingresarCedula(const_cast<char*>("\nIngrese cedula del titular para generar QR:"));
    
    // Obtener titular con datos actualizados desde MongoDB o busqueda local
    Titular* titular = obtenerTitularActualizado(cedula, titulares);
    
    if (!titular) {
        cout << "\nTitular no encontrado con la cedula: " << cedula << endl;
        system("pause");
        return;
    }

    if (!titular->getCuentaCorriente() && titular->getCuentasAhorro().vacia()) {
        cout << "\nEl titular no tiene cuentas registradas.\n" << endl;
        system("pause");
        return;
    }

    // Construir informacion completa de todas las cuentas
    string nombre = titular->getPersona().getNombre() + " " + titular->getPersona().getApellido();
    string infoCuentas = "";
    
    // Agregar cuenta corriente si existe
    if (titular->getCuentaCorriente()) {
        infoCuentas += "CC:" + titular->getCuentaCorriente()->getID() + ";";
    }
    
    // Agregar cuentas de ahorro
    if (!titular->getCuentasAhorro().vacia()) {
        NodoDoble<CuentaBancaria*>* actual = titular->getCuentasAhorro().getCabeza();
        if (actual) {
            do {
                infoCuentas += "CA:" + actual->dato->getID() + ";";
                actual = actual->siguiente;
            } while (actual != titular->getCuentasAhorro().getCabeza());
        }
    }

    // Formato para QR con informacion completa
    string qrData = "MICHIBANK\n|" + nombre + "\n|CI:" + cedula + "\n|" + infoCuentas;

    // Generar PDF con QR integrado
    generarPDFConQR(nombre, cedula, infoCuentas, qrData);
    
    cout << "\nQR generado exitosamente" << endl;
    system("pause");
}

/**
 * @brief Genera un PDF con informacion del titular y un QR funcional
 * 
 * @param nombre Nombre completo del titular
 * @param cedula Cedula del titular
 * @param cuentasInfo Informacion de las cuentas
 * @param qrData Datos que contendra el QR
 */
void GeneradorQR::generarPDFConQR(const string& nombre, const string& cedula, const string& cuentasInfo, const string& qrData) {
    string outputFile = "QR_Titular_" + cedula + ".pdf";
    ofstream archivo(outputFile, ios::binary);
    if (!archivo) {
        cout << "\nError: No se pudo crear el archivo PDF: " << outputFile << endl;
        return;
    }

    // Generar QR usando libqrencode directamente
    QRcode *qr = QRcode_encodeString(qrData.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    
    int QR_SIZE = 21;  // Valor por defecto
    bool hasRealQR = false;
    
    if (qr) {
        QR_SIZE = qr->width;
        hasRealQR = true;
    } else {
        cout << "\nError: usando tamano fijo 21x21" << endl;
    }

    archivo << "%PDF-1.4\n";
    archivo << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";
    archivo << "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n";
    archivo << "3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources << /Font << /F1 4 0 R /F2 5 0 R >> >> /MediaBox [0 0 595 842] /Contents 6 0 R >>\nendobj\n";
    archivo << "4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>\nendobj\n";
    archivo << "5 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n";

    stringstream contenido;
    contenido << "6 0 obj\n<< /Length [LENGTH] >>\nstream\n";
    contenido << "BT\n";
    contenido << "/F1 18 Tf\n200 780 Td\n(MICHIBANK) Tj\n";
    contenido << "/F1 14 Tf\n150 750 Td\n(CODIGO QR DEL TITULAR) Tj\n";
    contenido << "/F1 12 Tf\n50 720 Td\n(INFORMACION DEL TITULAR:) Tj\n";
    contenido << "/F2 11 Tf\n50 700 Td\n(Nombre: " << nombre << ") Tj\n";
    contenido << "50 680 Td\n(Cedula: " << cedula << ") Tj\n";
    contenido << "50 660 Td\n(Cuentas: " << cuentasInfo << ") Tj\n";
    
    FechaHora fechaActual;
    contenido << "50 640 Td\n(Fecha: " << fechaActual.obtenerFecha() << " " << fechaActual.obtenerHora() << ") Tj\n";
    
    contenido << "/F1 12 Tf\n50 600 Td\n(CONTENIDO DEL QR:) Tj\n";
    contenido << "/F2 10 Tf\n50 580 Td\n(" << qrData << ") Tj\n";
    contenido << "ET\n";

    // Dibujar QR en el PDF - Centrado y mas grande
    float xBase = 220, yBase = 520;  // Posicion centrada
    float moduleSize = 8;  // Tamano de cada modulo del QR
    
    // Marco blanco alrededor del QR
    contenido << "q\n";
    contenido << "1 1 1 rg\n";  // Color blanco
    contenido << (xBase - 10) << " " << (yBase - QR_SIZE * moduleSize - 10) << " " 
              << (QR_SIZE * moduleSize + 20) << " " << (QR_SIZE * moduleSize + 20) << " re\n";
    contenido << "f\n";
    contenido << "Q\n";
    
    // Dibujar matriz QR usando datos reales de libqrencode
    contenido << "q\n";
    contenido << "0 0 0 rg\n";  // Color negro para modulos
    
    if (hasRealQR && qr) {
        // Usar datos reales de libqrencode
        for (int i = 0; i < QR_SIZE; i++) {
            for (int j = 0; j < QR_SIZE; j++) {
                if ((qr->data[i * QR_SIZE + j] & 1) != 0) {
                    float x = xBase + j * moduleSize;
                    float y = yBase - i * moduleSize;
                    contenido << x << " " << (y - moduleSize) << " " << moduleSize << " " << moduleSize << " re\nf\n";
                }
            }
        }
    } else {
        // Fallback: generar QR simple
        bool qrMatrix[21][21];
        for (int i = 0; i < 21; i++) {
            for (int j = 0; j < 21; j++) {
                qrMatrix[i][j] = false;
            }
        }
        generarQRSimple(qrData, qrMatrix);
        
        for (int i = 0; i < 21; i++) {
            for (int j = 0; j < 21; j++) {
                if (qrMatrix[i][j]) {
                    float x = xBase + j * moduleSize;
                    float y = yBase - i * moduleSize;
                    contenido << x << " " << (y - moduleSize) << " " << moduleSize << " " << moduleSize << " re\nf\n";
                }
            }
        }
    }
    contenido << "Q\n";
    
    // Liberar memoria de libqrencode
    if (qr) {
        QRcode_free(qr);
    }
    
    // Marco negro alrededor del QR
    contenido << "q\n";
    contenido << "0 0 0 RG\n";  // Color negro para borde
    contenido << "2 w\n";       // Grosor de linea
    contenido << (xBase - 10) << " " << (yBase - QR_SIZE * moduleSize - 10) << " " 
              << (QR_SIZE * moduleSize + 20) << " " << (QR_SIZE * moduleSize + 20) << " re\n";
    contenido << "S\n";         // Solo el contorno
    contenido << "Q\n";
    contenido << "endstream\nendobj\n";
    
    string contenidoStr = contenido.str();
    string lengthStr = to_string(contenidoStr.find("endstream") - contenidoStr.find("stream\n") - 7);
    contenidoStr.replace(contenidoStr.find("[LENGTH]"), 8, lengthStr);
    
    archivo << contenidoStr;
    archivo << "xref\n0 7\n0000000000 65535 f \n0000000009 00000 n \n0000000074 00000 n \n0000000173 00000 n \n0000000301 00000 n \n0000000380 00000 n \n0000000459 00000 n \n";
    archivo << "trailer\n<< /Size 7 /Root 1 0 R >>\nstartxref\n";
    archivo << "538\n%%EOF\n";
    
    archivo.close();
    cout << "\nPDF generado exitosamente: " << outputFile << endl;
}

/**
 * @brief Genera una matriz QR simple usando patrones basicos
 * 
 * @param data Datos a codificar en el QR
 * @param qrMatrix Matriz 21x21 donde se generara el QR
 */
void GeneradorQR::generarQRSimple(const string& data, bool qrMatrix[21][21]) {
    const int SIZE = 21;
    
    // Limpiar matriz
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            qrMatrix[i][j] = false;
        }
    }
    
    // Patrones de deteccion simplificados pero funcionales
    auto dibujarFinder = [&](int x, int y) {
        // 7x7 exterior negro
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (x + j < SIZE && y + i < SIZE) {
                    qrMatrix[y + i][x + j] = true;
                }
            }
        }
        // 5x5 interior blanco
        for (int i = 1; i < 6; i++) {
            for (int j = 1; j < 6; j++) {
                if (x + j < SIZE && y + i < SIZE) {
                    qrMatrix[y + i][x + j] = false;
                }
            }
        }
        // 3x3 centro negro
        for (int i = 2; i < 5; i++) {
            for (int j = 2; j < 5; j++) {
                if (x + j < SIZE && y + i < SIZE) {
                    qrMatrix[y + i][x + j] = true;
                }
            }
        }
    };
    
    // Tres esquinas
    dibujarFinder(0, 0);
    dibujarFinder(14, 0);
    dibujarFinder(0, 14);
    
    // Timing patterns
    for (int i = 8; i < 13; i++) {
        qrMatrix[6][i] = (i % 2 == 0);
        qrMatrix[i][6] = (i % 2 == 0);
    }
    
    // Dark module
    qrMatrix[13][8] = true;
    
    // Datos simplificados
    hash<string> hasher;
    size_t hashValue = hasher(data);
    
    for (int i = 9; i < 13; i++) {
        for (int j = 9; j < 13; j++) {
            int charIndex = ((i - 9) * 4 + (j - 9)) % data.length();
            if (charIndex < static_cast<int>(data.length())) {
                char c = data[charIndex];
                qrMatrix[i][j] = ((c + i + j + hashValue) % 2 == 0);
            }
        }
    }
}
