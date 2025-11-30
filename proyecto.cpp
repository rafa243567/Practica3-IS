// ==========================================================
// ARCHIVO: src/backend/AuthService.cpp
// Contiene la implementación de la lógica de negocio.
// ==========================================================
#include "proyecto.h"
#include <iostream>

// Constructor (opcional, por ahora no hace nada)
AuthService::AuthService() {}

// Función conceptual de hashing (Simulación simple)
std::string AuthService::hashPassword(const std::string& Contraseña) {
    // NOTA: En un proyecto real, se usaría una librería criptográfica fuerte
    return Contraseña + "_hashed_securely"; 
}

// Implementación del requisito clave F1
bool AuthService::isFirstYear(const std::string& Matricula) {
    // [Fuente: 4] RESTRICCIÓN: debe ser de primer ingreso para ser válido.
    
    // SIMULACIÓN DE LA CONEXIÓN CON LA UCO:
    // Asumimos que la matrícula de primer ingreso comienza con 'I'
    return Matricula.length() == 8 && Matricula.substr(0, 1) == "I";
}

std::pair<bool, std::string> AuthService::registerUser(
    const std::string& NombreCompleto,
    const std::string& Matricula,
    const std::string& Contraseña,
    const std::string& Rol
) {
    // 1. Aplicar la restricción F1 para Tutelados
    if (Rol == ROL_TUTELADO) {
        if (!isFirstYear(matricula)) {
            return {false, "Registro fallido: Solo alumnos de primer ingreso pueden ser Tutelados."};
        }
    }
    
    // 2. Comprobar si ya existe
    if (users.count(Matricula)) {
        return {false, "El usuario con esa matrícula ya está registrado."};
    }
    
    // 3. Crear y guardar el usuario
    User newUser;
    newUser.nombre = NombreCompleto;
    newUser.passwordHahs = hashPassword(Contraseña);
    newUser.rol = Rol;

    users[Matricula] = newUser;
    return {true, "Registro exitoso. Bienvenido como " + Rol + "."};
}

std::pair<bool, std::string> AuthService::loginUser(
    const std::string& Matricula,
    const std::string& Contraseña
) {
    // [Fuente: 269] El sistema valida la información
    auto it = users.find(Matricula);

    if (it != users.end()) {
        const User& user = it->second;
        if (hashPassword(Contraseña) == user.passwordHash) {
            return {true, "Inicio de sesión exitoso. Rol: " + user.rol};
        }
    }

    return {false, "Credenciales incorrectas o usuario no registrado."};
}
