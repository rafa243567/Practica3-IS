// ==========================================================
// ARCHIVO: src/backend/AuthService.h
// Contiene la declaración de la clase y la estructura de datos.
// ==========================================================
#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <string>
#include <map>
#include <utility> // Para std::pair

// Estructura para almacenar la información de un usuario
struct User {
    std::string name;
    std::string passwordHash;
    std::string role;
};

class AuthService {
private:
    // Simula la base de datos: [Matrícula UCO] -> [Datos del Usuario]
    std::map<std::string, User> users;

    // Roles según el documento de requisitos
    const std::string ROL_TUTELADO = "tutelado"; 
    
    // Función de validación del REQUISITO F1: Solo alumnos de primer ingreso
    bool isFirstYear(const std::string& matricula);
    
    // Simulación de hashing de contraseña (Conceptual)
    std::string hashPassword(const std::string& password);

public:
    // Constructor
    AuthService();

    // F1: Función principal de registro
    std::pair<bool, std::string> registerUser(
        const std::string& fullName,
        const std::string& matricula,
        const std::string& password,
        const std::string& role
    );

    // F1: Función principal de inicio de sesión
    std::pair<bool, std::string> loginUser(
        const std::string& matricula,
        const std::string& password
    );
};

#endif // AUTH_SERVICE_H