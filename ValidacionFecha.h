#ifndef VALIDACIONFECHA_H
#define VALIDACIONFECHA_H

#include <string>

class ValidacionFecha {
public:
    // Devuelve true si la fecha es válida y la guarda en los parámetros por referencia
    bool ingresarFecha(std::string& fecha, int& dia, int& mes, int& anio);
    bool formatoCorrecto(const std::string& fecha);
    bool valoresValidos(int dia, int mes, int anio);
    bool esBisiesto(int anio);
    bool esMayorDeEdad(int dia, int mes, int anio);
    int compararFechas(int d1, int m1, int a1, int d2, int m2, int a2);
};

#endif