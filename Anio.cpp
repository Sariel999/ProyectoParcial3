/**
 * @file Anio.cppsis
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Anio.h"

int Anio::getAnio(void)
{
   return anio;
}

void Anio::setAnio(int newAnio)
{
   anio = newAnio;
}

bool Anio::getAnioBisiesto(void)
{
   return anioBisiesto;
}

void Anio::setAnioBisiesto(bool newAnioBisiesto)
{
   anioBisiesto = newAnioBisiesto;
}

Anio::Anio()
{
}

Anio::~Anio()
{
   // TODO : implement
}