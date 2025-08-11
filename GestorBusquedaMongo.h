/**
 * @file GestorBusquedaMongo.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar busquedas y cargas desde MongoDB
 * @version 0.1
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GESTORBUSQUEDAMONGO_H
#define GESTORBUSQUEDAMONGO_H

#include "ListaDobleCircular.h"
#include "Titular.h"
#include "GestorConexion.h"
#include <string>

/**
 * @brief Clase para gestionar busquedas de titulares con carga automatica desde MongoDB
 */
class GestorBusquedaMongo {
private:
    GestorConexion& gestorConexion;

public:
    /**
     * @brief Constructor que recibe referencia al gestor de conexion MongoDB
     * @param gestor Referencia al gestor de conexion
     */
    GestorBusquedaMongo(GestorConexion& gestor);

    /**
     * @brief Destructor
     */
    ~GestorBusquedaMongo();

    /**
     * @brief Busca un titular por CI, primero en memoria local y luego en MongoDB
     * Si lo encuentra en MongoDB, lo carga automaticamente en la lista local
     * @param titulares Lista de titulares en memoria
     * @param ci Cedula de identidad a buscar
     * @return Titular* Puntero al titular encontrado o nullptr si no existe
     */
    Titular* buscarTitularConCarga(ListaDobleCircular<Titular*>& titulares, const std::string& ci);

    /**
     * @brief Busca un titular solo en la lista local (sin cargar desde MongoDB)
     * @param titulares Lista de titulares donde buscar
     * @param ci Cedula de identidad a buscar
     * @return Titular* Puntero al titular encontrado o nullptr si no existe
     */
    Titular* buscarTitularLocal(const ListaDobleCircular<Titular*>& titulares, const std::string& ci);

private:
    /**
     * @brief Reemplaza o agrega un titular en la lista local
     * @param titulares Lista de titulares
     * @param titular Titular a agregar/reemplazar
     * @param ci Cedula del titular
     * @return bool true si se realizo la operacion correctamente
     */
    bool reemplazarOAgregarTitular(ListaDobleCircular<Titular*>& titulares, Titular* titular, const std::string& ci);
};

#endif // GESTORBUSQUEDAMONGO_H
