// ==========================================================
// ARCHIVO: src/backend/AuthService.cpp
// Contiene la implementación de la lógica de negocio.
// ==========================================================
#include "proyecto.h"
#include <iostream>

// Constructor (opcional, por ahora no hace nada)
AuthService::AuthService() {}

// Función conceptual de hashing (Simulación simple)
std::string AuthService::hashPassword(const std::string& password) {
    // NOTA: En un proyecto real, se usaría una librería criptográfica fuerte
    return password + "_hashed_securely"; 
}

// Implementación del requisito clave F1
bool AuthService::isFirstYear(const std::string& matricula) {
    // [Fuente: 4] RESTRICCIÓN: debe ser de primer ingreso para ser válido.
    
    // SIMULACIÓN DE LA CONEXIÓN CON LA UCO:
    // Asumimos que la matrícula de primer ingreso comienza con 'I'
    return matricula.length() == 8 && matricula.substr(0, 1) == "I";
}

std::pair<bool, std::string> AuthService::registerUser(
    const std::string& fullName,
    const std::string& matricula,
    const std::string& password,
    const std::string& role
) {
    // 1. Aplicar la restricción F1 para Tutelados
    if (role == ROL_TUTELADO) {
        if (!isFirstYear(matricula)) {
            return {false, "Registro fallido: Solo alumnos de primer ingreso pueden ser Tutelados."};
        }
    }
    
    // 2. Comprobar si ya existe
    if (users.count(matricula)) {
        return {false, "El usuario con esa matrícula ya está registrado."};
    }
    
    // 3. Crear y guardar el usuario
    User newUser;
    newUser.name = fullName;
    newUser.passwordHash = hashPassword(password);
    newUser.role = role;

    users[matricula] = newUser;
    return {true, "Registro exitoso. Bienvenido como " + role + "."};
}

std::pair<bool, std::string> AuthService::loginUser(
    const std::string& matricula,
    const std::string& password
) {
    // [Fuente: 269] El sistema valida la información
    auto it = users.find(matricula);

    if (it != users.end()) {
        const User& user = it->second;
        if (hashPassword(password) == user.passwordHash) {
            return {true, "Inicio de sesión exitoso. Rol: " + user.role};
        }
    }

    return {false, "Credenciales incorrectas o usuario no registrado."};
}