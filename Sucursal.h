/**
 * @file Sucursal.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SUCURSAL_H
#define SUCURSAL_H

#include <string>
#include <iostream>

class Sucursal {
private:
    std::string nombre;
    float latitud;  // Latitud de la sucursal
    float longitud; // Longitud de la sucursal
    std::string idSucursal;  // Identificador único de la sucursal
    int contadorCuentas;  // Contador de cuentas asociadas a la sucursal
    int horaApertura;
    int horaCierre;

public:
    // Constructor
    Sucursal(const std::string& nombre, float latitud, float longitud, const std::string& idSucursal);

    // Getters y setters
    std::string getNombre() const;
    void setNombre(const std::string& nombre);

    float getLatitud() const;
    void setLatitud(float latitud);

    float getLongitud() const;
    void setLongitud(float longitud);

    std::string getIdSucursal() const;
    void setIdSucursal(const std::string& idSucursal);

    int getContadorCuentas() const;
    void incrementarContadorCuentas();
    void setContadorCuentas(int contador);
    // Función para calcular la distancia entre dos coordenadas usando la fórmula de Haversine (en kilómetros)
    static double calcularDistancia(float lat1, float lon1, float lat2, float lon2);

    // Función para mostrar la información de la sucursal
    void imprimir() const;

    int getHoraApertura() const { return horaApertura; }
    void setHoraApertura(int hora) { horaApertura = hora; }

    int getHoraCierre() const { return horaCierre; }
    void setHoraCierre(int hora) { horaCierre = hora; }
};

#endif
