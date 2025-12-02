#include "proyecto.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Vector estático para simular la base de datos de usuarios
// Los usuarios se almacenan en la memoria mientras la aplicación se ejecuta.
static vector<Usuario> usuarios;

// --- Implementación de Funciones Auxiliares ---

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
        cout << "\n  🎉 ¡Bienvenido, " << user_input << "! Has iniciado sesión con éxito.\n";
        // Aquí iría la lógica para entrar a la aplicación principal.
    } else {
        cout << "\n  ❌ ERROR: Usuario o contraseña incorrectos.\n";
    }

    cout << "============================================\n";
}

void registrarse() {
    cout << "\n============================================\n";
    cout << "  ➕ PANTALLA DE REGISTRO\n";
    
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
    
    cout << "\n  ✅ ¡REGISTRO EXITOSO! Ya puedes iniciar sesión.\n";
    cout << "============================================\n";
}