#include <iostream>
#include <string>
#include "../proyecto.h" 

using namespace std;


int testsPasados = 0;
int testsFallados = 0;

void ASSERT(bool condicion, string nombrePrueba) {
    if (condicion) {
        cout << " [PASS] " << nombrePrueba << endl;
        testsPasados++;
    } else {
        cout << "[FAIL] " << nombrePrueba << " (Resultado inesperado)" << endl;
        testsFallados++;
    }
}

// PRUEBAS UNITARIAS: CU-07 (ENCUESTAS)


// PRUEBA 1: Validar Rango de Puntuación (Debe ser 1-5)
void testRangoPuntuacion() {
    cout << "\n--- Ejecutando Test: Rango de Puntuación ---" << endl;
    
    // Casos Válidos (Límites)
    ASSERT(esPuntuacionValida(1) == true, "Acepta valor mínimo (1)");
    ASSERT(esPuntuacionValida(5) == true, "Acepta valor máximo (5)");
    ASSERT(esPuntuacionValida(3) == true, "Acepta valor medio (3)");

    // Casos Inválidos (Fuera de rango)
    ASSERT(esPuntuacionValida(0) == false, "Rechaza valor 0 (Inferior)");
    ASSERT(esPuntuacionValida(6) == false, "Rechaza valor 6 (Superior)");
    ASSERT(esPuntuacionValida(-5) == false, "Rechaza negativos");
}

// PRUEBA 2: Validar Comentario 
void testValidacionComentario() {
    cout << "--- Ejecutando Test: Validación de Comentario ---" << endl;
    
    string comentarioNormal = "La clase fue muy productiva.";
    string comentarioVacio = "";
    
    string comentarioGigante(250, 'X'); 

    
    ASSERT(esTextoValido(comentarioNormal) == true, "Acepta comentario estándar");
    
    //Prueba para ver si rechaza comentarios vacíos 
    ASSERT(esTextoValido(comentarioVacio) == false, "Rechaza comentario vacío");

    // Prueba que rechaza comentarios demasiados largos (>200 caracteres)
    ASSERT(esTextoValido(comentarioGigante) == false, "Rechaza comentario > 200 caracteres");
}


int main() {

    cout << "   TEST  CU-07 (ENCUESTAS)" << endl;
    
    testRangoPuntuacion();
    testValidacionComentario();

    cout << "RESUMEN: " << testsPasados << " Pasados | " << testsFallados << " Fallados" << endl;
    
    if (testsFallados == 0) {
        cout << "RESULTADO: ÉXITO " << endl;
        return 0;
    } else {
        cout << "RESULTADO: HAY ERRORES " << endl;
        return 1;
    }
}