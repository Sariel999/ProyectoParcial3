/**
 * @file Anio.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ANIO_H
#define ANIO_H

class Anio
{
public:
   int getAnio(void);
   void setAnio(int newAnio);
   bool getAnioBisiesto(void);
   void setAnioBisiesto(bool newAnioBisiesto);
   Anio();
   ~Anio();

protected:
private:
   int anio;
   bool anioBisiesto;


};

#endif