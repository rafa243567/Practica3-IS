#include "proyecto.h"
#include <limits>

// Variables globales de SQLite
static sqlite3* db;
static const char* DB_FILE = "tutoria_app.db";

// --- Funciones Auxiliares de Limpieza ---
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ==========================================
// IMPLEMENTACIÓN DE LOS MENÚS
// ==========================================

// --- ALUMNO ---
void Alumno::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE ESTUDIANTE: " << usuario << " ===\n";
        cout << "1. Abrir Chat con Tutor\n";
        cout << "2. Generar Alerta de Incidencia\n";
        cout << "3. Cerrar Sesión\n";
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1) {
            chatPersonal.cargarHistorial(); // Usamos el objeto Chat
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        } 
        else if (opcion == 2) {
            generarAlerta();
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 3);
}

void Alumno::generarAlerta() {
    cout << "\n>> Enviando notificación urgente al Coordinador...\n";
    cout << ">> Incidencia registrada con éxito.\n";
}

// --- TUTOR ---
void Tutor::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE TUTOR: " << usuario << " (" << departamento << ") ===\n";
        cout << "1. Ver Ficha de Alumnos Asignados\n";
        cout << "2. Registrar Acta de Sesión\n";
        cout << "3. Cerrar Sesión\n";
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1) {
            verFichaAlumno();
            limpiarBuffer(); cin.get();
        } 
        else if (opcion == 2) {
            registrarActa();
            limpiarBuffer(); cin.get();
        }
    } while(opcion != 3);
}

void Tutor::verFichaAlumno() {
    cout << "\n--- Consultando Base de Datos Académica ---\n";
    // Simulamos la creación de una ficha (objeto temporal)
    FichaAcademica ficha;
    ficha.datosAcademicos = "Juan Pérez (1º Curso)";
    ficha.asignaturas = "Matemáticas, Programación, Física";
    ficha.mostrarFicha();
}

void Tutor::registrarActa() {
    string tema;
    cout << "\nTema tratado en la sesión: ";
    cin >> tema;
    cout << ">> Acta guardada correctamente en el historial del alumno.\n";
}

// --- Lógica del COORDINADOR (ADMIN) ---
void Coordinador::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE COORDINADOR ===\n";
        cout << "1. Gestionar Asignaciones (Tutor-Alumno)\n";
        cout << "2. Cerrar Sesión\n";
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1) {
            gestionarAsignaciones();
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 2);
}

void Coordinador::gestionarAsignaciones() {
    cout << "\n>> Accediendo al sistema de asignación manual...\n";
    cout << ">> [INFO] Se ha asignado el Tutor 'Miguel' al Alumno 'Juan'.\n";
}

// ==========================================
// GESTIÓN DE BASE DE DATOS Y LOGIN
// ==========================================

void inicializarDatosPrueba() {
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc) {
        cerr << "Error al abrir DB: " << sqlite3_errmsg(db) << endl;
        return;
    }
    
    // Crear tabla si no existe
    string sql = "CREATE TABLE IF NOT EXISTS usuarios ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "usuario TEXT UNIQUE, pass TEXT, rol TEXT);";
    char* err;
    sqlite3_exec(db, sql.c_str(), 0, 0, &err);
}

// ESTA ES LA FUNCIÓN CLAVE (FACTORY)
Usuario* iniciarSesion() {
    string u, p;
    cout << "\n================================\n";
    cout << "      INICIO DE SESIÓN\n";
    cout << "================================\n";
    cout << "Usuario: "; cin >> u;
    cout << "Contraseña: "; cin >> p;

    string sql = "SELECT rol FROM usuarios WHERE usuario = ? AND pass = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        cerr << "Error DB.\n";
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, u.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, p.c_str(), -1, SQLITE_STATIC);

    Usuario* usuarioLogueado = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string rolDB = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        
        cout << "\n✅ Login correcto. Rol detectado: " << rolDB << endl;
        // Pausa breve para efecto visual
        for(int i=0; i<300000000; i++); 

        // AQUÍ OCURRE LA MAGIA: Creamos el objeto específico según el rol
        if (rolDB == "alumno") {
            usuarioLogueado = new Alumno(u, p, "Ingenieria", true);
        } else if (rolDB == "tutor") {
            usuarioLogueado = new Tutor(u, p, "Dpto. Informatica");
        } else if (rolDB == "admin") {
            usuarioLogueado = new Coordinador(u, p);
        }
    } else {
        cout << "\n❌ Usuario o contraseña incorrectos.\n";
        cout << "Presiona Enter...";
        limpiarBuffer(); cin.get();
    }

    sqlite3_finalize(stmt);
    return usuarioLogueado;
}

void registrarse() {
    cout << "\n--- REGISTRO DE NUEVO ESTUDIANTE ---\n";
    cout << "(Los tutores y coordinadores deben ser dados de alta por administración)\n";
    
    string u, p;
    cout << "Usuario: "; cin >> u;
    cout << "Contraseña: "; cin >> p;

    string rol = "alumno"; 

    string sql = "INSERT INTO usuarios (usuario, pass, rol) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    
    // Preparamos la consulta
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        cerr << " Error en la base de datos.\n";
        return;
    }

    // Vinculamos los parámetros
    sqlite3_bind_text(stmt, 1, u.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, p.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, rol.c_str(), -1, SQLITE_STATIC); // Insertamos "alumno" siempre

    // Ejecutamos
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "¡Registro exitoso! Ya puedes iniciar sesión como Alumno.\n";
    } else {
        cout << "Error: El nombre de usuario ya existe.\n";
    }
    
    // Limpiamos memoria de la consulta
    sqlite3_finalize(stmt);
}