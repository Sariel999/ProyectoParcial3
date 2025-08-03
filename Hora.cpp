
#include "Hora.h"

int Hora::getHoras(void)
{
   return horas;
}

void Hora::setHoras(int newHoras)
{
   horas = newHoras;
}

int Hora::getMinutos(void)
{
   return minutos;
}

void Hora::setMinutos(int newMinutos)
{
   minutos = newMinutos;
}

int Hora::getSegundos(void)
{
   return segundos;
}

void Hora::setSegundos(int newSegundos)
{
   segundos = newSegundos;
}

Hora::Hora()
{
}

Hora::~Hora()
{
   // TODO : implement
}