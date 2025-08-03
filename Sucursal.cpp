#include "Sucursal.h"
#include <cmath> // Para calcular la distancia entre dos puntos geográficos

// Constructor
/**
 * @brief Construct a new Sucursal:: Sucursal object
 * 
 * @param nombre 
 * @param latitud 
 * @param longitud 
 * @param idSucursal 
 */
Sucursal::Sucursal(const std::string& nombre, float latitud, float longitud, const std::string& idSucursal)
    : nombre(nombre), latitud(latitud), longitud(longitud), idSucursal(idSucursal), contadorCuentas(0) {}

// Getters y setters
/**
 * @brief Obtiene el nombre de la sucursal.
 * 
 * @return std::string 
 */
std::string Sucursal::getNombre() const { return nombre; }
void Sucursal::setNombre(const std::string& nombre) { this->nombre = nombre; }

float Sucursal::getLatitud() const { return latitud; }
void Sucursal::setLatitud(float latitud) { this->latitud = latitud; }

float Sucursal::getLongitud() const { return longitud; }
void Sucursal::setLongitud(float longitud) { this->longitud = longitud; }

std::string Sucursal::getIdSucursal() const { return idSucursal; }
void Sucursal::setIdSucursal(const std::string& idSucursal) { this->idSucursal = idSucursal; }

int Sucursal::getContadorCuentas() const { return contadorCuentas; }
void Sucursal::setContadorCuentas(int contador) { contadorCuentas = contador; }
void Sucursal::incrementarContadorCuentas() { contadorCuentas++; }

// Función para calcular la distancia entre dos coordenadas usando la fórmula de Haversine (en kilómetros)
double Sucursal::calcularDistancia(float lat1, float lon1, float lat2, float lon2) {
    const double PI = 3.14159265358979323846;
    const double R = 6371.0;  // Radio de la Tierra en kilómetros

    // Convertir las coordenadas de grados a radianes
    lat1 = lat1 * PI / 180.0;
    lon1 = lon1 * PI / 180.0;
    lat2 = lat2 * PI / 180.0;
    lon2 = lon2 * PI / 180.0;

    // Aplicar la fórmula de Haversine
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;  // Distancia en kilómetros
}

// Función para mostrar la información de la sucursal
void Sucursal::imprimir() const {
    std::cout << "Sucursal: " << nombre << std::endl;
    std::cout << "ID Sucursal: " << idSucursal << std::endl;
    std::cout << "Coordenadas: (" << latitud << ", " << longitud << ")" << std::endl;
    std::cout << "Numero de cuentas: " << contadorCuentas << std::endl;
}


