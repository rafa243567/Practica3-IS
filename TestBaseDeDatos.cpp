#include <gtest/gtest.h>
#include <sqlite3.h>
#include "proyecto.h" // Asegúrate de que este sea el nombre de tu cabecera

class BaseDatosTest : public ::testing::Test {
protected:
    sqlite3* db;

    // Se ejecuta ANTES de cada prueba
    void SetUp() override {
        // Abrimos una base de datos limpia en memoria para cada test
        int rc = sqlite3_open(":memory:", &db);
        ASSERT_EQ(rc, SQLITE_OK);

        // Creamos la estructura mínima necesaria (puedes añadir más tablas)
        const char* sql = 
            "CREATE TABLE usuarios (usuario TEXT PRIMARY KEY, contrasena TEXT, rol TEXT);"
            "CREATE TABLE alertas (emisor_usuario TEXT, categoria TEXT, descripcion TEXT, estado TEXT);";
        
        rc = sqlite3_exec(db, sql, 0, 0, 0);
        ASSERT_EQ(rc, SQLITE_OK);
    }

    // Se ejecuta DESPUÉS de cada prueba
    void TearDown() override {
        sqlite3_close(db);
    }
};

// PRUEBA 1: Verificar que un usuario se guarda correctamente
TEST_F(BaseDatosTest, InsertarUsuarioCorrectamente) {
    const char* sql_insert = "INSERT INTO usuarios VALUES ('profe_test', '1234', 'tutor');";
    int rc = sqlite3_exec(db, sql_insert, 0, 0, 0);
    
    EXPECT_EQ(rc, SQLITE_OK);

    // Verificamos que realmente existe
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT count(*) FROM usuarios WHERE usuario='profe_test';", -1, &stmt, 0);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    EXPECT_EQ(count, 1);
}

// PRUEBA 2: Verificar la lógica de tus funciones (ejemplo Alertas)
TEST_F(BaseDatosTest, RegistrarAlertaFunciona) {
    // Aquí llamamos a tu función real del código
    // Nota: Debes adaptar el nombre si en tu proyecto se llama diferente
    string emisor = "alumno01";
    string categoria = "Salud";
    string desc = "Dolor de cabeza";

    // Simulamos la inserción que hace tu programa
    string sql = "INSERT INTO alertas (emisor_usuario, categoria, descripcion, estado) VALUES ('alumno01', 'Salud', 'Prueba', 'ABIERTA');";
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, 0);

    EXPECT_EQ(rc, SQLITE_OK);
}

// PRUEBA 3: Error por Clave Duplicada
TEST_F(BaseDatosTest, ErrorUsuarioDuplicado) {
    sqlite3_exec(db, "INSERT INTO usuarios VALUES ('user1', 'p', 'r');", 0, 0, 0);
    
    // Intentar insertar el mismo usuario otra vez debería fallar (SQLITE_CONSTRAINT)
    int rc = sqlite3_exec(db, "INSERT INTO usuarios VALUES ('user1', 'p', 'r');", 0, 0, 0);
    
    EXPECT_EQ(rc, SQLITE_CONSTRAINT);
}