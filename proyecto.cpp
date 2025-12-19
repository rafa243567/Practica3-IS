#include "proyecto.h"
#include <limits>
#include <sstream>

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
        cout << "1. Ver mi Tutor Asignado\n";            
        cout << "2. Realizar Encuesta de Satisfacción\n"; 
        cout << "3. Abrir Chat con Tutor\n";
        cout << "4. Generar Alerta de Incidencia\n";
        cout << "5. Cerrar Sesión\n";
        cout << "Opción: ";

        if (!(cin >> opcion)) {
            cout << "Entrada inválida.\n";
            limpiarBuffer();
            continue;
        }

        if (opcion == 1) {
            MostrarTutorAsignado(db, usuario);
            cout << "\nPresiona Enter para volver al menú.";
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 2) {
            Encuesta(db, usuario);
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 3) {
            string tutor_asig = ObtenerTutorAsignadoUsuario(db, usuario);
            if (tutor_asig != "") {
                MostrarHistorialChat(db, usuario, tutor_asig);
            } else {
                cout << " No tienes tutor asignado. No se puede abrir el chat." << endl;
            }
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        } 
        else if (opcion == 4) {
            GenerarAlertaIncidencia(db, usuario);
            cout << "\nPresiona Enter para volver al menú...";
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 5);
}

void Alumno::generarAlerta() {
    cout << "\n>> Enviando notificación urgente al Coordinador...\n";
    cout << ">> Incidencia registrada con éxito.\n";
}

// --- TUTOR (CON NUEVA FUNCIÓN) ---
void Tutor::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE TUTOR: " << usuario << " ===\n";
        cout << "1. Ver Alumnos Asignados\n";
        cout << "2. Registrar Acta de Sesión\n";
        cout << "3. Abrir Chat con Alumno\n"; 
        cout << "4. Ver Alertas de mis Alumnos Asignados\n"; // <--- NUEVA OPCIÓN
        cout << "5. Cerrar Sesión\n"; 
        cout << "Opción: ";
        
        if (!(cin >> opcion)) {
            cout << "Entrada inválida.\n";
            limpiarBuffer();
            continue;
        }

        if (opcion == 1) {
            MostrarAlumnosAsignados(db, usuario); 
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        } 
        else if (opcion == 2) {
            RegistrarActa(db, usuario);
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 3) {
            string alumno_asig = ObtenerAlumnoAsignadoUsuario(db, usuario);
            if (alumno_asig != "") {
                MostrarHistorialChat(db, usuario, alumno_asig);
            } else {
                cout << " No tienes alumnos asignados. No se puede abrir el chat." << endl;
            }
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 4) {
            MostrarAlertasParaTutor(db, usuario); // <--- NUEVA LLAMADA
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        }
    } while(opcion != 5); 
}

// --- COORDINADOR ---
void Coordinador::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE COORDINADOR ===\n";
        cout << "1. Gestionar Asignaciones\n";
        cout << "2. Ver Lista de Asignaciones\n";
        cout << "3. Ver Resultados de Encuestas\n"; 
        cout << "4. Cerrar Sesión\n";
        cout << "Opción: ";
        
        if (!(cin >> opcion)) { 
            limpiarBuffer(); 
            continue; 
        }

        if (opcion == 1) {
            RealizarAsignacion(db); 
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 2) {
            VerAsignaciones(db);
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 3) {
            VerResultadosEncuestas(db);
            cout << "\nPresiona Enter para continuar.";
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 4);
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
    iniciarBaseDeDatos(db);
}

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
        cout << "\n Login correcto. Rol detectado: " << rolDB << endl;

        if (rolDB == "alumno") {
            usuarioLogueado = new Alumno(u, p, "Ingenieria", true);
        } else if (rolDB == "tutor") {
            usuarioLogueado = new Tutor(u, p, "Dpto. Informatica");
        } else if (rolDB == "admin") {
            usuarioLogueado = new Coordinador(u, p);
        }
    } else {
        cout << "\n Usuario o contraseña incorrectos.\n";
        cout << "Presiona Enter...";
        limpiarBuffer(); cin.get();
    }

    sqlite3_finalize(stmt);
    return usuarioLogueado;
}

void registrarse() {
    string u, p;
    cout << "\n--- REGISTRO DE NUEVO ALUMNO ---\n";
    cout << "Usuario: "; cin >> u;
    cout << "Contraseña: "; cin >> p;

    string sql = "INSERT INTO usuarios (usuario, pass, rol) VALUES (?, ?, 'alumno');";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, u.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, p.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            int id_generado = sqlite3_last_insert_rowid(db);
            cout << " Registrado con éxito! ID: [ " << id_generado << " ]" << endl;
        } else {
            cout << " Error: El usuario ya existe." << endl;
        }
    }
    sqlite3_finalize(stmt);
}

