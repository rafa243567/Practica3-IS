#include <gtest/gtest.h>
#include "proyecto.h"

// TEST 1: El que ya tenías (Prueba Positiva)
TEST(AlertasTest, CategoriaToString) {
    EXPECT_EQ(CategoriaAlertaToString(CategoriaAlerta::Salud), "Salud");
    EXPECT_EQ(CategoriaAlertaToString(CategoriaAlerta::Academico), "Academico");
}

// TEST 2: El nuevo test (Prueba Negativa)
// Verifica que el sistema maneja correctamente categorías inexistentes
TEST(AlertasTest, CategoriaNoValida) {
    // Forzamos un valor que no está en el enum (ej. 99)
    CategoriaAlerta cat_erronea = static_cast<CategoriaAlerta>(99);
    
    // Según tu lógica en proyecto.cpp, el switch debería ir al default
    EXPECT_EQ(CategoriaAlertaToString(cat_erronea), "Desconocido");
}