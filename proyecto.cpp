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
// --- ALUMNO ---
void Alumno::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        // 'usuario' es la variable heredada de la clase padre Usuario
        cout << "\n=== PANEL DE ESTUDIANTE: " << usuario << " ===\n";
        cout << "1. Ver mi Tutor Asignado\n";            // <--- NUEVO: Antes no podías ver quién era
        cout << "2. Realizar Encuesta de Satisfacción\n"; // <--- INTEGRADO: Tu función de encuesta
        cout << "3. Abrir Chat con Tutor\n";
        cout << "4. Generar Alerta de Incidencia\n";
        cout << "5. Cerrar Sesión\n";
        cout << "Opción: ";

        // Validación básica de entrada para evitar bucles infinitos si meten letras
        if (!(cin >> opcion)) {
            cout << "Entrada inválida.\n";
            limpiarBuffer();
            continue;
        }

        if (opcion == 1) {
            // Llamamos a la función global que definiste abajo
            // Pasamos 'db' (global) y 'usuario' (atributo de la clase)
            MostrarTutorAsignado(db, usuario);
            
            cout << "\nPresiona Enter para volver al menú.";
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 2) {
            // Llamamos a tu función de encuesta
            Encuesta(db, usuario);
            
            // La propia función Encuesta ya tiene couts, pero añadimos pausa
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
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 5);
}


void Alumno::generarAlerta() {
    cout << "\n>> Enviando notificación urgente al Coordinador...\n";
    cout << ">> Incidencia registrada con éxito.\n";
}

// --- TUTOR ---
// --- TUTOR (CORREGIDO) ---
void Tutor::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE TUTOR: " << usuario << " ===\n";
        cout << "1. Ver Alumnos Asignados\n";
        cout << "2. Registrar Acta de Sesión\n";
        cout << "3. Abrir Chat con Alumno\n"; // Nuevo: Opción de Chat
        cout << "4. Cerrar Sesión\n"; 
        cout << "Opción: ";
        
        // === ESTO ES LO QUE TE FALTABA ===
        // Sin esto, el bucle es infinito porque nunca lee el número
        if (!(cin >> opcion)) {
            cout << "Entrada inválida.\n";
            limpiarBuffer();
            continue;
        }
        // =================================

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
    } while(opcion != 4); // Si no lee el 4, nunca sale de aquí
}

// --- Lógica del COORDINADOR (ADMIN) ---
void Coordinador::mostrarMenu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "\n=== PANEL DE COORDINADOR ===\n";
        cout << "1. Gestionar Asignaciones\n";
        cout << "2. Ver Lista de Asignaciones\n";
        cout << "3. Ver Resultados de Encuestas\n"; // <--- AÑADE ESTO
        cout << "4. Cerrar Sesión\n";
        cout << "Opción: ";
        
        if (!(cin >> opcion)) { /* ... validación ... */ }

        if (opcion == 1) {
            RealizarAsignacion(db); 
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 2) {
            VerAsignaciones(db);
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 3) {
            // === CONECTA LA FUNCIÓN AQUÍ ===
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
    
    // Crear tabla si no existe
    string sql = "CREATE TABLE IF NOT EXISTS usuarios ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "usuario TEXT UNIQUE, pass TEXT, rol TEXT);";
    char* err;
    sqlite3_exec(db, sql.c_str(), 0, 0, &err);

    iniciarBaseDeDatos(db);
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
        cout << "\n Usuario o contraseña incorrectos.\n";
        cout << "Presiona Enter...";
        limpiarBuffer(); cin.get();
    }

    sqlite3_finalize(stmt);
    return usuarioLogueado;
}

