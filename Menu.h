#ifndef MENU_H
#define MENU_H

class Menu
{

public:
    int ingresarMenu( const char* titulo, const char** opciones, int nOpciones );
    void gotoxy( int x, int y );
    Menu();
    ~Menu();
};

#endif
