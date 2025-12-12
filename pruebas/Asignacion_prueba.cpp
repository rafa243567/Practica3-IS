#include <iostream>
#include <sqlite3.h>
#include <string>

using namespace std;


void iniciarBaseDeDatos(sqlite3 *db) {
    char *error = 0;
    string sql = "CREATE TABLE IF NOT EXISTS asignaciones ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "id_tutor INTEGER, "
                 "nombre_tutor TEXT, "   
                 "id_alumno INTEGER, "
                 "nombre_alumno TEXT, " 
                 "fecha TEXT DEFAULT CURRENT_TIMESTAMP);";
    sqlite3_exec(db, sql.c_str(), 0, 0, &error);
}

bool verificarUsuario(sqlite3* db, int id, string rolEsperado) {
    sqlite3_stmt* stmt;

    // simulacion de una conlsulta
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

//  función de asignación previa
bool tieneAsignacionPrevia(sqlite3* db, int id_alumno) {
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

//Prueba de datos para ver si funciona correctamente
void prepararDatosDePrueba(sqlite3* db) {
    char* error = 0;

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS usuarios (id INTEGER PRIMARY KEY, usuario TEXT, contrasena TEXT, rol TEXT);", 0, 0, 0);

    sqlite3_exec(db, "DELETE FROM usuarios;", 0, 0, 0);
    sqlite3_exec(db, "DELETE FROM asignaciones;", 0, 0, 0);

    string sql = "INSERT INTO usuarios (id, usuario, contrasena, rol) VALUES "
                 "(1, 'alumnoTest', '123', 'alumno'),"
                 "(2, 'tutorTest', '123', 'tutor');";
    sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    
    // Asignación previa para probar la función 'tieneAsignacionPrevia'
    string sql_asig = "INSERT INTO asignaciones (id_tutor, id_alumno, nombre_tutor, nombre_alumno) "
                      "VALUES (2, 1, 'tutorTest', 'alumnoTest');";
    sqlite3_exec(db, sql_asig.c_str(), 0, 0, 0);
}



int main() {
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db); 
    
    if (rc) {
        cout << "Error abriendo DB de prueba." << endl;
        return 1;
    }
    

    iniciarBaseDeDatos(db);
    
    // Metemos los datos falsos
    prepararDatosDePrueba(db);

    cout << "----PROBANDO (CU-O4)----" << endl;

    // Prueba para verificar el usuario 
    cout << "TEST 1: Buscar alumno existente (ID 1)... ";
    if (verificarUsuario(db, 1, "alumno")) cout << " correcto" << endl;
    else cout << "FALLO" << endl;

    cout << "TEST 2: Buscar alumno FALSO (ID 999)... ";
    if (!verificarUsuario(db, 999, "alumno")) cout << " correcto" << endl;
    else cout << "FALLO" << endl;

    //Prueba para verificar la asignación previa
    cout << "TEST 3: Detectar si Alumno 1 ya tiene tutor... ";
    if (tieneAsignacionPrevia(db, 1)) cout << " correcto" << endl;
    else cout << "FALLO" << endl;

    cout << "TEST 4: Detectando si alumno 5 tiene tutor...  ";
    if (!tieneAsignacionPrevia(db, 5)) cout << " correcto " << endl;
    else cout << " FALLO" << endl;

    cout << "----FIN DE LAS PRUEAS-----" << endl;

    sqlite3_close(db);
    return 0;
}