void registrarse() {
    cout << "\n--- REGISTRO DE NUEVO ALUMNO ---\n";
    string u, p;
    cout << "Usuario: "; cin >> u;
    cout << "Contraseña: "; cin >> p;

    string rol = "alumno"; 
    string sql = "INSERT INTO usuarios (usuario, pass, rol) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, u.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, p.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, rol.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "¡Registro exitoso! Por favor inicia sesión en el menú principal.\n";
        } else {
            cout << "Error: El usuario ya existe.\n";
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
    char *mensajeError = 0; 

    //crea uan tabla en la base de datos coon todos los apartados necesarios para la asignaicon de tutores
    string sql_asignacion = "CREATE TABLE IF NOT EXISTS asignaciones ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "id_tutor INTEGER, "
                      "nombre_tutor TEXT, "   
                      "id_alumno INTEGER, "
                      "nombre_alumno TEXT, " 
                      "fecha TEXT DEFAULT CURRENT_TIMESTAMP);";

   int resultado = sqlite3_exec(db, sql_asignacion.c_str(), 0, 0, &error);
    if (resultado != SQLITE_OK) {
        cout << "Error tabla asignaciones: " << error << endl;
        sqlite3_free(error);
    }

        string sql_encuesta = "CREATE TABLE IF NOT EXISTS encuestas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "id_tutor INTEGER, "     // Id del tutor que ha sido evaluado 
                          "id_alumno INTEGER, "    // Id del alumno que realiza la encuesta
                          "puntuacion INTEGER, "   // Valoracion (1 al 5)
                          "comentario TEXT, "      // Comentario adicional
                          "fecha TEXT DEFAULT CURRENT_TIMESTAMP);";

        
        resultado = sqlite3_exec(db, sql_encuesta.c_str(), 0, 0, &error);
    if (resultado != SQLITE_OK) {
        cout << "Error tabla encuestas: " << error << endl;
        sqlite3_free(error);
    }

    //Crear la tabla para registrar el acta
    string sql_actas = "CREATE TABLE IF NOT EXISTS actas ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "id_tutor INTEGER, "
                       "id_alumno INTEGER, "
                       "fecha TEXT, "
                       "resumen TEXT);";

    sqlite3_exec(db, sql_actas.c_str(), 0, 0, &mensajeError);

    // NUEVA TABLA: ALERTA DE INCIDENCIAS (CU-04)
    string sql_alertas = "CREATE TABLE IF NOT EXISTS alertas ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "emisor_usuario TEXT, "
                         "categoria TEXT, "       // Salud, Convivencia, Academico
                         "descripcion TEXT, "     // Descripción opcional
                         "fecha TEXT DEFAULT CURRENT_TIMESTAMP, "
                         "estado TEXT DEFAULT 'ABIERTA');";
    
    resultado = sqlite3_exec(db, sql_alertas.c_str(), 0, 0, &error);
    if (resultado != SQLITE_OK) {
        cout << "Error tabla alertas: " << error << endl;
        sqlite3_free(error);
    }
    
    // NUEVA TABLA: CHAT / MENSAJES (CU-02)
    // Nota: Simplificamos el chat a un registro de mensajes bidireccionales
    string sql_chat = "CREATE TABLE IF NOT EXISTS mensajes ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "remitente TEXT, " 
                      "receptor TEXT, " 
                      "contenido TEXT, " 
                      "fecha TEXT DEFAULT CURRENT_TIMESTAMP, "
                      "archivo_adjunto TEXT DEFAULT '',"
                      "es_alerta INTEGER DEFAULT 0);"; // 0 o 1
    
    resultado = sqlite3_exec(db, sql_chat.c_str(), 0, 0, &error);
    if (resultado != SQLITE_OK) {
        cout << "Error tabla mensajes: " << error << endl;
        sqlite3_free(error);
    }
}



bool verificarUsuario (sqlite3* db, int id, string rolEsperado) {
    sqlite3_stmt* stmt;
string sql = "SELECT count(*) FROM usuarios WHERE id = ? AND rol = ?;";
    
    // Preparamos la consulta
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) return false;
    
    // Rellenamos los huecos (?) con los datos
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, rolEsperado.c_str(), -1, SQLITE_STATIC);
    
    bool existe = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Si el conteo es mayor que 0, es que existe
        if (sqlite3_column_int(stmt, 0) > 0) existe = true;
    }
    sqlite3_finalize(stmt); // Limpiamos memoria
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


