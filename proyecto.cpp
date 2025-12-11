#include "proyecto.h"
#include <iostream>
#include <string>
#include <limits>
#include <sqlite3.h> // Librería de base de datos

using namespace std;

// --- Variables Globales de SQLite ---
static sqlite3* db;
static const char* DB_FILE = "tutoria_app.db"; // El archivo debe estar junto al ejecutable
static const char* USERS_TABLE = "usuarios";

// --- Funciones Auxiliares de Interfaz ---

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

// Callback requerido por sqlite3_exec (aunque no lo usemos para leer datos aquí)
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    return 0;
}

// --- 1. GESTIÓN DE BASE DE DATOS ---

void inicializarDatosPrueba() {
    // 1. Abrir conexión
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc) {
        cerr << "❌ Error fatal: No se pudo abrir la base de datos external (" << DB_FILE << "): " << sqlite3_errmsg(db) << endl;
        exit(1);
    } else {
        cout << "  [SISTEMA] Conexión establecida con la base de datos.\n";
    }

    // 2. Asegurar que la tabla existe (Por seguridad, aunque la crees externamente)
    string sql_create = "CREATE TABLE IF NOT EXISTS " + string(USERS_TABLE) + " ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "usuario TEXT NOT NULL UNIQUE,"
                        "contrasena TEXT NOT NULL,"
                        "rol TEXT NOT NULL);";
    
    char* zErrMsg = 0;
    rc = sqlite3_exec(db, sql_create.c_str(), callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "❌ Error SQL: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    
    // NOTA: Ya no insertamos datos de prueba automáticamente.
}

// --- 2. BUSCAR USUARIO (SELECT) --- para comprobar que esta en la base de datos

Usuario* buscarUsuario(const string& user, const string& pass) {
    static Usuario encontrado; // Estático para devolver puntero válido

    string sql = "SELECT contrasena, rol FROM " + string(USERS_TABLE) + " WHERE usuario = ?;";
    sqlite3_stmt* stmt;

    // Preparar la consulta
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        cerr << "❌ Error al consultar DB: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    // Vincular el parámetro (el usuario que escribe la persona)
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);

    // Ejecutar paso a paso
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Usuario existe, verificamos contraseña
        string db_pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string db_rol  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        if (db_pass == pass) {
            encontrado.usuario = user;
            encontrado.contrasena = pass;
            encontrado.rol = db_rol;
            sqlite3_finalize(stmt);
            return &encontrado;
        }
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

// --- 3. REGISTRO (INSERT) --- añadir nuevo usuario

void registrarse() {
    cout << "\n============================================\n";
    cout << "  REGISTRO DE NUEVO ESTUDIANTE\n";
    cout << "============================================\n";

    string user_input, pass_input;
    cout << "  Nuevo Usuario: "; cin >> user_input;
    cout << "  Contraseña: "; cin >> pass_input;
    
    // Por defecto, el registro público es para alumnos
    string rol = "alumno"; 

    string sql = "INSERT INTO " + string(USERS_TABLE) + " (usuario, contrasena, rol) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        cerr << "❌ Error en DB: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Vincular parámetros
    sqlite3_bind_text(stmt, 1, user_input.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pass_input.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, rol.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        cout << "\n  ✅ ¡Registro exitoso! Ya puedes iniciar sesión.\n";
    } else if (rc == SQLITE_CONSTRAINT) {
        cout << "  ❌ El usuario '" << user_input << "' ya existe.\n";
    } else {
        cout << "  ❌ Error desconocido al guardar.\n";
    }
}

// --- 4. MENÚS (INTERFAZ) ---

void mostrarMenu() {
    limpiarPantalla();
    cout << "\n";
    cout << "--- APP DE TUTORÍAS (DB SQLite) ---\n";
    cout << "========================\n";
    cout << "1. Iniciar Sesión\n";
    cout << "2. Registrarse\n";
    cout << "3. Salir\n";
    cout << "========================\n";
    cout << "Selecciona una opción (1-3): ";
}

void menuEstudiante(const Usuario& usuario) {
    bool en_menu = true;
    int opcion;
    while (en_menu) {
        limpiarPantalla(); 
        cout << "\n MENÚ ESTUDIANTE: " << usuario.usuario << "\n";
        cout << " 1. Consultar Tutor\n";
        cout << " 2. Cerrar Sesión\n";
        cout << "Opción: ";
        if (!(cin >> opcion)) { limpiarBuffer(); continue; }
        
        if (opcion == 1) {
            cout << "\n Buscando tutor...\nPresiona Enter.";
            cin.ignore(); cin.get();
        } else if (opcion == 2) {
            en_menu = false;
        }
    }
}

void menuTutor(const Usuario& usuario) {
    bool en_menu = true;
    int opcion;
    while (en_menu) {
        limpiarPantalla();
        cout << "\n MENÚ TUTOR: " << usuario.usuario << "\n";
        cout << " 1. Ver alumnos\n ";
        cout << " 2. Cerrar Sesión\n ";
        cout << "Opción: ";
        if (!(cin >> opcion)) { limpiarBuffer(); continue; }

        if (opcion == 1) {
            cout << "\n Listando alumnos...\nPresiona Enter.";
            cin.ignore(); cin.get();
        } else if (opcion == 2) {
            en_menu = false;
        }
    }
}

