#include <gtest/gtest.h>
#include "proyecto.h"
#include <sqlite3.h>

// Declaración de la función si no está en proyecto.h
bool RegistrarAlertaEnBD(sqlite3* db, const std::string& usuario, const std::string& categoria, const std::string& desc);

// Fixture para preparar el entorno de base de datos antes de cada test
class AlertaTest : public ::testing::Test {
protected:
    sqlite3* db;

    void SetUp() override {
        sqlite3_open(":memory:", &db);
        // Creamos la tabla mínima necesaria según tu proyecto.cpp
        string sql = "CREATE TABLE alertas (id INTEGER PRIMARY KEY, emisor_usuario TEXT, "
                     "categoria TEXT, descripcion TEXT, estado TEXT DEFAULT 'ABIERTA');";
        sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    }

    void TearDown() override {
        sqlite3_close(db);
    }
};

// TEST 1: Verificar que una alerta válida se guarda correctamente
TEST_F(AlertaTest, GuardarAlertaValida) {
    string usuario = "alumno_test";
    string categoria = "Salud";
    string desc = "Estudiante con fiebre alta";

    bool resultado = RegistrarAlertaEnBD(db, usuario, categoria, desc);

    // Verificación 1: La función devuelve éxito
    EXPECT_TRUE(resultado);

    // Verificación 2: El dato existe realmente en la BD
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT emisor_usuario, categoria FROM alertas", -1, &stmt, 0);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    
    EXPECT_STREQ((const char*)sqlite3_column_text(stmt, 0), "alumno_test");
    EXPECT_STREQ((const char*)sqlite3_column_text(stmt, 1), "Salud");
    
    sqlite3_finalize(stmt);
}

// TEST 2: Verificar la conversión de Enum a String (Lógica pura)
TEST(AlertaLogica, ConversionEnumToString) {
    EXPECT_EQ(CategoriaAlertaToString(CategoriaAlerta::Salud), "Salud");
    EXPECT_EQ(CategoriaAlertaToString(CategoriaAlerta::Academico), "Academico");
    // Prueba de robustez con un valor fuera de rango si fuera posible
}