int MostrarDatos (void *NotUsed, int argc, char **argv, char **azColName){
    cout << "Guardado a : " << (argv[4] ? argv[4] : "?") << " | ";
    cout << "Tutor: " << (argv[2] ? argv[2] : "SinNombre") << " (ID " << (argv[0] ? argv[0] : "?") << ") ";
    cout << "--> Alumno: " << (argv[3] ? argv[3] : "SinNombre") << " (ID " << (argv[1] ? argv[1] : "?") << ")" << endl;
    return 0; 
}

void iniciarBaseDeDatos(sqlite3 *db) {
    char *error = 0;
    const char* queries[] = {
        "CREATE TABLE IF NOT EXISTS usuarios (id INTEGER PRIMARY KEY AUTOINCREMENT, usuario TEXT UNIQUE, pass TEXT, rol TEXT);",
        "CREATE TABLE IF NOT EXISTS asignaciones (id INTEGER PRIMARY KEY AUTOINCREMENT, id_tutor INTEGER, nombre_tutor TEXT, id_alumno INTEGER, nombre_alumno TEXT, fecha TEXT DEFAULT CURRENT_TIMESTAMP);",
        "CREATE TABLE IF NOT EXISTS encuestas (id INTEGER PRIMARY KEY AUTOINCREMENT, id_tutor INTEGER, id_alumno INTEGER, puntuacion INTEGER, comentario TEXT, fecha TEXT DEFAULT CURRENT_TIMESTAMP);",
        "CREATE TABLE IF NOT EXISTS actas (id INTEGER PRIMARY KEY AUTOINCREMENT, id_tutor INTEGER, id_alumno INTEGER, fecha TEXT, resumen TEXT);",
        "CREATE TABLE IF NOT EXISTS alertas (id INTEGER PRIMARY KEY AUTOINCREMENT, emisor_usuario TEXT, categoria TEXT, descripcion TEXT, fecha TEXT DEFAULT CURRENT_TIMESTAMP, estado TEXT DEFAULT 'ABIERTA');",
        "CREATE TABLE IF NOT EXISTS mensajes (id INTEGER PRIMARY KEY AUTOINCREMENT, remitente TEXT, receptor TEXT, contenido TEXT, fecha TEXT DEFAULT CURRENT_TIMESTAMP);"
    };
    for(int i=0; i<6; i++) {
        sqlite3_exec(db, queries[i], 0, 0, &error);
    }
}

bool verificarUsuario (sqlite3* db, int id, string rolEsperado) {
    sqlite3_stmt* stmt;
    string sql = "SELECT count(*) FROM usuarios WHERE id = ? AND rol = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, rolEsperado.c_str(), -1, SQLITE_STATIC);
    bool existe = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt, 0) > 0) existe = true;
    }
    sqlite3_finalize(stmt);
    return existe;
}

bool Asignado (sqlite3* db, int id_alumno) {
    sqlite3_stmt* stmt;
    string sql = "SELECT count(*) FROM asignaciones WHERE id_alumno = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id_alumno);
    bool tiene = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt, 0) > 0) tiene = true;
    }
    sqlite3_finalize(stmt);
    return tiene;
}

void RegistrarActa(sqlite3 *db, string tutor_usuario){
    sqlite3_stmt* stmt;
    int id_tutor = -1;
    string sql_tutor = "SELECT id FROM usuarios WHERE usuario = ?;";
    if (sqlite3_prepare_v2(db, sql_tutor.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) id_tutor = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (id_tutor == -1) return;

    string sql_lista = "SELECT id_alumno, nombre_alumno FROM asignaciones WHERE id_tutor = ?;";
    if (sqlite3_prepare_v2(db, sql_lista.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        cout << "Seleccione el alumno:" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << " [" << sqlite3_column_int(stmt, 0) << "] " << sqlite3_column_text(stmt, 1) << endl;
        }
    }
    sqlite3_finalize(stmt);

    int id_alumno_elegido;
    string fecha, resumen;
    cout << "ID del alumno: "; cin >> id_alumno_elegido;
    limpiarBuffer();
    cout << "Fecha (DD/MM/AAAA): "; getline(cin, fecha);
    do { cout << "Resumen (Obligatorio): "; getline(cin, resumen); } while (resumen.empty());

    string sql_guardar = "INSERT INTO actas (id_tutor, id_alumno, fecha, resumen) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql_guardar.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        sqlite3_bind_int(stmt, 2, id_alumno_elegido);
        sqlite3_bind_text(stmt, 3, fecha.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, resumen.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) cout << " ACTA REGISTRADA CORRECTAMENTE." << endl;
    }
    sqlite3_finalize(stmt);
}

