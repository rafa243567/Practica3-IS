// ==========================================================
// ARCHIVO: src/main.cpp
// Contiene la función main para la prueba del sistema.
// ==========================================================
 // Incluye la cabecera de la clase
#include "proyecto.h"
#include <stdio.h>

int main() {
    AuthService authService;
    std::pair<bool, std::string> result;
    
    printf("--- PRUEBA F1: REGISTRO DE TUTELADOS ---\n");

    // 1. Registro exitoso (Cumple la restricción: Primer Ingreso 'I')
    result = authService.registerUser("Ana López", "I2025001", "pass123", "tutelado");
    printf("1. Primer Ingreso: %s\n", result.second.c_str());

    // 2. Registro fallido (Violación de la restricción: No es Primer Ingreso 'P')
    result = authService.registerUser("Pablo Ríos", "P2023005", "pass456", "tutelado");
    printf("2. No Primer Ingreso: %s\n", result.second.c_str());
    
    // 3. Registro de Tutor (Este rol no tiene la restricción de primer ingreso)
    result = authService.registerUser("Sara Mentor", "U2020001", "tutopass", "tutor");
    printf("3. Registro Tutor: %s\n", result.second.c_str());

    printf("\n--- PRUEBA F1: INICIO DE SESIÓN ---\n");
    
    // 4. Login exitoso (Ana)
    result = authService.loginUser("I2025001", "pass123");
    printf("4. Login Ana: %s\n", result.second.c_str());

    // 5. Login fallido (Contraseña incorrecta)
    result = authService.loginUser("I2025001", "passERRONEA");
    printf("5. Login Fallido: %s\n", result.second.c_str());

    return 0;
}