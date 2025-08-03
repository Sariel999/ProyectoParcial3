#include "FechaHora.h"
#include <ctime>

using namespace std;

FechaHora::FechaHora() {
    actualizarFechaHora();
}

void FechaHora::actualizarFechaHora() {
    time_t ahora = time(nullptr);
    tm* tiempoLocal = localtime(&ahora);

    anio = 1900 + tiempoLocal->tm_year;
    mes = 1 + tiempoLocal->tm_mon;
    dia = tiempoLocal->tm_mday;
    hora = tiempoLocal->tm_hour;
    minuto = tiempoLocal->tm_min;
    segundo = tiempoLocal->tm_sec;
}

string FechaHora::obtenerFecha() const{
    const_cast<FechaHora*>(this)->actualizarFechaHora();
    return to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
}

string FechaHora::obtenerHora() const{
    const_cast<FechaHora*>(this)->actualizarFechaHora();
    return to_string(hora) + ":" + to_string(minuto) + ":" + to_string(segundo);
}

// Getters
int FechaHora::getAnio() const { return anio; }
int FechaHora::getMes() const { return mes; }
int FechaHora::getDia() const { return dia; }
int FechaHora::getHora() const { return hora; }
int FechaHora::getMinuto() const { return minuto; }
int FechaHora::getSegundo() const { return segundo; }

// Setters
void FechaHora::setAnio(int a) { anio = a; }
void FechaHora::setMes(int m) { mes = m; }
void FechaHora::setDia(int d) { dia = d; }
void FechaHora::setHora(int h) { hora = h; }
void FechaHora::setMinuto(int m) { minuto = m; }
void FechaHora::setSegundo(int s) { segundo = s; }
