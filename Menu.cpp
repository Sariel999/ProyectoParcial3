#include <iostream> 
#include <conio.h>
#include <windows.h> 
#include <cstdlib>
#include "Menu.h"
using namespace std;

// Definicion de la funcion gotoxy 
void Menu:: gotoxy (int x, int y){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

int Menu:: ingresarMenu (const char* titulo, const char** opciones, int n){
    int opcion = 1;
    int tecla;
    bool repite = true;

    do {
        system("cls");
        //Mostrar Titulo en la línea 2 (Y=1)
        gotoxy(15, 2); cout << titulo;
        //Mostrar Opciones desde la línea 4 (Y=3)
        for (int i = 0; i < n; i++) {
            gotoxy(10, 4 + i); 
            cout << i + 1 <<"." << *(opciones + i); 
        }
        //Mostrar Flecha cursor alineada con la opción seleccionada
        gotoxy(8, 4 + (opcion - 1)); 
        cout << "->";
        // Solo se puede presionar tecla arriba 72, abajo 80 o enter 13
        do {
            tecla = getch();
        } while(tecla!= 72 && tecla != 80 && tecla != 13) ;
        switch(tecla){
            case 72:
                opcion--;
                if(opcion < 1){
                    opcion = n;
                }
                break;
            case 80:
                opcion++;
                if(opcion > n){
                    opcion = 1;
                }
                break;
            case 13:
                repite = false;
                break;
        }
    }while(repite);
    return opcion; 
}

Menu:: Menu() {}

Menu:: ~Menu() {}