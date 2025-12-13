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



// --- FUNCIONES GLOBALES ---
void inicializarDatosPrueba();
void registrarse();
Usuario* iniciarSesion(); 

// Funciones de Base de Datos (Solo una vez cada una)
void iniciarBaseDeDatos(sqlite3 *db);

// Gestión de Asignaciones (Coordinador)
void RealizarAsignacion(sqlite3 *db); 
void VerAsignaciones(sqlite3 *db); 

// Funciones de Alumno
void Encuesta(sqlite3 *db, string alumno_usuario);
void MostrarTutorAsignado(sqlite3 *db, string alumno_usuario); // <-- ESTA TE FALTA EN EL MENU ALUMNO

// Funciones de Tutor
void RegistrarActa(sqlite3 *db, string tutor_usuario);
void MostrarAlumnosAsignados(sqlite3 *db, string tutor_usuario);

// Resultados (Coordinador)
void VerResultadosEncuestas(sqlite3 *db);

#endif