#include <gtest/gtest.h>
#include "proyecto.h" // Asegúrese de que este archivo sea su cabecera C++
#include <sqlite3.h>
#include <string>

// Usamos el espacio de nombres estándar
using namespace std;

// Prueba de la lógica de conversión (Lógica pura de C++)
TEST(AlertasTest, VerificarsalidaTexto) {
    // Definimos variables usando std::string de C++
    string resultado_esperado = "Salud";
    
    // Invocamos la función y comparamos con operadores de C++
    EXPECT_EQ(CategoriaAlertaToString(CategoriaAlerta::Salud), resultado_esperado);
}

// Prueba de robustez (Test Negativo)
TEST(AlertasTest, ManejoDeErrorCategoria) {
    // Casteo de estilo C++ (static_cast)
    CategoriaAlerta cat_falsa = static_cast<CategoriaAlerta>(999);
    
    EXPECT_EQ(CategoriaAlertaToString(cat_falsa), "Desconocido");
}