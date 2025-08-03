
#include "Fecha.h"

int Fecha::getDia(void) const
{
   return dia;
}

void Fecha::setDia(int newDia)
{
   dia = newDia;
}

int Fecha::getMes(void) const
{
   return mes;
}

void Fecha::setMes(int newMes)
{
   mes = newMes;
}

Anio Fecha::getAnio(void) const
{
   return anio;
}

void Fecha::setAnio(Anio newAnio)
{
   anio = newAnio;
}

Fecha::Fecha()
{
}


Fecha::~Fecha()
{
   // TODO : implement
}
