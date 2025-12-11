#ifndef PROYECTO_H
#define PROYECTO_H

#include <iostream>
#include <string>
#include <sqlite3.h> // Necesario para la integración en la Base de datos 

// Estructura para almacenar los datos de un usuario temporalmente en memoria
struct Usuario {
    std::string usuario;
    std::string contrasena;
    std::string rol; // "alumno", "tutor", "admin"
};

// --- Funciones de Gestión de Datos ---
void inicializarDatosPrueba(); // Ahora solo conecta a la DB

// --- Declaración de Funciones del Menú ---
void mostrarMenu();
void iniciarSesion();
void registrarse();

// Menús por rol
void menuEstudiante(const Usuario& usuario);
void menuTutor(const Usuario& usuario);
void menuAdministrador(const Usuario& usuario);

// --- Función Auxiliar ---
Usuario* buscarUsuario(const std::string& usuario, const std::string& contrasena);

// Funciones para la Asignación de Tutores
void iniciarBaseDeDatos(sqlite3 *db); // Abre la base de datos para guardar los datos que se inscriban en ella
void RealizarAsignacion(sqlite3 *db); // Funcion que se va a usar para realizar la asgnación de tutores a los alumnos 
void VerAsignaciones(sqlite3 *db); // Funcion que se va a usar para ver las asignaciones realizadas
#endif // PROYECTO_H