void RealizarAsignacion(sqlite3 *db) {
    int id_tutor, id_alumno;
    string nom_tutor, nom_alumno, opcion;
    char *error = 0;

    cout << ">> ID Alumno: "; cin >> id_alumno;
    limpiarBuffer();
    if (!verificarUsuario(db, id_alumno, "alumno")) return;

    if (Asignado(db, id_alumno)) {
        cout << "Ya tiene tutor. ¿Cambiar? (s/n): "; cin >> opcion;
        if (opcion != "s" && opcion != "S") return;
    }
    cout << ">> Nombre Alumno: "; getline(cin, nom_alumno);
    cout << ">> ID Tutor: "; cin >> id_tutor;
    limpiarBuffer();
    if (!verificarUsuario(db, id_tutor, "tutor")) return;
    cout << ">> Nombre Tutor: "; getline(cin, nom_tutor);

    string sql = "INSERT OR REPLACE INTO asignaciones (id_tutor, id_alumno, nombre_tutor, nombre_alumno) VALUES (" 
              + to_string(id_tutor) + ", " + to_string(id_alumno) + ", '" + nom_tutor + "', '" + nom_alumno + "');";

    sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    cout << " Asignación realizada correctamente." << endl;
}

void VerAsignaciones(sqlite3 *db) {
    char *error = 0;
    cout << "--- LISTA DE ASIGNACIONES ---" << endl;
    sqlite3_exec(db, "SELECT id_tutor, id_alumno, nombre_tutor, nombre_alumno, fecha FROM asignaciones;", MostrarDatos, 0, &error);
}

