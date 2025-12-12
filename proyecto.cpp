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


// función para introducir los datos y la asignación de tutor y alumno 
void RealizarAsignacion(sqlite3 *db) {
    int id_tutor, id_alumno;
    string nom_tutor, nom_alumno;
    string opcion; 
    char *error = 0;

    // comprobar que el alumno se encuentra en la Base de datos 
    cout << ">> Introduce el ID del Alumno: "; 
    cin >> id_alumno;
    cin.ignore();
    if (!verificarUsuario(db, id_alumno, "alumno")) {
        cout << " Error: El alumno con ID " << id_alumno << " no esta registrado en la base de datos." << endl;
        return; 
    }

    // FUncion que verifica si el alumno ya le ha sido asignado un tutor 
    bool esModificacion = false;
    if (Asignado(db, id_alumno)) {
        cout << " Este alumno ya tiene  un tutor. ¿Deseas CAMBIARLO? (s/n): ";
        cin >> opcion;
        cin.ignore();
        if (opcion != "s" && opcion != "S") return; // Si dice que no, salimos
        esModificacion = true; // SI dice S, se procede a modgicar y actualizar el cambio 
    }
    cout << ">> Nombre del Alumno (para confirmar): "; 
    getline(cin, nom_alumno);

    // comprueba que el tutor esta registrado en la Base de datos 
    cout << ">> Introduce el ID del Tutor: "; 
    cin >> id_tutor;
    cin.ignore();
    
    if (!verificarUsuario(db, id_tutor, "tutor")) {
        cout << " Error: El tutor con ID " << id_tutor << " no existe." << endl;
        return;
    }

    cout << ">> Nombre del Tutor: "; 
    getline(cin, nom_tutor);
    string sql;
    if (esModificacion) {
        sql = "UPDATE asignaciones SET id_tutor = " + to_string(id_tutor) + 
              ", nombre_tutor = '" + nom_tutor + "', nombre_alumno = '" + nom_alumno + 
              "', fecha = CURRENT_TIMESTAMP WHERE id_alumno = " + to_string(id_alumno) + ";";
    } else {
        sql = "INSERT INTO asignaciones (id_tutor, id_alumno, nombre_tutor, nombre_alumno) VALUES (" 
              + to_string(id_tutor) + ", " + to_string(id_alumno) + ", '" 
              + nom_tutor + "', '" + nom_alumno + "');";
    }

    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    
    if (rc != SQLITE_OK) {
        cout << "ERROR : " << error << endl;
        sqlite3_free(error);
    } else {
        cout << " Asignación de tutor realizada correctamente." << endl;
        cout << "  Enviando correo al alumno " << nom_alumno << "..." << endl;
        cout << "  Enviando correo al tutor " << nom_tutor << "..." << endl;
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
        // Rellenamos los 4 huecos (?)
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





    



