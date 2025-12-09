#include <iostream>
#include <limits> 
#include "proyecto.h" // Importa todas las funciones y la estructura Usuario

using namespace std;

// Función auxiliar para limpiar el buffer de entrada en caso de error debido a la introducción de datos erroneos
void ErrorEntrada() {
    cout << "❌ Entrada inválida. Por favor, ingresa un número.\n";
    cin.clear();
    // Ignora el resto de la línea en el buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
}

int main() {

    inicializarDatosPrueba();
    
    bool ejecutando = true;
    int opcion;

    while (ejecutando) {
        // Muestra el menú definido en proyecto.cpp
        mostrarMenu();

        // Captura la opción y maneja la entrada no numérica
        if (!(cin >> opcion)) {
            ErrorEntrada();
            continue; // Vuelve al inicio del bucle
        }

        switch (opcion) {
            case 1:
                iniciarSesion(); // Llama a la función de Login
                break;
            case 2:
                registrarse();  // Llama a la función de Registro
                break;
            case 3:
                cout << "\n👋 Saliendo de la aplicación. ¡Hasta pronto!\n";
                ejecutando = false; // Detiene la ejecución
                break;
            default:
                cout << "⚠️ Opción no válida. Por favor, selecciona 1, 2 o 3.\n";
                break;
        }
    }

    return 0;
}