// REGISTRAR ACTA (CU-03)
void RegistrarActa(sqlite3 *db, string tutor_usuario){
    sqlite3_stmt* stmt;
    int id_tutor = -1;

cout << "---- REGISTRO DE ACTA----" <<endl; 

// OBTENER ID DEL TUTOR
    string sql_tutor = "SELECT id FROM usuarios WHERE usuario = ?;";
    if (sqlite3_prepare_v2(db, sql_tutor.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_tutor = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt); //LImpiar buffer 

    if (id_tutor == -1) { cout << "ERROR, Usuario no identificado." << endl; return; }

    //  MOSTRAR LISTA DE ALUMNOS ASIGNADOS
    cout << "Seleccione el alumno con el que se ha reunido:" << endl;
    cout << "---------------------------------------------" << endl;
    
    string sql_lista = "SELECT id_alumno, nombre_alumno FROM asignaciones WHERE id_tutor = ?;";
    bool tiene_alumnos = false;

    if (sqlite3_prepare_v2(db, sql_lista.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id_a = sqlite3_column_int(stmt, 0);
            
            string nom = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            cout << " [" << id_a << "] " << nom << endl;
            tiene_alumnos = true;
        }
    }
    sqlite3_finalize(stmt);

    if (!tiene_alumnos) {
        cout << " No tienes alumnos asignados. No puedes crear actas." << endl;
        return;
    }

    // PEDIR DATOS DEL ACTA
    int id_alumno_elegido;
    string fecha, resumen;

    cout << "\nIndica el ID del alumno: ";
    if (!(cin >> id_alumno_elegido)) {
        cout << " Error: ID inválido." << endl; 
        cin.clear(); cin.ignore(10000, '\n'); return;
    }
    cin.ignore(10000, '\n'); // Limpiar buffer para no mezlcar con getline

    cout << "Fecha de la reunión (DD/MM/AAAA): ";
    getline(cin, fecha);


    do {
        cout << "Resumen / Problemas tratados (Obligatorio): ";
        getline(cin, resumen);
        
        if (resumen.empty()) {
            cout << " Error: El resumen no puede estar vacío. Inténtalo de nuevo." << endl;
        }
    } while (resumen.empty());

    // GUARDAR EN LA BASE DE DATOS
    string sql_guardar = "INSERT INTO actas (id_tutor, id_alumno, fecha, resumen) VALUES (?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql_guardar.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        sqlite3_bind_int(stmt, 2, id_alumno_elegido);
        sqlite3_bind_text(stmt, 3, fecha.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, resumen.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << " ACTA REGISTRADA CORRECTAMENTE." << endl;
            cout << "Se ha guardado constancia de la reunión." << endl;
        } else {
            cout << " Error, el acta no se ha guardado ." << endl;
        }
    } else {
        cout << "Error SQL al preparar guardado." << endl;
    }
    sqlite3_finalize(stmt);
}





// 1. Comprueba si un nombre existe en la tabla de usuarios
bool existeUsuarioPorNombre(sqlite3* db, string nombre, string rol) {
    sqlite3_stmt* stmt;
    string sql = "SELECT count(*) FROM usuarios WHERE usuario = ? AND rol = ?;";
    bool existe = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nombre.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, rol.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            if (sqlite3_column_int(stmt, 0) > 0) existe = true;
        }
    }
    sqlite3_finalize(stmt);
    return existe;
}

// 2. Comprueba si un alumno (por nombre) ya tiene tutor asignado
bool yaTieneTutor(sqlite3* db, string nombreAlumno) {
    sqlite3_stmt* stmt;
    string sql = "SELECT count(*) FROM asignaciones WHERE nombre_alumno = ?;";
    bool tiene = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nombreAlumno.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            if (sqlite3_column_int(stmt, 0) > 0) tiene = true;
        }
    }
    sqlite3_finalize(stmt);
    return tiene;
}



// función para introducir los datos y la asignación de tutor y alumno 
// CONSULTAS DE ASIGNACION CU-04)

