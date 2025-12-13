#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include "proyecto.h" // Incluimos tu cabecera

using namespace std;

// --- CLASE DE SOPORTE PARA REDIRECCIONAR I/O ---
// Esta clase se encarga de "secuestrar" el cin y cout y restaurarlos al final
class MenuTest : public ::testing::Test {
protected:

    stringstream input_fake;
    stringstream output_fake;
    streambuf* cin_backup;
    streambuf* cout_backup;

    void SetUp() override {
        // Guardamos los buffers originales (Teclado y Pantalla real)
        cin_backup = cin.rdbuf();
        cout_backup = cout.rdbuf();

        // Redirigimos a nuestros buffers falsos
        cin.rdbuf(input_fake.rdbuf());
        cout.rdbuf(output_fake.rdbuf());
        
        // Inicializamos la BD para que no falle si alguna función interna la llama
        // (Aunque para probar solo el menú "Salir" no sería estrictamente necesario, es buena práctica)
        inicializarDatosPrueba();
    }

    void TearDown() override {
        // Restauramos el orden natural de las cosas (Muy importante)
        cin.rdbuf(cin_backup);
        cout.rdbuf(cout_backup);
    }
};

// ==========================================
// TEST 1: Menú de Alumno
// ==========================================
TEST_F(MenuTest, MenuAlumnoMuestraOpcionesYSale) {
    // 1. SIMULAMOS EL INPUT DEL USUARIO
    // Escribimos "5" en el buffer de entrada falso + salto de linea.
    // Esto simula que el usuario pulsa '5' y luego Enter para salir.
    input_fake << "5\n";

    // 2. CREAMOS EL OBJETO
    Alumno alumno("test_alumno", "1234", "Ingenieria", true);

    // 3. EJECUTAMOS EL MÉTODO
    // El método leerá el '5' del input_fake y saldrá del bucle do-while inmediatamente.
    alumno.mostrarMenu();

    // 4. CAPTURAMOS Y VERIFICAMOS LA SALIDA
    string salida = output_fake.str();

    // Verificamos que el menú se ha impreso
    // (npos significa "no encontrado", así que esperamos que NO sea npos)
    EXPECT_NE(salida.find("PANEL DE ESTUDIANTE: test_alumno"), string::npos) 
        << "El título del menú del alumno no aparece.";
    
    EXPECT_NE(salida.find("1. Ver mi Tutor Asignado"), string::npos)
        << "La opción 1 no aparece en el menú.";
        
    EXPECT_NE(salida.find("5. Cerrar Sesión"), string::npos)
        << "La opción de salir no aparece.";
}

// ==========================================
// TEST 2: Menú de Tutor
// ==========================================
TEST_F(MenuTest, MenuTutorMuestraOpcionesYSale) {
    // El tutor sale con la opción 3
    input_fake << "3\n";

    Tutor tutor("test_tutor", "1234", "Informatica");
    tutor.mostrarMenu();

    string salida = output_fake.str();

    // Comprobamos que sale el texto esperado
    // Nota: en tu código actual quitaste lo del departamento en el cout, 
    // así que buscamos "PANEL DE TUTOR"
    EXPECT_NE(salida.find("PANEL DE TUTOR: test_tutor"), string::npos);
    EXPECT_NE(salida.find("1. Ver Alumnos Asignados"), string::npos);
}

// ==========================================
// TEST 3: Menú de Coordinador
// ==========================================
TEST_F(MenuTest, MenuCoordinadorMuestraOpcionesYSale) {
    // El coordinador sale con la opción 4
    input_fake << "4\n";

    Coordinador coord("test_admin", "1234");
    coord.mostrarMenu();

    string salida = output_fake.str();

    EXPECT_NE(salida.find("PANEL DE COORDINADOR"), string::npos);
    EXPECT_NE(salida.find("3. Ver Resultados de Encuestas"), string::npos);
}

// ==========================================
// TEST 4: Entrada Inválida (Robustez)
// ==========================================
TEST_F(MenuTest, MenuManejaEntradaInvalida) {
    // Simulamos que el usuario es torpe:
    // 1. Escribe 'a' (letra) -> El menú debe detectar error y limpiar
    // 2. Escribe '99' (opción no válida) -> El menú repite
    // 3. Escribe '5' -> Sale correctamente
    input_fake << "a\n99\n5\n";

    Alumno alumno("test_torpe", "1234", "Ingenieria", true);
    alumno.mostrarMenu();

    string salida = output_fake.str();

    // Verificamos que tu código detectó la entrada inválida
    // (Tu código tiene un cout << "Entrada inválida.\n")
    EXPECT_NE(salida.find("Entrada inválida"), string::npos) 
        << "El menú no detectó la entrada de texto (letra 'a')";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}