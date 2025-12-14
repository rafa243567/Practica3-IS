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
        cout << " [FAIL] " << nombrePrueba << " (Resultado inesperado)" << endl;
        testsFallados++;
    }
}



// PRUEBA 1: Verificar Resumen Obligatorio (Escenario 6a)
void testResumenObligatorio() {
    cout << "\n--- [CU-03] Test: Validación de Campo Obligatorio (Resumen) ---" << endl;
    
    string resumenVacio = "";
    string resumenValido = "Reunión de seguimiento sobre el Sprint 2.";

    ASSERT(esTextoValido(resumenVacio) == false, "Impide guardar un acta con resumen vacío");

    ASSERT(esTextoValido(resumenValido) == true, "Permite guardar un acta con contenido válido");
}

// PRUEBA 2: Verificar Integridad (Límite de caracteres)
void testIntegridadResumen() {
    cout << "\n--- [CU-03] Test: Validación de Integridad de Datos ---" << endl;
    
    string resumenGigante(250, 'A'); 

    ASSERT(esTextoValido(resumenGigante) == false, "Rechaza resúmenes excesivamente largos (>200)");
}


int main() {

    cout << "   TEST SUITE: REGISTRAR ACTA (CU-03)" << endl;

    testResumenObligatorio();
    testIntegridadResumen();

    cout << "RESUMEN: " << testsPasados << " Pasados | " << testsFallados << " Fallados" << endl;
    
    if (testsFallados == 0) {
        cout << "RESULTADO FINAL: ÉXITO " << endl;
        return 0;
    } else {
        cout << "RESULTADO FINAL: ERRORES DETECTADOS " << endl;
        return 1;
    }
}