// CU-04: REALIZAR ASIGNACIÓN 
// --- CU-04: REALIZAR ASIGNACIÓN (SOLO NOMBRES + CONTROL TOTAL) ---
void RealizarAsignacion(sqlite3 *db) {
    string nom_alumno, nom_tutor, opcion;
    char *error = 0;

    cout << "\n=== ASIGNAR TUTOR POR NOMBRE ===" << endl;

    // --- 1. PEDIR NOMBRE ALUMNO ---
    cout << ">> Nombre del usuario Alumno: "; 
    cin >> nom_alumno;

    // Verificamos si existe (usando el nombre)
    if (!existeUsuarioPorNombre(db, nom_alumno, "alumno")) {
        cout << "❌ Error: El alumno '" << nom_alumno << "' no existe." << endl;
        return; 
    }

    // --- 2. TU CONTROL: ¿YA TIENE TUTOR? ---
    // Aquí usamos la función que busca por nombre, no por ID
    bool modificar = false;
    if (yaTieneTutor(db, nom_alumno)) {
        cout << "⚠️ AVISO: " << nom_alumno << " ya tiene un tutor asignado." << endl;
        cout << "   ¿Quieres CAMBIARLO? (s/n): ";
        cin >> opcion;
        if (opcion != "s" && opcion != "S") return; // Si dices que no, no hace nada
        modificar = true;
    }

    // --- 3. PEDIR NOMBRE TUTOR ---
    cout << ">> Nombre del usuario Tutor: "; 
    cin >> nom_tutor;

    // Verificamos si existe (usando el nombre)
    if (!existeUsuarioPorNombre(db, nom_tutor, "tutor")) {
        cout << "❌ Error: El tutor '" << nom_tutor << "' no existe." << endl;
        return;
    }

    // --- 4. GUARDAR EN LA BASE DE DATOS ---
    string sql;
    
    // NOTA: Aunque tú no uses IDs, la base de datos necesita rellenar esa columna.
    // Usamos este truco SQL: "(SELECT id FROM...)" para que la BD busque el número ella sola.
    
    if (modificar) {
        // ACTUALIZAR (UPDATE)
        sql = "UPDATE asignaciones SET "
              "nombre_tutor = '" + nom_tutor + "', "
              "id_tutor = (SELECT id FROM usuarios WHERE usuario = '" + nom_tutor + "'), " // Truco SQL
              "fecha = CURRENT_TIMESTAMP "
              "WHERE nombre_alumno = '" + nom_alumno + "';";
    } else {
        // INSERTAR NUEVO (INSERT)
        sql = "INSERT INTO asignaciones (id_tutor, id_alumno, nombre_tutor, nombre_alumno) VALUES (" 
              "(SELECT id FROM usuarios WHERE usuario = '" + nom_tutor + "'), "  // Truco SQL
              "(SELECT id FROM usuarios WHERE usuario = '" + nom_alumno + "'), " // Truco SQL
              "'" + nom_tutor + "', '" + nom_alumno + "');";
    }

    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    
    if (rc != SQLITE_OK) {
        cout << "❌ Error SQL: " << error << endl;
        sqlite3_free(error);
    } else {
        cout << "✅ ¡Asignación guardada con éxito!" << endl;
        cout << "   Tutor: " << nom_tutor << "  --->  Alumno: " << nom_alumno << endl;
    }
}



