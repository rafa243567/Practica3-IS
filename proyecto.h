#ifndef PROYECTO_H
#define PROYECTO_H

#include <string>

// Estructura para almacenar los datos de un usuario
struct Usuario {
    std::string usuario;
    std::string contrasena;
};

// --- Declaración de Funciones del Menú ---

// Muestra las opciones del menú principal.
void mostrarMenu();

// Función principal para iniciar sesión, pide credenciales y verifica.
void iniciarSesion();

// Función principal para registrar un nuevo usuario, pide credenciales y guarda.
void registrarse();

// --- Función Auxiliar ---

// Busca un usuario con las credenciales dadas en el vector de usuarios.
// Retorna true si las credenciales son correctas.
bool buscarUsuario(const std::string& usuario, const std::string& contrasena);

#endif // PROYECTO_H