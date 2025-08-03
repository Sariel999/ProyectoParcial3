#ifndef DIAUTIL_H
#define DIAUTIL_H

#include "Fecha.h"
#include <vector>

bool esMismaFecha(const Fecha& a, const Fecha& b);
bool esNoTrasladable(int dia, int mes);
Fecha trasladarFeriadoSegunLey(const Fecha& original);
int calcularDiaSemana(int d, int m, int y);
Fecha calcularPascua(int anio);
std::vector<Fecha> feriadosMoviles(int anio);
std::vector<Fecha> obtenerFeriadosFijos(int anio);
bool esFeriado(const Fecha& fecha);
bool esDiaLaborable(const Fecha& fecha);
Fecha sumarUnDia(Fecha f);
Fecha restarUnDia(Fecha f);
Fecha ajustarAlDiaHabilMasCercano(Fecha f);
Fecha siguienteFeriadoDesde(Fecha f);
#endif