// FUNCION PARA QUE EL TUTOR PUEDA VER SUS ALUMNOS ASIGNADOS
void MostrarAlumnosAsignados(sqlite3 *db, string tutor_usuario){
    sqlite3_stmt* stmt;
    int id_tutor = -1;

    cout << " --- ALUMNOS ASIGNADOS --- " << endl;
    
    //Obtener el ID del tutor a traves del nombre registrado
    string sql_id = "SELECT id FROM usuarios WHERE usuario = ?;";
    if (sqlite3_prepare_v2(db, sql_id.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_tutor = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt); //Limpia el buffer
    if (id_tutor == -1) {
        cout << "ERROR, no ha sido encontrado el usuario." << endl;
        return;
    }

    string sql_lista = "SELECT nombre_alumno, fecha FROM asignaciones WHERE id_tutor = ?;";
    if (sqlite3_prepare_v2(db, sql_lista.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        
        int contador = 0;
        cout << "Listado de alumnos a tu cargo:" << endl;
        cout << "---------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string alumno = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            string fecha = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            
            cout << " El alumno:  " << alumno << "  está asignado desde: " << fecha << " " << endl;
            contador++;
        }
        
        if (contador == 0) {
            cout << "NO tienes ningún alumno asignado." << endl;
        }
    } else {
        cout << "Error al consultar la lista de alumnos." << endl;
    }
    sqlite3_finalize(stmt);
}


 // FUNCIONES DE CREAR ENCUESTA (CU-07)
void Encuesta(sqlite3 *db, string alumno_usuario) {
    sqlite3_stmt* stmt;
    int id_alumno = -1;
    int id_tutor = -1; // Inicializamos todo a -1 para evitar errores 
    string nombre_tutor = "";

    cout << " -----REALIZAR ENCUESTA DE SATISFACCION----- " << endl;

    //Obtener el ID del alumno a traves del nombre registrado 
    string sql_user = "SELECT id FROM usuarios WHERE usuario = ?;";
    if (sqlite3_prepare_v2(db, sql_user.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, alumno_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_alumno = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt); // Limpiar la memoria para no mezclar consultas

    // verificar previamente que el alumno existe 
    if (id_alumno == -1) {
        cout << " Error interno: No se encuentra tu perfil del alumno." << endl;
        return;
    }

    // busqueda del tutor asignado al alumno 
    //FUncion que revisa sin en la tabla el alumno tiene un tutor asignado
    string sql_asig = "SELECT id_tutor, nombre_tutor FROM asignaciones WHERE id_alumno = ?;";
    
    if (sqlite3_prepare_v2(db, sql_asig.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_alumno);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_tutor = sqlite3_column_int(stmt, 0);
            // Truco para leer texto de SQLite a C++ string de forma segura:
            const unsigned char* texto = sqlite3_column_text(stmt, 1);
            nombre_tutor = string(reinterpret_cast<const char*>(texto));
        }
    }
    sqlite3_finalize(stmt);

//Verifica la existencia del tutor 
    if (id_tutor == -1) {
        cout << " Este alumno no tiene un tutor asignado todavia." << endl;
        cout << "   No puedes realizar la encuesta todavía." << endl;
        return; // Sacamos al usuario de la función
    }

    cout << "Hola " << alumno_usuario << ", vas a valorar a tu tutor: " << nombre_tutor << endl;

    int puntuacion; 
    string comentario; 
    bool puntuacion_valida = false;

    do{
        cout << "Introduzca una puntuacion para su tutor del 1 al 5: " << endl; 
        cin >> puntuacion;
        if (!(cin >> puntuacion)) {
    
            cout << " ERROR, Introduzca un numero válido" << endl;
            cin.clear(); 
            cin.ignore(10000, '\n'); 
        }
        else if (puntuacion < 1 || puntuacion > 5) {
            cout << "ERROR, introduzca un número entre (1-5)." << endl;
        } 
        else {
            puntuacion_valida = true;
        }
    } while (!puntuacion_valida);


    cin.ignore(); // Limpia el buffer para que no haya problemas de lectura 
    cout << " Escriba un comentario sobre su experencia (opcional): " << endl;
    getline (cin, comentario);


    //Guardar en la base de datos 
    cout << "Guardando tu respuesta..." << endl;

    string sql_guardar = "INSERT INTO encuestas (id_tutor, id_alumno, puntuacion, comentario) VALUES (?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql_guardar.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        sqlite3_bind_int(stmt, 2, id_alumno);
        sqlite3_bind_int(stmt, 3, puntuacion);
        sqlite3_bind_text(stmt, 4, comentario.c_str(), -1, SQLITE_STATIC);
    int resultado = sqlite3_step(stmt);
        
        if (resultado == SQLITE_DONE) {
            cout << " ¡ENCUESTA ENVIADA CON ÉXITO!" << endl;
            cout << "Gracias por ayudarnos a mejorar la calidad de las tutorías." << endl;
        } else {
            cout << " Error al guardar la encuesta en la base de datos." << endl;
        }
    } else {
        cout << "Error al preparar la consulta de guardado." << endl;
    }
    
    sqlite3_finalize(stmt); //limpia para dejar el buffer vacio
}


