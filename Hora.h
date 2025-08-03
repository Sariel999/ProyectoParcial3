
#ifndef HORA_H
#define HORA_H

class Hora
{
public:
   int getHoras(void);
   void setHoras(int newHoras);
   int getMinutos(void);
   void setMinutos(int newMinutos);
   int getSegundos(void);
   void setSegundos(int newSegundos);
   Hora();
   ~Hora();

protected:
private:
   int horas;
   int minutos;
   int segundos;


};

#endif