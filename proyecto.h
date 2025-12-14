#ifndef PROYECTO_H
#define PROYECTO_H

#include <iostream>
#include <string>
#include <sqlite3.h> 

using namespace std;

// --- CLASES BASE Y DERIVADAS ---

class Usuario {
protected:
    string usuario;
    string contrasena;
    string rol;
public:
    Usuario(string u, string p, string r) : usuario(u), contrasena(p), rol(r) {}
    virtual ~Usuario() {}
    virtual void mostrarMenu() = 0; 
    string getUsuario() const { return usuario; }
    string getRol() const { return rol; }
};

class FichaAcademica {
public:
    string datosAcademicos;
    string asignaturas;
    void mostrarFicha() {
        cout << "\n   [FICHA] Datos: " << datosAcademicos << " | Asignaturas: " << asignaturas << endl;
    }
};

class Chat {
public:
    void cargarHistorial() {
        cout << "\n   [CHAT] --- Historial cargado ---\n";
    }
};

class Alumno : public Usuario {
private:
    string carrera;
    bool esPrimerCurso;
    Chat chatPersonal;
public:
    Alumno(string u, string p, string c, bool primerCurso) 
        : Usuario(u, p, "alumno"), carrera(c), esPrimerCurso(primerCurso) {}
    void mostrarMenu() override;
    void generarAlerta();        
};

class Tutor : public Usuario {
private:
    string departamento;
public:
    Tutor(string u, string p, string dep) 
        : Usuario(u, p, "tutor"), departamento(dep) {}
    void mostrarMenu() override; 
    void verFichaAlumno();       
    void registrarActa();
};

class Coordinador : public Usuario { 
public:
    Coordinador(string u, string p) : Usuario(u, p, "admin") {}
    void mostrarMenu() override;
    void gestionarAsignaciones();
};

// --- TUS FUNCIONES GLOBALES ---
void inicializarDatosPrueba();
void registrarse();
Usuario* iniciarSesion(); 


//FUNCIONES PARA REGISTRAR CADA ACTA (CU-03)
void RegistrarActa(sqlite3 *db, string tutor_usuario);



// Funciones para la Asignación de Tutores (CU-04) 
void iniciarBaseDeDatos(sqlite3 *db); // Abre la base de datos para guardar los datos que se inscriban en ella
void RealizarAsignacion(sqlite3 *db); // Funcion que se va a usar para realizar la asgnación de tutores a los alumnos 
void VerAsignaciones(sqlite3 *db); // Funcion que se va a usar para ver las asignaciones realizadas

void MostrarTutorAsignado(sqlite3 *db, string alumno_usuario); // Funcion que muestra al alumno su tutor 
void MostrarAlumnosAsignados(sqlite3 *db, string tutor_usuario); //Funcion que muestra al tutor los alumnos que le han asignado 



//Funciones para la realización de la encuesta (CU-07)
void iniciarBaseDeDatos(sqlite3 *db); 
void RealizarAsignacion(sqlite3 *db); 
void VerAsignaciones(sqlite3 *db); 
void Encuesta(sqlite3 *db, string alumno_usuario);
void VerResultadosEncuestas(sqlite3 *db); // funcion para ver los resultados de las encuestas realizadas


//PRUEBAS CU-07 ( ENCUESTA)
bool esPuntuacionValida(int puntuacion);
bool esTextoValido(string texto);

#endif