/**
 * @file MenuAdministrador.h
 * @author MichiBank Team
 * @brief Clase para gestionar el menu de administrador del sistema
 * @version 1.0
 * @date 2025-08-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MENUADMINISTRADOR_H
#define MENUADMINISTRADOR_H

#include "ListaDobleCircular.h"
#include "Titular.h"
#include "BPlusTreeTitulares.h"
#include "Backups.h"
#include "FechaHora.h"
#include "Validaciones.h"
#include "GestorConexion.h"
#include <string>

class MenuAdministrador {
private:
    Validaciones val;
    GestorConexion* gestorConexion;

public:
    MenuAdministrador();
    MenuAdministrador(GestorConexion& gestor);
    ~MenuAdministrador();
    
    // Menu principal del administrador
    void mostrarMenuAdministrador(ListaDobleCircular<Titular*>& titulares, BPlusTreeTitulares& arbolTitulares);
    
    // Opciones del menu
    void hacerBackupBaseDatos();
    void reestablecerBackupBaseDatos();
    void programarBackupBaseDatos();
    
    // Metodos auxiliares
    bool validarContrasenaAdmin();
    void esperarHoraEspecifica(int hora, int minuto, int segundo);
    std::string crearNombreBackupDB();
    bool exportarBaseDatosCompleta(const std::string& nombreArchivo);
    bool importarBaseDatosCompleta(const std::string& nombreArchivo);
};

#endif