void menuAdministrador(const Usuario& usuario) {
    bool en_menu = true;
    int opcion;
    while (en_menu) { 
        limpiarPantalla();
        cout << "\n MENÚ ADMIN: " << usuario.usuario << "\n";
        cout << " 1. Gestionar Usuarios\n";
        cout << " 2. Cerrar Sesión\n";
        cout << " Opción: ";
        if (!(cin >> opcion)) { limpiarBuffer(); continue; }

        if (opcion == 1) {
            cout << "\n Panel de gestión...\nPresiona Enter.";
            cin.ignore(); cin.get();
        } else if (opcion == 2) {
            en_menu = false;
        }
    }
}

void iniciarSesion() {
    cout << "\n============================================\n";
    cout << "  INICIO DE SESIÓN\n";
    string u, p;
    cout << "  Usuario: "; cin >> u;
    cout << "  Contraseña: "; cin >> p;
    
    Usuario* usuarioLogueado = buscarUsuario(u, p);

    if (usuarioLogueado != nullptr) {
        cout << "\n  ✅ Bienvenido " << usuarioLogueado->usuario << " (" << usuarioLogueado->rol << ")\n";
        // Pequeña pausa visual
        for(int i=0; i<300000000; i++); 

        if (usuarioLogueado->rol == "alumno") menuEstudiante(*usuarioLogueado);
        else if (usuarioLogueado->rol == "tutor") menuTutor(*usuarioLogueado);
        else if (usuarioLogueado->rol == "admin") menuAdministrador(*usuarioLogueado);
        else cout << "  Rol desconocido.\n";

    } else {
        cout << "\n  ❌ Credenciales incorrectas.\n";
        cout << "  Presiona Enter para volver...";
        cin.ignore(); cin.get();
    }
}


int MostrarDatos (void *NotUsed, int argc, char **argv, char **azColName){
    
    cout << "Guardado a : " << (argv[4] ? argv[4] : "?") << " | ";
    cout << "Tutor: " << (argv[2] ? argv[2] : "SinNombre") << " (ID " << (argv[0] ? argv[0] : "?") << ") ";
    cout << "--> Alumno: " << (argv[3] ? argv[3] : "SinNombre") << " (ID " << (argv[1] ? argv[1] : "?") << ")" << endl;

    return 0; 
}

void iniciarBaseDeDatos(sqlite3 *db) {
    char *error = 0; 
    string sql = "CREATE TABLE IF NOT EXISTS asignaciones ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "id_tutor INTEGER, "
                 "nombre_tutor TEXT, "   
                 "id_alumno INTEGER, "
                 "nombre_alumno TEXT, " 
                 "fecha TEXT DEFAULT CURRENT_TIMESTAMP);";

    int resultado = sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    
    if (resultado != 0) {
        cout << "Error al crear la tabla: " << error << endl;
    }
} 

// función para introducir los datos y la asignación de tutor y alumno 
void RealizarAsignacion(sqlite3 *db) {
    int id_tutor, id_alumno;
    string nom_tutor, nom_alumno;
    char *error = 0;

    cout << "--- REALIZAR ASIGNACIÓN DE TUTOR ---"<< endl;

    //Datos tutor 
    cout << "Ingrese el ID del tutor: ";
    cin >> id_tutor;
    cin.ignore();
    cout << "Ingrese el nombre del tutor: ";
    getline(cin, nom_tutor);

    //Datos alumno
    cout << "Ingrese el ID del alumno: ";
    cin >> id_alumno;
    cin.ignore();
    cout << "Ingrese el nombre del alumno: ";
  getline(cin, nom_alumno);

    string sql = "INSERT INTO asignaciones (id_tutor, id_alumno, nombre_tutor, nombre_alumno) VALUES (" 
                 + to_string(id_tutor) + ", " 
                 + to_string(id_alumno) + ", '" 
                 + nom_tutor + "', '" 
                 + nom_alumno + "');";

    cout << "Guardando los datos en el sistema... " << endl;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    
    if (rc != SQLITE_OK) {
        cout << " Error al asignar: " << error << endl;
        sqlite3_free(error);
    } else {
        cout << "Asignación de tutor completa correctamente." << endl;
    }
}

void VerAsignaciones(sqlite3 *db) {
    char *error = 0;

    cout << "--- LISTA DE ASIGNACIONES DE TUTORES ---" << endl;
    string sql = "SELECT id_tutor, id_alumno, nombre_tutor, nombre_alumno, fecha FROM asignaciones;";
    int rc = sqlite3_exec(db, sql.c_str(), MostrarDatos, 0, &error);
    
    if (rc != SQLITE_OK) {
        cout << " Error al leer la lista: " << error << endl;
        sqlite3_free(error);
    }
    cout << "---------------------------------------" << endl;
}