void MostrarTutorAsignado(sqlite3 *db, string alumno_usuario){
    sqlite3_stmt* stmt;
    int id_alumno = -1;
    string sql_id = "SELECT id FROM usuarios WHERE usuario = ?;";
    if (sqlite3_prepare_v2(db, sql_id.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, alumno_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) id_alumno = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (id_alumno == -1) return;

    string sql_asig = "SELECT nombre_tutor FROM asignaciones WHERE id_alumno = ?;";
    if (sqlite3_prepare_v2(db, sql_asig.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_alumno);
        if (sqlite3_step(stmt) == SQLITE_ROW) cout << " Tu tutor es: " << sqlite3_column_text(stmt, 0) << endl;
    }
    sqlite3_finalize(stmt);
}

void MostrarAlumnosAsignados(sqlite3 *db, string tutor_usuario){
    sqlite3_stmt* stmt;
    int id_tutor = -1;
    string sql_id = "SELECT id FROM usuarios WHERE usuario = ?;";
    if (sqlite3_prepare_v2(db, sql_id.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) id_tutor = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (id_tutor == -1) return;

    string sql_lista = "SELECT nombre_alumno FROM asignaciones WHERE id_tutor = ?;";
    if (sqlite3_prepare_v2(db, sql_lista.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        while (sqlite3_step(stmt) == SQLITE_ROW) cout << " Alumno: " << sqlite3_column_text(stmt, 0) << endl;
    }
    sqlite3_finalize(stmt);
}

void Encuesta(sqlite3 *db, string alumno_usuario) {
    sqlite3_stmt* stmt;
    int id_alumno = -1, id_tutor = -1, puntuacion;
    string nombre_tutor, comentario, sql_user = "SELECT id FROM usuarios WHERE usuario = ?;";
    
    if (sqlite3_prepare_v2(db, sql_user.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, alumno_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) id_alumno = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    string sql_asig = "SELECT id_tutor, nombre_tutor FROM asignaciones WHERE id_alumno = ?;";
    if (sqlite3_prepare_v2(db, sql_asig.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_alumno);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_tutor = sqlite3_column_int(stmt, 0);
            nombre_tutor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        }
    }
    sqlite3_finalize(stmt);

    if (id_tutor == -1) return;
    do { cout << "Puntuación (1-5): "; cin >> puntuacion; } while (puntuacion < 1 || puntuacion > 5);
    limpiarBuffer();
    cout << "Comentario: "; getline(cin, comentario);

    string sql_ins = "INSERT INTO encuestas (id_tutor, id_alumno, puntuacion, comentario) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql_ins.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        sqlite3_bind_int(stmt, 2, id_alumno);
        sqlite3_bind_int(stmt, 3, puntuacion);
        sqlite3_bind_text(stmt, 4, comentario.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        cout << " ENCUESTA ENVIADA CON ÉXITO!" << endl;
    }
    sqlite3_finalize(stmt);
}

void VerResultadosEncuestas(sqlite3 *db) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT id_tutor, puntuacion, comentario FROM encuestas;", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << " Tutor: " << sqlite3_column_int(stmt, 0) << " | Nota: " << sqlite3_column_int(stmt, 1) << " | Comentario: " << sqlite3_column_text(stmt, 2) << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// --- CHAT ---
string ObtenerTutorAsignadoUsuario(sqlite3 *db, string alumno_usuario) {
    sqlite3_stmt* stmt; string tutor_usuario = "";
    string sql = "SELECT T.usuario FROM asignaciones A JOIN usuarios T ON A.id_tutor = T.id JOIN usuarios AL ON A.id_alumno = AL.id WHERE AL.usuario = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, alumno_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) tutor_usuario = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return tutor_usuario;
}

string ObtenerAlumnoAsignadoUsuario(sqlite3 *db, string tutor_usuario) {
    sqlite3_stmt* stmt; string alumno_usuario = "";
    string sql = "SELECT AL.usuario FROM asignaciones A JOIN usuarios AL ON A.id_alumno = AL.id JOIN usuarios T ON A.id_tutor = T.id WHERE T.usuario = ? LIMIT 1;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) alumno_usuario = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return alumno_usuario;
}

void EnviarMensajeChat(sqlite3* db, string rem, string rec, const string& msj) {
    sqlite3_stmt* stmt;
    string sql = "INSERT INTO mensajes (remitente, receptor, contenido) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, rem.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, rec.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, msj.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void MostrarHistorialChat(sqlite3* db, string u1, string u2) {
    sqlite3_stmt* stmt; string msj;
    string sql = "SELECT remitente, contenido FROM mensajes WHERE (remitente=? AND receptor=?) OR (remitente=? AND receptor=?) ORDER BY fecha ASC;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, u1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, u2.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, u2.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, u1.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) cout << sqlite3_column_text(stmt, 0) << ": " << sqlite3_column_text(stmt, 1) << endl;
    }
    sqlite3_finalize(stmt);
    cout << "Mensaje: "; getline(cin >> ws, msj);
    if (msj != "salir") EnviarMensajeChat(db, u1, u2, msj);
}

// --- ALERTAS ---
string CategoriaAlertaToString(CategoriaAlerta cat) {
    if(cat == CategoriaAlerta::Salud) return "Salud";
    if(cat == CategoriaAlerta::Convivencia) return "Convivencia";
    return "Academico";
}

void GenerarAlertaIncidencia(sqlite3* db, string emisor) {
    int c; string d; sqlite3_stmt* stmt;
    cout << "Categoría (1.Salud 2.Convivencia 3.Académico): "; cin >> c;
    limpiarBuffer();
    if (c < 1 || c > 3) return;
    cout << "Descripción: "; getline(cin, d);
    string cat = CategoriaAlertaToString(static_cast<CategoriaAlerta>(c));
    string sql = "INSERT INTO alertas (emisor_usuario, categoria, descripcion) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, emisor.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, cat.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, d.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt); cout << " ALERTA ENVIADA." << endl;
    }
    sqlite3_finalize(stmt);
}

// --- NUEVA FUNCIÓN SOLICITADA ---
void MostrarAlertasParaTutor(sqlite3* db, string tutor_usuario) {
    sqlite3_stmt* stmt;
    string sql = "SELECT a.emisor_usuario, a.categoria, a.descripcion, a.fecha "
                 "FROM alertas a "
                 "JOIN asignaciones asig ON a.emisor_usuario = asig.nombre_alumno "
                 "JOIN usuarios u_tutor ON asig.id_tutor = u_tutor.id "
                 "WHERE u_tutor.usuario = ?;";

    cout << "\n--- ALERTAS DE MIS ALUMNOS ASIGNADOS ---\n";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        bool hay = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hay = true;
            cout << "\n Alumno: " << sqlite3_column_text(stmt, 0) << " | Tipo: " << sqlite3_column_text(stmt, 1);
            cout << "\n Descripción: " << sqlite3_column_text(stmt, 2) << " | Fecha: " << sqlite3_column_text(stmt, 3) << endl;
        }
        if(!hay) cout << "No hay alertas registradas para tus alumnos." << endl;
    }
    sqlite3_finalize(stmt);
}

bool esPuntuacionValida(int p) { return p >= 1 && p <= 5; }
bool esTextoValido(string t) { return !t.empty() && t.length() <= 200; }