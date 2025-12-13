#ifndef PROYECTO_H
#define PROYECTO_H

#include <iostream>
#include <string>
#include <sqlite3.h> // Necesario para la integración en la Base de datos 

using namespace std;

// --- 1. CLASES DE APOYO (Según tu Diagrama de Clases) ---
// Estas clases representan objetos que usan los usuarios.

class FichaAcademica {
public:
    string datosAcademicos;
    string asignaturas;
    
    // Método para mostrar la ficha
    void mostrarFicha() {
        cout << "\n   [FICHA] Datos: " << datosAcademicos << " | Asignaturas: " << asignaturas << endl;
    }
};

class Chat {
public:
    int idChat;
    
    // Simulación de cargar historial
    void cargarHistorial() {
        cout << "\n   [CHAT] --- Historial de mensajes cargado ---\n";
        cout << "   Tutor: Hola, ¿cómo vas con la práctica?\n";
        cout << "   Alumno: Bien, avanzando con el código.\n";
    }
};

class Usuario {
protected:
    // Atributos protegidos: solo accesibles por Usuario y sus hijos (Alumno, Tutor...)
    string usuario;
    string contrasena;
    string rol;

public:
    // Constructor
    Usuario(string u, string p, string r) 
        : usuario(u), contrasena(p), rol(r) {}
    
    // Destructor virtual (Importante para limpiar memoria correctamente)
    virtual ~Usuario() {}

    // POLIMORFISMO: Este método es "mágico". Cada hijo lo implementará a su manera.
    virtual void mostrarMenu() = 0; 

    // Getters
    string getUsuario() const { return usuario; }
    string getRol() const { return rol; }
};

// --- 3. CLASES HIJAS (HERENCIA) ---

class Alumno : public Usuario {
private:
    string carrera;
    bool esPrimerCurso;
    Chat chatPersonal; // Relación de Composición: Un alumno TIENE un chat

public:
    // Constructor que llama al constructor del padre
    Alumno(string u, string p, string c, bool primerCurso) 
        : Usuario(u, p, "alumno"), carrera(c), esPrimerCurso(primerCurso) {}

    void mostrarMenu() override; // Implementación propia del menú
    void generarAlerta();        // Funcionalidad específica
};

class Tutor : public Usuario {
private:
    string departamento;
    
public:
    Tutor(string u, string p, string dep) 
        : Usuario(u, p, "tutor"), departamento(dep) {}

    void mostrarMenu() override; 
    void verFichaAlumno();       // Funcionalidad específica del tutor
    void registrarActa();
};

class Coordinador : public Usuario { // Equivalente a tu Admin
public:
    Coordinador(string u, string p) 
        : Usuario(u, p, "admin") {}

    void mostrarMenu() override;
    void gestionarAsignaciones();
};

// --- FUNCIONES GLOBALES ---
void inicializarDatosPrueba();
void registrarse();
Usuario* iniciarSesion(); // Ahora devuelve un puntero a Usuario



// Funciones para la Asignación de Tutores (CU-04) 
void iniciarBaseDeDatos(sqlite3 *db); // Abre la base de datos para guardar los datos que se inscriban en ella
void RealizarAsignacion(sqlite3 *db); // Funcion que se va a usar para realizar la asgnación de tutores a los alumnos 
void VerAsignaciones(sqlite3 *db); // Funcion que se va a usar para ver las asignaciones realizadas

void MostrarTutorAsignado(sqlite3 *db, string alumno_usuario); // Funcion que muestra al alumno su tutor 
void MostrarAlumnosAsignados(sqlite3 *db, string tutor_usuario); //Funcion que muestra al tutor los alumnos que le han asignado 



//Funciones para la realización de la encuesta (CU-05)
void Encuesta(sqlite3 *db, string alumno_usuario);
void VerResultadosEncuestas(sqlite3 *db); // funcion para ver los resultados de las encuestas realizadas


#endif // PROYECTO_H