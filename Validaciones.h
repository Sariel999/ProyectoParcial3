#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <string>
using namespace std;

class Validaciones {
public:
    int ingresarEntero(const char* mensaje);
    float ingresarFlotante(char msj[50]);
    string ingresarCadena(char msj[50]);
    char ingresarCaracter(char msj[50]);
    string ingresarOperacion(char msj[50]);
    string ingresarCedula(char msj[50]);
    string ingresarNumeroTelefonico(char msj[50]);
    float ingresarMonto(const char* mensaje);
    string ingresarNumeros(char msj[50]);
    string ingresarCorreo(char msj[50]);
    bool esCedulaValida(string cedula);
    bool validarLongitud(string cedula);
    bool validarNumeros(string cedula);
    bool validarDigitoVerificador(string cedula);
    string ingresarTextoLibre(char msj[50]);
    string ingresarCodigoBak(char msj[50]);
    string ingresarCodigoSucursal(char msj[50]);
    float ingresarCoordenada(char msj[50], bool esLatitud = true);
    string ingresarNombreArchivo(const char* mensaje);
    string ingresarContrasena(const char* mensaje);
    string ingresarNombreBackupDB(const char* mensaje);
    string ingresarNombreBackupDBConCancelacion(const char* mensaje);

};

#endif
