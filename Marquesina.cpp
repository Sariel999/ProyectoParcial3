
#include "Marquesina.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <algorithm>

Marquesina::Marquesina() : ejecutando(false), pausado(false), necesita_actualizacion(false),
                          ancho_consola(80), alto_consola(25), posicion_actual(0) {
    handle_consola = GetStdHandle(STD_OUTPUT_HANDLE);
    obtener_dimensiones_consola();
    inicializar_buffer();
}


Marquesina::~Marquesina() {
    detener();
}


void Marquesina::iniciar(const std::string& texto) {
    if (ejecutando.load()) {
        detener();
    }
    
    texto_marquesina = "    " + texto + "    ";  // Espacios para suavizar transición
    ejecutando = true;
    pausado = false;
    posicion_actual = ancho_consola;
    necesita_actualizacion = true;
    
    hilo_marquesina = std::thread(&Marquesina::ejecutar_marquesina, this);
}


void Marquesina::detener() {
    ejecutando = false;
    if (hilo_marquesina.joinable()) {
        hilo_marquesina.join();
    }
    
    // Limpiar solo la línea superior usando la función segura
    std::lock_guard<std::mutex> lock(mutex_consola);
    escribir_linea_segura(0, std::string(ancho_consola, ' '), 
                         FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


void Marquesina::pausar() {
    pausado = true;
}


void Marquesina::reanudar() {
    pausado = false;
}

void Marquesina::ejecutar_marquesina() {
    auto ultimo_update = std::chrono::steady_clock::now();
    
    while (ejecutando.load()) {
        auto ahora = std::chrono::steady_clock::now();
        auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultimo_update);
        
        if (!pausado.load() && tiempo_transcurrido.count() >= 500) { // 500ms para mayor estabilidad
            actualizar_marquesina_en_buffer();
            
            // Solo renderizar si es necesario
            if (necesita_actualizacion.load()) {
                std::lock_guard<std::mutex> lock(mutex_consola);
                
                // Capturar estado actual antes de escribir
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                GetConsoleScreenBufferInfo(handle_consola, &csbi);
                COORD pos_original = csbi.dwCursorPosition;
                WORD color_original = csbi.wAttributes;
                
                // Solo escribir en la línea 0 (marquesina)
                COORD pos = {0, 0};
                SetConsoleCursorPosition(handle_consola, pos);
                
                SetConsoleTextAttribute(handle_consola, 
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED | BACKGROUND_BLUE);
                
                // Escribir solo la línea de marquesina sin salto de línea
                WriteConsole(handle_consola, buffer_pantalla[0].c_str(), 
                           static_cast<DWORD>(buffer_pantalla[0].length()), nullptr, nullptr);
                
                // Restaurar estado inmediatamente
                SetConsoleTextAttribute(handle_consola, color_original);
                SetConsoleCursorPosition(handle_consola, pos_original);
                
                necesita_actualizacion = false;
            }
            
            // Actualizar posición
            posicion_actual--;
            if (posicion_actual < -(int)texto_marquesina.length()) {
                posicion_actual = ancho_consola;
            }
            
            ultimo_update = ahora;
        }
        
        // Sleep corto para no consumir CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


void Marquesina::obtener_dimensiones_consola() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(handle_consola, &csbi)) {
        ancho_consola = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        alto_consola = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        ancho_consola = 80;
        alto_consola = 25;
    }
}

void Marquesina::inicializar_buffer() {
    buffer_pantalla.resize(alto_consola);
    buffer_colores.resize(alto_consola);
    
    for (int i = 0; i < alto_consola; i++) {
        buffer_pantalla[i] = std::string(ancho_consola, ' ');
        buffer_colores[i] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}

void Marquesina::actualizar_marquesina_en_buffer() {
    // Crear línea de marquesina completamente limpia
    std::string linea_marquesina(ancho_consola, ' ');
    
    // Calcular qué parte del texto mostrar con verificaciones estrictas
    if (posicion_actual < ancho_consola && posicion_actual > -(int)texto_marquesina.length()) {
        int inicio_texto = std::max(0, -posicion_actual);
        int inicio_pantalla = std::max(0, posicion_actual);
        int longitud_disponible = ancho_consola - inicio_pantalla;
        int longitud_texto_restante = (int)texto_marquesina.length() - inicio_texto;
        int longitud_visible = std::min(longitud_disponible, longitud_texto_restante);
        
        // Solo escribir si hay texto visible y posición válida
        if (longitud_visible > 0 && inicio_pantalla < ancho_consola && inicio_texto >= 0) {
            std::string parte_texto = texto_marquesina.substr(inicio_texto, longitud_visible);
            
            // Verificar que no excedamos los límites
            if (inicio_pantalla + parte_texto.length() <= static_cast<size_t>(ancho_consola)) {
                linea_marquesina.replace(inicio_pantalla, parte_texto.length(), parte_texto);
            }
        }
    }
    
    // Asegurar que la línea tenga exactamente el ancho de la consola
    if (linea_marquesina.length() != static_cast<size_t>(ancho_consola)) {
        linea_marquesina.resize(ancho_consola, ' ');
    }
    
    buffer_pantalla[0] = linea_marquesina;
    buffer_colores[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
    necesita_actualizacion = true;
}

/**
 * @brief Fuerza la actualización de la pantalla completa (usado por el menú).
 */
void Marquesina::actualizar_pantalla_completa() {
    // Esta función puede ser llamada por el menú para forzar una actualización
    if (ejecutando.load() && !pausado.load()) {
        actualizar_marquesina_en_buffer();
    }
}


void Marquesina::escribir_linea_segura(int fila, const std::string& texto, WORD color) {
    if (fila < 0 || fila >= alto_consola) return;
    
    // Capturar estado actual
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(handle_consola, &csbi);
    COORD pos_original = csbi.dwCursorPosition;
    WORD color_original = csbi.wAttributes;
    
    // Ir a la posición específica
    COORD pos = {0, static_cast<SHORT>(fila)};
    SetConsoleCursorPosition(handle_consola, pos);
    SetConsoleTextAttribute(handle_consola, color);
    
    // Preparar texto ajustado exactamente al ancho de consola
    std::string texto_ajustado = texto;
    if (texto_ajustado.length() > static_cast<size_t>(ancho_consola)) {
        texto_ajustado = texto_ajustado.substr(0, ancho_consola);
    } else if (texto_ajustado.length() < static_cast<size_t>(ancho_consola)) {
        texto_ajustado += std::string(ancho_consola - texto_ajustado.length(), ' ');
    }
    
    // Escribir usando WriteConsole para mayor control
    WriteConsole(handle_consola, texto_ajustado.c_str(), 
                static_cast<DWORD>(texto_ajustado.length()), nullptr, nullptr);
    
    // Restaurar estado inmediatamente
    SetConsoleTextAttribute(handle_consola, color_original);
    SetConsoleCursorPosition(handle_consola, pos_original);
}


void Marquesina::capturar_estado_consola() {
    GetConsoleScreenBufferInfo(handle_consola, &info_consola_original);
}

void Marquesina::restaurar_estado_consola() {
    SetConsoleTextAttribute(handle_consola, info_consola_original.wAttributes);
    SetConsoleCursorPosition(handle_consola, info_consola_original.dwCursorPosition);
}