// REVISION DE LOS RESULTADOS DE LAS ENCUESTAS 

void VerResultadosEncuestas(sqlite3 *db) {
    sqlite3_stmt* stmt;

    cout << "--- RESULTADOS DE LAS ENCUESTAS DE SATISFACCIÓN ---" << endl;
   string sql = "SELECT id_tutor, id_alumno, puntuacion, comentario, fecha FROM encuestas;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        
        int contador = 0;
        double suma_notas = 0;

        cout << "ID Tutor | ID Alumno | Nota | Comentario" << endl;
        cout << "----------------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id_t = sqlite3_column_int(stmt, 0);
            int id_a = sqlite3_column_int(stmt, 1);
            int nota = sqlite3_column_int(stmt, 2);
            
            // Leer texto de forma segura
            string coment = "Sin comentario";
            if (sqlite3_column_text(stmt, 3)) {
                coment = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            }

            cout << "   " << id_t << "     |    " << id_a << "      |  " << nota << "   | " << coment << endl;

            suma_notas += nota;
            contador++;
        }

        cout << "----------------------------------------------------" << endl;
        
        if (contador > 0) {
            double media = suma_notas / contador;
            cout << " Total encuestas: " << contador << endl;
            cout << " Nota Media del Profesorado: " << media << " / 5" << endl;
        } else {
            cout << "(Aún no se han recibido encuestas)" << endl;
        }
        } else {
        cout << "Error al leer la tabla de encuestas." << endl;
    }
    sqlite3_finalize(stmt);
}


//PRUEBAS (CU-07) 

bool esPuntuacionValida(int puntuacion) {
    if (puntuacion >= 1 && puntuacion <= 5) return true;
    return false;
}

bool esTextoValido(string texto) {
    if (texto.empty()) return false;
    if (texto.length() > 200) return false;
    return true;
}


// ==========================================
// IMPLEMENTACIÓN DE CHAT Y ALERTAS
// ==========================================

// --- FUNCIONES DE CHAT (CU-02) ---

string ObtenerTutorAsignadoUsuario(sqlite3 *db, string alumno_usuario) {
    // Retorna el nombre de usuario del tutor asignado o "" si no tiene.
    sqlite3_stmt* stmt;
    string tutor_usuario = "";
    string sql = "SELECT T.usuario FROM asignaciones A JOIN usuarios T ON A.id_tutor = T.id JOIN usuarios AL ON A.id_alumno = AL.id WHERE AL.usuario = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, alumno_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            tutor_usuario = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return tutor_usuario;
}

string ObtenerAlumnoAsignadoUsuario(sqlite3 *db, string tutor_usuario) {
    // Retorna el nombre de usuario del primer alumno asignado o "" si no tiene.
    sqlite3_stmt* stmt;
    string alumno_usuario = "";
    // Simplificación: Solo devuelve el primer alumno si el tutor tiene varios.
    string sql = "SELECT AL.usuario FROM asignaciones A JOIN usuarios AL ON A.id_alumno = AL.id JOIN usuarios T ON A.id_tutor = T.id WHERE T.usuario = ? LIMIT 1;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tutor_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            alumno_usuario = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return alumno_usuario;
}

