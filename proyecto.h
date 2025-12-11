#ifndef PROYECTO_H
#define PROYECTO_H

#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h> // Necesario para la base de datos

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

#endif // PROYECTO_H