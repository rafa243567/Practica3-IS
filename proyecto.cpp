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

 // --- 2. BUSCAR USUARIO (Preparado para DB) ---
Usuario* buscarUsuario(const string& user, const string& pass) {
    for (auto& u : usuarios) {
        if (u.usuario == user && u.contrasena == pass) {
            return &u; // Devuelve el usuario encontrado para poder leer su rol
        }
    }
    return nullptr; // No encontrado
}

// Función para limpiar el buffer de entrada en caso de error
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Función para limpiar la pantalla
void limpiarPantalla() {
    #ifdef _WIN32
        system("cls"); // Para Windows
    #else
        system("clear"); // Para Mac/Linux
    #endif
}




// --- 1. SIMULACIÓN DE BASE DE DATOS ---
void inicializarDatosPrueba() {
    // Estos datos simulan lo que ya existiría en tu base de datos SQL
    usuarios.push_back({"admin",  "1234", "admin"});
    usuarios.push_back({"profe",  "1234", "tutor"});
    usuarios.push_back({"alumno", "1234", "alumno"}); 
    
    cout << "  [DEBUG] Base de datos simulada cargada con " << usuarios.size() << " usuarios.\n";
}





// --- Implementación de Funciones del Menú ---

void mostrarMenu() {
    limpiarPantalla();
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
        limpiarPantalla(); // <--- 1. Limpia lo anterior
        cout << "\n============================================\n";
        cout << "  🎓 MENÚ PRINCIPAL DEL ESTUDIANTE: " << usuario.usuario << "\n";
        cout << "============================================\n";
        cout << "1. Consultar Tutor\n";
        cout << "2. Cerrar Sesión\n";
        cout << "============================================\n";
        cout << "Selecciona una opción: ";
        
        if (!(cin >> opcion)) {
            limpiarBuffer();
            continue;
        }

        switch (opcion) {
            case 1:
                cout << "\n🔎 Buscando tutor asignado...\n";
                // Pausa para que el usuario pueda leer antes de que se limpie la pantalla de nuevo
                cout << "Presiona Enter para continuar...";
                cin.ignore(); cin.get(); 
                break;
            case 2:
                cout << "\n Cerrando sesión...\n";
                en_menu = false; // <--- Esto rompe el bucle y vuelve a iniciarSesion
                break;
            default:
                break;
        }
    }
}

void menuTutor(const Usuario& usuario) {
    bool en_menu = true;
    int opcion;

    while (en_menu) { // <--- IMPORTANTE: El bucle mantiene al usuario aquí
        limpiarPantalla();
        cout << "\n============================================\n";
        cout << "  🍎 MENÚ PRINCIPAL DEL TUTOR: " << usuario.usuario << "\n";
        cout << "============================================\n";
        cout << "1. Ver mis alumnos\n";
        cout << "2. Cerrar Sesión\n";
        cout << "============================================\n";
        cout << "Selecciona una opción: ";

        if (!(cin >> opcion)) {
            limpiarBuffer();
            continue;
        }

        switch (opcion) {
            case 1:
                cout << "\n📄 Listando alumnos...\n";
                cout << "Presiona Enter para continuar...";
                cin.ignore(); cin.get();
                break;
            case 2:
                en_menu = false;
                break;
        }
    }
}

void menuAdministrador(const Usuario& usuario) {
    bool en_menu = true;
    int opcion;

    while (en_menu) { // <--- IMPORTANTE: El bucle mantiene al usuario aquí
        limpiarPantalla();
        cout << "\n============================================\n";
        cout << "  ⚙️ MENÚ DEL ADMINISTRADOR: " << usuario.usuario << "\n";
        cout << "============================================\n";
        cout << "1. Gestionar Usuarios\n";
        cout << "2. Cerrar Sesión\n";
        cout << "============================================\n";
        cout << "Selecciona una opción: ";

        if (!(cin >> opcion)) {
            limpiarBuffer();
            continue;
        }

        switch (opcion) {
            case 1:
                cout << "\n🔧 Panel de gestión...\n";
                cout << "Presiona Enter para continuar...";
                cin.ignore(); cin.get();
                break;
            case 2:
                en_menu = false;
                break;
        }
    }
}


// --- Implementación de Funciones Auxiliares ---

void iniciarSesion() {
    cout << "\n============================================\n";
    cout << "  ➡️ PANTALLA DE INICIO DE SESIÓN\n";
    
    string user_input, pass_input;
    cout << "  Usuario: "; cin >> user_input;
    cout << "  Contraseña: "; cin >> pass_input;
    
    // Buscamos el usuario en la "Base de Datos"
    Usuario* usuarioLogueado = buscarUsuario(user_input, pass_input);

    if (usuarioLogueado != nullptr) {
        cout << "\n  ✅ Login correcto. Rol detectado: " << usuarioLogueado->rol << "\n";
        
        // SWITCH DE REDIRECCIÓN AUTOMÁTICA
        if (usuarioLogueado->rol == "alumno") {
            menuEstudiante(*usuarioLogueado);
        } 
        else if (usuarioLogueado->rol == "tutor") {
            menuTutor(*usuarioLogueado);
        } 
        else if (usuarioLogueado->rol == "admin") {
            menuAdministrador(*usuarioLogueado);
        } 
        else {
            cout << "  ⚠️ Error: Tu usuario no tiene un rol válido asignado.\n";
        }

    } else {
        cout << "\n  ❌ ERROR: Usuario o contraseña incorrectos.\n";
    }
    cout << "============================================\n";
}

// --- 4. REGISTRO (Sin pedir rol) ---
void registrarse() {
    cout << "\n============================================\n";
    cout << "  REGISTRO DE NUEVO ESTUDIANTE\n";
    cout << "============================================\n";

    string user_input, pass_input;
    cout << "  Nuevo Usuario: "; cin >> user_input;
    
    // Verificación simple de duplicados
    for (const auto& u : usuarios) {
        if (u.usuario == user_input) {
            cout << "  ❌ El usuario ya existe.\n";
            return;
        }
    }
    
    cout << "  Contraseña: "; cin >> pass_input;
    
    // LÓGICA IMPORTANTE:
    // Como es un registro público, asumimos que es ALUMNO.
    // Los Admins y Tutores se crearían manualmente en la Base de Datos.
    Usuario nuevo_usuario = {user_input, pass_input, "alumno"};
    
    usuarios.push_back(nuevo_usuario);
    
    cout << "\n  ¡Cuenta creada! Ahora eres un 'alumno'.\n";
}