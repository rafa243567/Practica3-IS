#include "proyecto.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits> // Para std::numeric_limits

using namespace std;

// Vector estático para simular la base de datos de usuarios temporalmente
// Los usuarios se almacenan en la memoria mientras la aplicación se ejecuta.
static vector<Usuario> usuarios;

// --- Funciones Auxiliares ---

/**
 * Busca un usuario y verifica su contraseña.
 * @return true si se encuentra una coincidencia exacta de usuario y contraseña.
 */
bool buscarUsuario(const string& user, const string& pass) {
    for (const auto& u : usuarios) {
        if (u.usuario == user && u.contrasena == pass) {
            return true; // Credenciales correctas
        }
    }
    return false; // Credenciales incorrectas o usuario no encontrado
}

// Función para limpiar el buffer de entrada en caso de error
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// --- Implementación de Funciones del Menú ---

void mostrarMenu() {
    cout << "\n";
    cout << "--- APP DE TUTORÍAS ---\n";
    cout << "========================\n";
    cout << "1. Iniciar Sesión\n";
    cout << "2. Registrarse\n";
    cout << "3. Salir\n";
    cout << "========================\n";
    cout << "Total de usuarios registrados: " << usuarios.size() << "\n";
    cout << "Selecciona una opción (1-3): ";
}

// --- Implementación de Menús por Rol ---

void menuEstudiante(const Usuario& usuario) {
    bool en_menu = true;
    int opcion;

    while (en_menu) {
        cout << "\n============================================\n";
        cout << "  🎓 MENÚ PRINCIPAL DEL ESTUDIANTE: " << usuario.usuario << "\n";
        cout << "============================================\n";
        cout << "1. Consultar Tutor\n";
        cout << "2. Cerrar Sesión\n";
        cout << "============================================\n";
        cout << "Selecciona una opción: ";
        
        // Captura y manejo de errores de entrada en el submenú
        if (!(cin >> opcion)) {
            limpiarBuffer();
            cout << "❌ Entrada inválida. Por favor, ingresa un número.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                cout << "\n🔎 Buscando tutor asignado...\n";
                // Aquí se integraría el motor de búsqueda y filtros.
                break;
            case 2:
                cout << "\n Cerrando sesión del Estudiante...\n";
                en_menu = false; // Rompe el bucle y vuelve a la función iniciarSesion, y de ahí al main.
                break;
            default:
                cout << "Opción no válida.\n";
        }
    }
}

void menuTutor(const Usuario& usuario) {
    cout << "\n============================================\n";
    cout << "  MENÚ PRINCIPAL DEL TUTOR: " << usuario.usuario << "\n";
    cout << "  Cierra la sesión para volver al menú principal.\n";
    cout << "============================================\n";
    // Nota: Aquí se implementaría el bucle 'while' similar al de menuEstudiante.
}

void menuAdministrador(const Usuario& usuario) {
    cout << "\n============================================\n";
    cout << "  ⚙️ MENÚ DEL ADMINISTRADOR: " << usuario.usuario << "\n";
    cout << "  Cierra la sesión para volver al menú principal.\n";
    cout << "============================================\n";
    // Nota: Aquí se implementaría el bucle 'while' similar al de menuEstudiante.
}


// --- Implementación de Funciones Auxiliares ---

void iniciarSesion() {
    cout << "\n============================================\n";
    cout << "  ➡️ PANTALLA DE INICIO DE SESIÓN\n";
    
    if (usuarios.empty()) {
        cout << "  ⚠️ No hay usuarios registrados. Por favor, regístrate primero (Opción 2).\n";
        cout << "============================================\n";
        return;
    }
    
    string user_input, pass_input;
    
    cout << "  Usuario: ";
    cin >> user_input;
    cout << "  Contraseña: ";
    cin >> pass_input;
    
    // Validar credenciales usando la función auxiliar
    if (buscarUsuario(user_input, pass_input)) {
        cout << "\n  ¡Bienvenido, " << user_input << "! Has iniciado sesión con éxito.\n";
        // Aquí iría la lógica para entrar a la aplicación principal.
    } else {
        cout << "\n ERROR: Usuario o contraseña incorrectos.\n";
    }

    cout << "============================================\n";
}

void registrarse() {
    cout << "\n============================================\n";
    cout << "  PANTALLA DE REGISTRO\n";
    cout << "============================================\n";

    string user_input, pass_input;

    // 1. Pedir Nombre de Usuario
    cout << "  Define un Nombre de Usuario: ";
    cin >> user_input;
    
    // 2. Verificar si el usuario ya existe
    for (const auto& u : usuarios) {
        if (u.usuario == user_input) {
            cout << "\n  ❌ ERROR: El usuario '" << user_input << "' ya existe. Intenta con otro nombre.\n";
            cout << "============================================\n";
            return; // Salir de la función de registro
        }
    }
    
    // 3. Pedir Contraseña
    cout << "  Define una Contraseña: ";
    cin >> pass_input;
    
    // 4. Crear y guardar el nuevo usuario
    Usuario nuevo_usuario = {user_input, pass_input};
    usuarios.push_back(nuevo_usuario);
    
    cout << "\n ¡REGISTRO EXITOSO! Ya puedes iniciar sesión.\n";
    cout << "============================================\n";
}