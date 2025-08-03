#ifndef FECHA_H
#define FECHA_H
#include "Anio.h"
class Fecha
{
public:
   int getDia()const;
   void setDia(int newDia);
   int getMes() const;
   void setMes(int newMes);
   Anio getAnio() const;
   void setAnio(Anio newAnio);
   Fecha();
   ~Fecha();

protected:
private:
   int dia;
   int mes;
   Anio anio;


};
#endif 