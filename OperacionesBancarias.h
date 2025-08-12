/**
 * @file OperacionesBancarias.h
 * @author your name (you@domain.com)
 * @brief Clase para gestionar operaciones bancarias como depositos y retiros
 * @version 0.1
 * @date 2025-08-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef OPERACIONESBANCARIAS_H
#define OPERACIONESBANCARIAS_H

#include <string>
#include "ListaDobleCircular.h"
#include "Titular.h"
#include "CuentaBancaria.h"
#include "Movimiento.h"
#include "Validaciones.h"
#include "Backups.h"
#include "GestorConexion.h"
#include "GestorBusquedaMongo.h"
#include <sstream>
#include <iomanip>

class OperacionesBancarias {
private:
    Validaciones val;
    GestorConexion& gestorConexion;
    GestorBusquedaMongo gestorBusqueda;
    
public:
    OperacionesBancarias(GestorConexion& gestor);
    ~OperacionesBancarias();
    
    // OPERACIONES BANCARIAS
    void realizarDeposito(ListaDobleCircular<Titular*>& titulares);
    void realizarRetiro(ListaDobleCircular<Titular*>& titulares);
    
    // OPERACIONES ATOMICAS (sin riesgo de concurrencia)
    void realizarDepositoAtomico(ListaDobleCircular<Titular*>& titulares);
    void realizarRetiroAtomico(ListaDobleCircular<Titular*>& titulares);
    bool ejecutarDepositoAtomico(const std::string& cedula, const std::string& idCuenta, 
                                float monto);
    bool ejecutarRetiroAtomico(const std::string& cedula, const std::string& idCuenta, 
                              float monto);
    float consultarSaldoActual(const std::string& cedula, const std::string& idCuenta);
    
    // FUNCIONES AUXILIARES
    Titular* buscarTitularPorCI(const ListaDobleCircular<Titular*>& titulares, const std::string& ci);
    CuentaBancaria* seleccionarCuenta(Titular* titular);
    
    // GESTION DE CONEXION MONGODB
    void configurarModoServidor();
    void configurarModoCliente();
    bool estaConectadoMongoDB() const;
    std::string generarJSONTitularCompleto(const Titular* titular);
    void sincronizarTitularCompleto(const Titular* titular);
};

#endif
