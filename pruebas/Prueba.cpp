#include <iostream>
#include <sqlite3.h>
#include <string>
#include <limits>

using namespace std;

// ==========================================
// 1. UTILIDADES Y CONFIGURACIÓN
// ==========================================

void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void iniciarDB(sqlite3 *db) {
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS usuarios (id INTEGER PRIMARY KEY AUTOINCREMENT, usuario TEXT, rol TEXT);", 0,0,0);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS asignaciones (id INTEGER PRIMARY KEY, id_tutor INTEGER, nombre_tutor TEXT, id_alumno INTEGER, nombre_alumno TEXT, fecha TEXT DEFAULT CURRENT_TIMESTAMP);", 0,0,0);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS encuestas (id INTEGER PRIMARY KEY, id_tutor INTEGER, id_alumno INTEGER, puntuacion INTEGER, comentario TEXT, fecha TEXT DEFAULT CURRENT_TIMESTAMP);", 0,0,0);

    // DATOS INICIALES PARA QUE NO TENGAS QUE CREARLOS
    cout << "--- INICIALIZANDO DATOS DE PRUEBA ---" << endl;
    sqlite3_exec(db, "INSERT INTO usuarios (usuario, rol) VALUES ('Alex', 'alumno');",0,0,0);       // ID 1
    sqlite3_exec(db, "INSERT INTO usuarios (usuario, rol) VALUES ('ProfeMalo', 'tutor');",0,0,0);  // ID 2
    sqlite3_exec(db, "INSERT INTO usuarios (usuario, rol) VALUES ('ProfeBueno', 'tutor');",0,0,0); // ID 3
    
    // Asignación inicial: Alex con ProfeMalo
    sqlite3_exec(db, "INSERT INTO asignaciones (id_tutor, nombre_tutor, id_alumno, nombre_alumno) VALUES (2, 'ProfeMalo', 1, 'Alex');",0,0,0);
    cout << "✅ Usuarios creados: Alex (ID 1), ProfeMalo (ID 2), ProfeBueno (ID 3)." << endl;
    cout << "✅ Asignación inicial: Alex (1) -> ProfeMalo (2)." << endl;
}

// ==========================================
// 2. FUNCIONES INTERACTIVAS
// ==========================================

void VerEstadoActual(sqlite3 *db) {
    cout << "\n--- ESTADO DE ASIGNACIONES ---" << endl;
    sqlite3_stmt* stmt;
    string sql = "SELECT id_alumno, nombre_alumno, id_tutor, nombre_tutor FROM asignaciones;";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "Alumno: " << sqlite3_column_text(stmt, 1) << " (ID " << sqlite3_column_int(stmt, 0) << ")";
        cout << " --> Tutor: " << sqlite3_column_text(stmt, 3) << " (ID " << sqlite3_column_int(stmt, 2) << ")" << endl;
    }
    sqlite3_finalize(stmt);
}

void CambiarTutorManual(sqlite3 *db) {
    int id_alumno, id_nuevo_tutor;
    string nombre_tutor;

    cout << "\n>> CAMBIO DE TUTOR (Modo Coordinador)" << endl;
    VerEstadoActual(db); // Para que veas los IDs
    
    cout << "Introduce ID del Alumno a cambiar: ";
    cin >> id_alumno;
    cout << "Introduce ID del NUEVO Tutor: ";
    cin >> id_nuevo_tutor;
    cout << "Introduce Nombre del NUEVO Tutor: ";
    limpiarBuffer();
    getline(cin, nombre_tutor);

    string sql = "UPDATE asignaciones SET id_tutor = ?, nombre_tutor = ? WHERE id_alumno = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_nuevo_tutor);
        sqlite3_bind_text(stmt, 2, nombre_tutor.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, id_alumno);
        sqlite3_step(stmt);
        cout << "✅ ¡Cambio realizado!" << endl;
    }
    sqlite3_finalize(stmt);
}

void HacerEncuestaManual(sqlite3 *db) {
    int id_alumno, puntuacion;
    string comentario;

    cout << "\n>> REALIZAR ENCUESTA (Modo Alumno)" << endl;
    cout << "Soy el alumno con ID: ";
    cin >> id_alumno;

    // 1. Buscar Tutor
    int id_tutor = -1;
    sqlite3_stmt* stmt;
    string sql_buscar = "SELECT id_tutor, nombre_tutor FROM asignaciones WHERE id_alumno = ?;";
    if (sqlite3_prepare_v2(db, sql_buscar.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_alumno);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id_tutor = sqlite3_column_int(stmt, 0);
            cout << "Vas a evaluar al tutor: " << sqlite3_column_text(stmt, 1) << " (ID " << id_tutor << ")" << endl;
        }
    }
    sqlite3_finalize(stmt);

    if (id_tutor == -1) { cout << "❌ Error: Ese ID de alumno no tiene tutor." << endl; return; }

    // 2. Pedir Datos
    cout << "Puntuación (1-5): ";
    cin >> puntuacion;
    limpiarBuffer();
    cout << "Comentario: ";
    getline(cin, comentario);

    // 3. Guardar
    string sql_ins = "INSERT INTO encuestas (id_tutor, id_alumno, puntuacion, comentario) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql_ins.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id_tutor);
        sqlite3_bind_int(stmt, 2, id_alumno);
        sqlite3_bind_int(stmt, 3, puntuacion);
        sqlite3_bind_text(stmt, 4, comentario.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        cout << "✅ Encuesta guardada." << endl;
    }
    sqlite3_finalize(stmt);
}

void VerResultados(sqlite3 *db) {
    cout << "\n--- 📊 TABLA DE RESULTADOS EN BASE DE DATOS ---" << endl;
    cout << "ID_TUTOR | ID_ALUMNO | NOTA | COMENTARIO" << endl;
    cout << "--------------------------------------------" << endl;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT * FROM encuestas;", -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "    " << sqlite3_column_int(stmt, 1) 
             << "    |     " << sqlite3_column_int(stmt, 2) 
             << "     |  " << sqlite3_column_int(stmt, 3) 
             << "   | " << sqlite3_column_text(stmt, 4) << endl;
    }
    sqlite3_finalize(stmt);
    cout << "--------------------------------------------" << endl;
}

// ==========================================
// 3. MENÚ PRINCIPAL
// ==========================================

int main() {
    sqlite3* db;
    // Usamos una DB en memoria (:memory:) o archivo temporal para que sea rápido y limpio
    sqlite3_open("test_manual.db", &db);
    iniciarDB(db);

    int opcion;
    do {
        cout << "\n=== LABORATORIO DE PRUEBAS ===" << endl;
        cout << "1. Ver quién es mi tutor (Estado Actual)" << endl;
        cout << "2. CAMBIAR TUTOR (Simular Coordinador)" << endl;
        cout << "3. HACER ENCUESTA (Simular Alumno)" << endl;
        cout << "4. VER RESULTADOS (Comprobar si se guardó bien)" << endl;
        cout << "5. Salir" << endl;
        cout << "Elige: ";
        cin >> opcion;

        switch(opcion) {
            case 1: VerEstadoActual(db); break;
            case 2: CambiarTutorManual(db); break;
            case 3: HacerEncuestaManual(db); break;
            case 4: VerResultados(db); break;
        }
    } while(opcion != 5);

    sqlite3_close(db);
    remove("test_manual.db");
    return 0;
}