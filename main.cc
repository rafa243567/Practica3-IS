#include <iostream>
#include <limits> 
#include "proyecto.h" 

using namespace std;

int main() {
    // 1. Conectar a la Base de Datos
    inicializarDatosPrueba();
    
    bool ejecutando = true;
    int opcion;

    while (ejecutando) {
        // Limpiamos pantalla (compatible Windows/Linux)
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        cout << "\n--- APP DE TUTORÍAS EDU-CONNECT ---\n";
        cout << "1. Iniciar Sesión\n";
        cout << "2. Registrarse\n";
        cout << "3. Salir\n";
        cout << "Selecciona: ";
        
        if (!(cin >> opcion)) {
            cout << "Entrada inválida.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (opcion) {
            case 1: {
                // Obtenemos un usuario genérico
                Usuario* usuarioActual = iniciarSesion();

                if (usuarioActual != nullptr) {
                    // EL PROGRAMA DECIDE AUTOMÁTICAMENTE QUÉ MENÚ MOSTRAR
                    // Si es Alumno -> muestra menú alumno
                    // Si es Tutor -> muestra menú tutor
                    usuarioActual->mostrarMenu();
                    
                    // Importante: Liberar la memoria al terminar la sesión
                    delete usuarioActual; 
                }
                break;
            }
            case 2:
                registrarse();
                // Pausa para leer mensaje de registro
                cout << "Presiona Enter para continuar...";
                cin.ignore(); cin.get(); 
                break;
            case 3:
                cout << "\n👋 ¡Hasta pronto!\n";
                ejecutando = false;
                break;
            default:
                cout << "Opción no válida.\n";
                break;
        }
    }

    return 0;
}