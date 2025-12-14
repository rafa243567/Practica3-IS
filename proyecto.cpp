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
        cout << "\n=== PANEL DE ESTUDIANTE: " << usuario << " ===\n"; // El 'usuario' ya lo tienes en la clase
        cout << "1. Abrir Chat con Tutor\n";
        cout << "2. Generar Alerta de Incidencia\n";
        cout << "3. Realizar Encuesta de Satisfaccion\n"; // <--- TU NUEVA OPCIÓN
        cout << "4. Cerrar Sesión\n";
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1) {
            chatPersonal.cargarHistorial();
            cout << "\nPresiona Enter para volver.";
            limpiarBuffer(); cin.get();
        } 
        else if (opcion == 2) {
            generarAlerta();
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 3) {
            // === AQUÍ LLAMAMOS A TU FUNCIÓN ===
            // 'db' es la variable global que tienes arriba en proyecto.cpp
            // 'usuario' es el atributo de la clase (ej. "juan.perez")
            Encuesta(db, usuario); 
            
            // Pausa para que el alumno lea el mensaje de "Encuesta enviada"
            cout << "\nPresiona Enter para volver al menú.";
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 4);
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
        cout << "1. Gestionar Asignaciones (Asignar Tutor)\n"; // <--- TU FUNCION
        cout << "2. Ver Lista de Asignaciones\n";            // <--- TU OTRA FUNCION
        cout << "3. Cerrar Sesión\n";
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1) {
            // === AQUÍ LLAMAMOS A LA ASIGNACIÓN ===
            // Como tu función tiene cin/cout dentro, tomará el control de la consola
            RealizarAsignacion(db); 
            
            cout << "\nPresiona Enter para continuar.";
            limpiarBuffer(); cin.get();
        }
        else if (opcion == 2) {
            // === AQUÍ LLAMAMOS A VER LISTA ===
            VerAsignaciones(db);
            
            cout << "\nPresiona Enter para continuar.";
            limpiarBuffer(); cin.get();
        }

    } while(opcion != 3);
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









// función para introducir los datos y la asignación de tutor y alumno 
// CONSULTAS DE ASIGNACION CU-04)


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


// FUNCION PARA QUE EL ALUMNO PUEDA VER SU TUTOR ASIGNADO 
void MostrarTutorAsignado(sqlite3 *db, string alumno_usuario){
    sqlite3_stmt* stmt;
    int id_alumno = -1;
    string nombre_tutor = ""; // Inicializar estas variables para poder comprobar fallos mas rapido 
    bool tiene_tutor = false;

    cout << " --- TUTOR ASIGNADO --- " << endl;

    //Obtener el ID del alumno a traves del nombre registrado
    string sql_id = "SELECT id FROM usuarios WHERE usuario = ?;";
    
    if (sqlite3_prepare_v2(db, sql_id.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, alumno_usuario.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_alumno = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);  // Limpiar el buffer para que no se queden datos mezclados 
    if (id_alumno == -1) {
        cout << " ERROR, El alumno no ha sido encontrado." << endl;
        return;
    }

    //  Buscar en la tabla el tutor asignado a ese nombre 
    string sql_asig = "SELECT nombre_tutor FROM asignaciones WHERE id_alumno = ?;";
    
    if (sqlite3_prepare_v2(db, sql_asig.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_alumno);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* texto = sqlite3_column_text(stmt, 0);
            nombre_tutor = string(reinterpret_cast<const char*>(texto));
            tiene_tutor = true;
        }
    }
    sqlite3_finalize(stmt);

    if (tiene_tutor) {
        cout << " Tu tutor asignado es: " << nombre_tutor << endl;
    } else {
        cout << " Aun no tienes un tutor asignado, pronto se te asignará uno ." << endl;
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





    



