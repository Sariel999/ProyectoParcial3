/**
 * @file Marquesina.h
 * @brief Declaración de la clase Marquesina para animación de texto en consola tipo marquesina.
 *
 * Esta clase permite mostrar un texto desplazándose horizontalmente en la parte superior de la consola,
 * utilizando hilos, sincronización y manipulación avanzada de la consola de Windows.
 */
#ifndef MARQUESINA_H_INCLUDED
#define MARQUESINA_H_INCLUDED
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <chrono>
#include <vector>


class Marquesina {
public:

    Marquesina();

    ~Marquesina();

    void iniciar(const std::string& texto);

    void detener();

    void pausar();

    void reanudar();

    void actualizar_pantalla_completa();
    
private:

    std::atomic<bool> ejecutando;

    std::atomic<bool> pausado;

    std::atomic<bool> necesita_actualizacion;

    std::thread hilo_marquesina;

    std::string texto_marquesina;

    int ancho_consola;

    int alto_consola;

    int posicion_actual;

    mutable std::mutex mutex_consola;

    std::vector<std::string> buffer_pantalla;

    std::vector<WORD> buffer_colores;

    HANDLE handle_consola;

    CONSOLE_SCREEN_BUFFER_INFO info_consola_original;

    void ejecutar_marquesina();

    void obtener_dimensiones_consola();

    void inicializar_buffer();

    void actualizar_marquesina_en_buffer();

    void renderizar_buffer_completo();

    void capturar_estado_consola();

    void restaurar_estado_consola();

    void escribir_linea_segura(int fila, const std::string& texto, WORD color);
};

#endif