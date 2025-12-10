#include <iostream>
#include <string>
#include <sqlite3.h> 

using namespace std;

void inicializarBaseDeDatos(sqlite3 *db) {
    char *error = 0;
    // Creamos la tabla con todas las columnas necesarias
    string sql = "CREATE TABLE IF NOT EXISTS asignaciones ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "id_tutor INTEGER, "
                 "id_alumno INTEGER, "
                 "nombre_tutor TEXT, "
                 "nombre_alumno TEXT, "
                 "fecha TEXT DEFAULT CURRENT_TIMESTAMP);";
    sqlite3_exec(db, sql.c_str(), 0, 0, &error);
}

void RealizarAsignacion (sqlite3 *db){
    int id_tutor, id_alumno; 
    string nom_tutor, nom_alumno;
    char *error = 0;

    cout << "\n------- REALIZAR ASIGNACION TUTOR - ALUMNO -------" << endl;
    
    cout << " Introduce el ID del tutor: ";
    cin >> id_tutor;
    cin.ignore(); 
    cout << " Introduce el nombre del tutor: ";
    getline(cin, nom_tutor);
    
    cout << " Introduce el ID del alumno: ";
    cin >> id_alumno;
    cin.ignore(); 
    cout << " Introduce el nombre del alumno: ";
    getline(cin, nom_alumno);

    string sql = "INSERT INTO asignaciones (id_tutor, id_alumno, nombre_tutor, nombre_alumno) VALUES (" 
                 + to_string(id_tutor) + ", " 
                 + to_string(id_alumno) + ", '" 
                 + nom_tutor + "', '" 
                 + nom_alumno + "');";

    int resultado  = sqlite3_exec(db, sql.c_str(), 0, 0, &error);
    
    if (resultado != SQLITE_OK) {
        cerr << "Error al realizar la asignacion: " << error << endl;
        sqlite3_free(error);
    } else {
        cout << "Asignacion realizada exitosamente." << endl;
    }
}

// Función para mostrar cada fila encontrada
int MostrarDatos(void *NotUsed, int argc, char **argv, char **azColName){
    // El orden depende del SELECT de abajo:
    // [0]id_tutor, [1]id_alumno, [2]nombre_tutor, [3]nombre_alumno, [4]fecha
    
    string id_t = argv[0] ? argv[0] : "?";
    string id_a = argv[1] ? argv[1] : "?";
    string nom_t = argv[2] ? argv[2] : "Sin Nombre";
    string nom_a = argv[3] ? argv[3] : "Sin Nombre";
    string fecha = argv[4] ? argv[4] : "?";

    cout << "Fecha: " << fecha << " | " 
         << "Tutor: " << nom_t << " (ID " << id_t << ") "
         << "--> Alumno: " << nom_a << " (ID " << id_a << ")" << endl;
    return 0;
}

void VerAsignaciones(sqlite3 *db){
    cout << "\n------- ASIGNACIONES REALIZADAS -------" << endl;
    
    // Pedimos las columnas explícitamente para evitar errores de orden
    string sql = "SELECT id_tutor, id_alumno, nombre_tutor, nombre_alumno, fecha FROM asignaciones;";
    
    char *error = 0;
    int resultado = sqlite3_exec(db, sql.c_str(), MostrarDatos, 0, &error);
    
    if (resultado != SQLITE_OK) {
        cerr << "Error al leer datos: " << error << endl;
    }
    cout << "---------------------------------------" << endl;
}
  
int main() {
    sqlite3 *db; 
    
    if (sqlite3_open("tutores_alumnos.db", &db)) {
        cerr << "No se pudo abrir la base de datos: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    inicializarBaseDeDatos(db);

    int opcion = 0;

    do{
        cout << "\n------- Menu de Asignacion -------" << endl;
        cout << "1. Realizar una asignacion" << endl;
        cout << "2. Ver asignaciones" << endl;
        cout << "3. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch(opcion){
            case 1: RealizarAsignacion(db); break;
            case 2: VerAsignaciones(db); break;
            case 3: cout << "Saliendo del programa..." << endl; break;
            default: cout << "Opcion no valida. Intente de nuevo." << endl;
        }
    } while(opcion != 3);
    
    sqlite3_close(db);
    return 0;
}