void MostrarHistorialChat(sqlite3* db, string usuario_actual, string otro_usuario) {
    sqlite3_stmt* stmt;
    string mensaje_entrada;

    // La consulta selecciona mensajes donde el remitente Y el receptor son A y B (bidireccional)
    string sql = "SELECT remitente, contenido, fecha FROM mensajes WHERE (remitente = ? AND receptor = ?) OR (remitente = ? AND receptor = ?) ORDER BY fecha ASC;";
    
    limpiarPantalla();
    cout << "=== CHAT PERSONAL: " << otro_usuario << " ===\n";
    cout << "------------------------------------------\n";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        // Enlazar los 4 parámetros (bidireccional)
        sqlite3_bind_text(stmt, 1, usuario_actual.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, otro_usuario.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, otro_usuario.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, usuario_actual.c_str(), -1, SQLITE_STATIC);

        int contador = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string remitente = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            string contenido = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string fecha = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

            if (remitente == usuario_actual) {
                cout << " >> YO (" << fecha.substr(11, 5) << "): " << contenido << endl;
            } else {
                cout << " << " << remitente << " (" << fecha.substr(11, 5) << "): " << contenido << endl;
            }
            contador++;
        }
        sqlite3_finalize(stmt);

        if (contador == 0) {
            cout << " * Comienza una nueva conversación. *\n";
        }
        cout << "------------------------------------------\n";

        // Bucle de envío de mensajes
        do {
            cout << "[" << usuario_actual << "] Mensaje (o 'salir'): ";
            // Usamos getline con ws para consumir el espacio en blanco inicial si existe
            getline(cin >> ws, mensaje_entrada);
            
            if (mensaje_entrada.empty()) continue;

            if (mensaje_entrada != "salir") {
                EnviarMensajeChat(db, usuario_actual, otro_usuario, mensaje_entrada);
                // Volver a mostrar el historial incluyendo el nuevo mensaje (simplificación)
                limpiarPantalla();
                MostrarHistorialChat(db, usuario_actual, otro_usuario); 
                return; // Salimos del bucle interno para simplificar
            }
        } while (mensaje_entrada != "salir");

    } else {
        cout << "Error al cargar el chat." << endl;
        sqlite3_finalize(stmt);
    }
}

void EnviarMensajeChat(sqlite3* db, string remitente_usuario, string receptor_usuario, const string& mensaje) {
    sqlite3_stmt* stmt;

    string sql = "INSERT INTO mensajes (remitente, receptor, contenido) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, remitente_usuario.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, receptor_usuario.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, mensaje.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            // cout << "Mensaje enviado." << endl; // Mensaje retirado para que no interfiera en la UI del chat
        } else {
            cout << "Error al enviar mensaje: " << sqlite3_errmsg(db) << endl;
        }
    }
    sqlite3_finalize(stmt);
}


// --- FUNCIONES DE ALERTA (CU-04) ---

string CategoriaAlertaToString(CategoriaAlerta cat) {
    switch(cat) {
        case CategoriaAlerta::Salud: return "Salud";
        case CategoriaAlerta::Convivencia: return "Convivencia";
        case CategoriaAlerta::Academico: return "Academico";
    }
    return "Desconocido";
}

void GenerarAlertaIncidencia(sqlite3* db, string emisor_usuario) {
    int cat_int;
    string descripcion;
    sqlite3_stmt* stmt;

    cout << "\n--- GENERAR ALERTA URGENTE ---" << endl;
    cout << "Categorías disponibles:" << endl;
    cout << " 1. Salud" << endl;
    cout << " 2. Convivencia" << endl;
    cout << " 3. Académico" << endl;
    cout << "Seleccione la categoría (1-3): ";

    if (!(cin >> cat_int) || cat_int < 1 || cat_int > 3) {
        cout << "Error: Categoría no válida." << endl;
        limpiarBuffer();
        return;
    }
    cin.ignore();
    
    CategoriaAlerta categoria = static_cast<CategoriaAlerta>(cat_int);
    string cat_str = CategoriaAlertaToString(categoria);

    cout << "Escriba una descripción detallada (opcional): ";
    getline(cin, descripcion);

    // 5. El sistema registra la alerta con fecha y emisor (RI-5)
    string sql_guardar = "INSERT INTO alertas (emisor_usuario, categoria, descripcion) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql_guardar.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, emisor_usuario.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, cat_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, descripcion.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            // 6. El sistema notifica inmediatamente al coordinador (simulación)
            cout << "\n🚨 ALERTA URGENTE ENVIADA." << endl;
            cout << "La incidencia de categoría '" << cat_str << "' ha sido notificada al Coordinador." << endl;
        } else {
            cout << "Error al registrar la alerta: " << sqlite3_errmsg(db) << endl;
        }
    } else {
        cout << "Error SQL al preparar la alerta." << endl;
    }
    sqlite3_finalize(stmt);
}