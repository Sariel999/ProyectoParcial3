#ifndef BUSQUEDASBINARIAS_H
#define BUSQUEDASBINARIAS_H

#include "ListaDobleCircular.h"
#include "Movimiento.h"
#include "Ordenamiento.h"
#include "DiaUtil.h"
#include "Titular.h"
#include "Sucursal.h"
#include "NodoSucursal.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <utility> // para std::pair

// Clase utilitaria para búsquedas binarias especializadas
class BusquedasBinarias {
public:
    // Busca el primer depósito >= montoMin en una lista ordenada de Movimiento* (por monto ascendente)
    // Retorna un puntero al Movimiento o nullptr si no existe
    Movimiento* primerDepositoMayorIgual(ListaDobleCircular<Movimiento*>& lista, float montoMin) {
        // Definir comparador para búsqueda binaria
        auto cmpDeposito = [](Movimiento* mov, float x) -> int {
            if (!mov->getTipo()) return 1; // Solo depósitos (tipo==true)
            if (mov->getMonto() < x) return -1;
            if (mov->getMonto() >= x) return 0;
            return 1;
        };

        // Usar búsqueda binaria de Ordenamiento
        Ordenamiento<Movimiento*, decltype(cmpDeposito)> ord;
        NodoDoble<Movimiento*>* nodo = nullptr;
        int n = 0;
        NodoDoble<Movimiento*>* actual = lista.getCabeza();
        if (!actual) return nullptr;
        do { n++; actual = actual->siguiente; } while (actual != lista.getCabeza());

        int left = 0, right = n - 1, resIdx = -1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            NodoDoble<Movimiento*>* midNode = lista.getCabeza();
            for (int i = 0; i < mid; ++i) midNode = midNode->siguiente;

            Movimiento* mov = midNode->dato;
            if (!mov->getTipo()) { // No es depósito, buscar a la derecha
                left = mid + 1;
                continue;
            }
            if (mov->getMonto() >= montoMin) {
                resIdx = mid;
                right = mid - 1; // Buscar más a la izquierda
            } else {
                left = mid + 1;
            }
        }
        if (resIdx == -1) return nullptr;
        NodoDoble<Movimiento*>* resNode = lista.getCabeza();
        for (int i = 0; i < resIdx; ++i) resNode = resNode->siguiente;
        if (resNode->dato->getTipo() && resNode->dato->getMonto() >= montoMin)
            return resNode->dato;
        return nullptr;
    }

    // Ejemplo 3: Buscar el saldo mínimo necesario para cumplir una meta en una cuenta de ahorro
    // saldoInicial: saldo actual de la cuenta
    // saldoMeta: saldo objetivo a alcanzar
    // meses: cantidad de meses para alcanzar la meta
    // Retorna el depósito mensual mínimo necesario (entero, redondeado hacia arriba)
    int depositoMinimoParaMeta(float saldoInicial, float saldoMeta, int meses) {
        if (meses <= 0 || saldoMeta <= saldoInicial) return 0;
        float left = 0, right = saldoMeta - saldoInicial;
        float resultado = right;

        // Simula mes a mes si con un depósito mensual dado se alcanza la meta
        auto esPosible = [&](float deposito) {
            float saldo = saldoInicial;
            for (int i = 0; i < meses; ++i) {
                saldo += deposito;
            }
            return saldo >= saldoMeta;
        };

        // Búsqueda binaria sobre el depósito mensual
        while (right - left > 0.01f) { // Precisión de centavos
            float mid = (left + right) / 2.0f;
            if (esPosible(mid)) {
                resultado = mid;
                right = mid;
            } else {
                left = mid;
            }
        }
        // Redondear hacia arriba a dos decimales
        return static_cast<int>(resultado + 0.99f);
    }

    // Busca el primer titular cuyo apellido es mayor o igual al apellido dado (usando Ordenamiento)
    Titular* primerTitularApellidoMayorIgual_Ordenamiento(ListaDobleCircular<Titular*>& lista, const std::string& apellidoBuscado) {
        // Comparador para ordenar por apellido ascendente
        auto cmpOrden = [](Titular* a, Titular* b) {
            return a->getPersona().getApellido() < b->getPersona().getApellido();
        };
        // Ordenar la lista por apellido (si no está ordenada)
        Ordenamiento<Titular*, decltype(cmpOrden)> ord;
        ord.mergeSort(lista, cmpOrden);

        // Comparador para búsqueda binaria (lower bound)
        auto cmpKey = [](Titular* a, const std::string& key) {
            if (a->getPersona().getApellido() < key) return -1;
            if (a->getPersona().getApellido() == key) return 0;
            return 1;
        };

        // Búsqueda binaria usando Ordenamiento
        NodoDoble<Titular*>* nodo = nullptr;
        int n = 0;
        NodoDoble<Titular*>* actual = lista.getCabeza();
        if (!actual) return nullptr;
        do { n++; actual = actual->siguiente; } while (actual != lista.getCabeza());

        int left = 0, right = n - 1, resIdx = -1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            NodoDoble<Titular*>* midNode = lista.getCabeza();
            for (int i = 0; i < mid; ++i) midNode = midNode->siguiente;

            int cmp = cmpKey(midNode->dato, apellidoBuscado);
            if (cmp >= 0) {
                resIdx = mid;
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        if (resIdx == -1) return nullptr;
        NodoDoble<Titular*>* resNode = lista.getCabeza();
        for (int i = 0; i < resIdx; ++i) resNode = resNode->siguiente;
        if (cmpKey(resNode->dato, apellidoBuscado) >= 0)
            return resNode->dato;
        return nullptr;
    }

    // Ejemplo: Buscar el primer titular cuyo CI es mayor o igual a un valor dado
    // La lista debe estar ordenada por CI ascendente
    Titular* primerTitularCIMayorIgual(ListaDobleCircular<Titular*>& lista, const std::string& ciBuscado) {
        int n = 0;
        NodoDoble<Titular*>* actual = lista.getCabeza();
        if (!actual) return nullptr;
        do { n++; actual = actual->siguiente; } while (actual != lista.getCabeza());

        int left = 0, right = n - 1, resIdx = -1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            NodoDoble<Titular*>* midNode = lista.getCabeza();
            for (int i = 0; i < mid; ++i) midNode = midNode->siguiente;

            std::string ci = midNode->dato->getPersona().getCI();
            if (ci.compare(ciBuscado) >= 0) {
                resIdx = mid;
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        if (resIdx == -1) return nullptr;
        NodoDoble<Titular*>* resNode = lista.getCabeza();
        for (int i = 0; i < resIdx; ++i) resNode = resNode->siguiente;
        if (resNode->dato->getPersona().getCI().compare(ciBuscado) >= 0)
            return resNode->dato;
        return nullptr;
    }

    // Ejemplo: Buscar el primer titular cuyo año de nacimiento es mayor o igual a un año dado
    // La lista debe estar ordenada por año de nacimiento ascendente
    Titular* primerTitularAnioNacimientoMayorIgual(ListaDobleCircular<Titular*>& lista, int anioBuscado) {
        int n = 0;
        NodoDoble<Titular*>* actual = lista.getCabeza();
        if (!actual) return nullptr;
        do { n++; actual = actual->siguiente; } while (actual != lista.getCabeza());

        int left = 0, right = n - 1, resIdx = -1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            NodoDoble<Titular*>* midNode = lista.getCabeza();
            for (int i = 0; i < mid; ++i) midNode = midNode->siguiente;

            int anio = midNode->dato->getPersona().getFechaNa().getAnio().getAnio();
            if (anio >= anioBuscado) {
                resIdx = mid;
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        if (resIdx == -1) return nullptr;
        NodoDoble<Titular*>* resNode = lista.getCabeza();
        for (int i = 0; i < resIdx; ++i) resNode = resNode->siguiente;
        if (resNode->dato->getPersona().getFechaNa().getAnio().getAnio() >= anioBuscado)
            return resNode->dato;
        return nullptr;
    }

    // Busca la sucursal más cercana a las coordenadas dadas
    Sucursal* sucursalMasCercana(NodoSucursal* cabeza, float latUsuario, float lonUsuario) {
        if (!cabeza) return nullptr;
        Sucursal* masCercana = nullptr;
        double minDist = 1e9;
        NodoSucursal* actual = cabeza;
        while (actual != nullptr) {
            double dist = Sucursal::calcularDistancia(
                latUsuario, lonUsuario,
                actual->sucursal.getLatitud(),
                actual->sucursal.getLongitud()
            );
            if (dist < minDist) {
                minDist = dist;
                masCercana = &(actual->sucursal);
            }
            actual = actual->siguiente;
        }
        
        return masCercana;
    }

    int maximoIntervaloCitas(NodoSucursal* cabeza, int nClientes, int duracionCita = 30) {
        // Verificar entrada válida
        if (nClientes <= 0 || !cabeza || duracionCita <= 0) return 0;
        
        // Función para calcular el número total de citas posibles con intervalo D
        auto calcularCitasPosibles = [&](int D) -> long long {
            long long totalCitas = 0;
            NodoSucursal* actual = cabeza;
            while (actual != nullptr) {
                int a = actual->sucursal.getHoraApertura();  // Hora de apertura en minutos
                int b = actual->sucursal.getHoraCierre();    // Hora de cierre en minutos
                if (b >= a + duracionCita) { // Intervalo válido
                    // Calcular número de citas: (b - a - duracionCita + D) / D + 1
                    long long citas = (static_cast<long long>(b) - a - duracionCita + D) / D + 1;
                    totalCitas += citas;
                }
                actual = actual->siguiente;
            }
            return totalCitas;
        };
        
        // Búsqueda binaria para encontrar el máximo D
        int left = 1; // Mínimo intervalo posible
        int right = 540; // Máximo intervalo (24 horas en minutos)
        int maxD = 0;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (calcularCitasPosibles(mid) >= nClientes) {
                maxD = mid; // D es factible, intentar un D mayor
                left = mid + 1;
            } else {
                right = mid - 1; // D no es factible, intentar un D menor
            }
        }
        
        return maxD;
    }
};
#endif