/**
 * @file ValidacionFecha.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ValidacionFecha.h"
#include <iostream>
#include <conio.h>
#include <cctype>
#include <ctime>
#include <cstdlib>

bool ValidacionFecha::formatoCorrecto(const std::string& fecha) {
    if (fecha.length() != 10)
        return false;
    // DD
    if (!isdigit(fecha[0]) || !isdigit(fecha[1]))
        return false;
    // /
    if (fecha[2] != '/')
        return false;
    // MM
    if (!isdigit(fecha[3]) || !isdigit(fecha[4]))
        return false;
    // /
    if (fecha[5] != '/')
        return false;
    // AAAA
    for (int i = 6; i < 10; ++i)
        if (!isdigit(fecha[i]))
            return false;
    return true;
}

bool ValidacionFecha::valoresValidos(int dia, int mes, int anio) {
    if (anio < 1900 || anio > 2025)
        return false;
    if (mes < 1 || mes > 12)
        return false;
    int diasMes[] = {31, esBisiesto(anio) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (dia < 1 || dia > diasMes[mes - 1])
        return false;
    return true;
}

bool ValidacionFecha::esBisiesto(int anio) {
    return (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
}


bool ValidacionFecha::esMayorDeEdad(int dia, int mes, int anio) {
    // Obtener fecha actual
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int diaHoy = now->tm_mday;
    int mesHoy = now->tm_mon + 1; // tm_mon es 0-11
    int anioHoy = now->tm_year + 1900;

    int edad = anioHoy - anio;
    if (mesHoy < mes || (mesHoy == mes && diaHoy < dia)) {
        edad--;
    }
    return edad >= 18;
}

bool ValidacionFecha::ingresarFecha(std::string& fecha, int& dia, int& mes, int& anio) {
    do {
        std::cout << "\nFormato de la fecha (DD/MM/AAAA): ";
        char buffer[11];
        int i = 0;
        while (i < 10) {
            char c = getch();
            // Solo permitir números y barras en las posiciones correctas
            if ((i == 2 || i == 5) && c == '/') {
                buffer[i++] = c;
                std::cout << c;
            } else if ((i != 2 && i != 5) && isdigit(c)) {
                buffer[i++] = c;
                std::cout << c;
            } else if (c == 8 && i > 0) { // Backspace
                --i;
                std::cout << "\b \b";
            }
        }
        buffer[10] = '\0';
        std::cout << std::endl;
        fecha = buffer;

        if (!formatoCorrecto(fecha)) {
            std::cout << "\nFormato incorrecto. Debe ser DD/MM/AAAA.\n";
            continue;
        }

        dia = std::stoi(fecha.substr(0, 2));
        mes = std::stoi(fecha.substr(3, 2));
        anio = std::stoi(fecha.substr(6, 4));

        if (!valoresValidos(dia, mes, anio)) {
            std::cout << "\nFecha invalida.\n";
            continue;
        }

        return true;
    } while (true);

}
int ValidacionFecha::compararFechas(int d1, int m1, int a1, int d2, int m2, int a2) {
    if (a1 < a2) return -1;
    if (a1 > a2) return 1;
    if (m1 < m2) return -1;
    if (m1 > m2) return 1;
    if (d1 < d2) return -1;
    if (d1 > d2) return 1;
    return 0;
}