#include <gtest/gtest.h>
#include "proyecto.h"
#include <sqlite3.h>
#include <string>

using namespace std;

// Fixture para gestionar la base de datos de prueba
class ChatTest : public ::testing::Test {
protected:
    sqlite3* db;

    void SetUp() override {
        // Base de datos en memoria para no ensuciar el archivo real
        sqlite3_open(":memory:", &db);
        
        // Creamos la tabla necesaria siguiendo el esquema de proyecto.cpp
        string sql = "CREATE TABLE mensajes ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                     "remitente TEXT, "
                     "receptor TEXT, "
                     "contenido TEXT, "
                     "fecha TEXT DEFAULT CURRENT_TIMESTAMP);";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }

    void TearDown() override {
        sqlite3_close(db);
    }
};

// Caso de prueba: Registro exitoso de mensaje
TEST_F(ChatTest, RegistroMensajeExitoso) {
    string remitente = "miguel_alumno";
    string receptor = "profesor_tutor";
    string mensaje = "Hola, ¿podemos revisar el diagrama de clases?";

    // Ejecutamos la función lógica de nuestro proyecto
    EnviarMensajeChat(db, remitente, receptor, mensaje);

    // Verificación mediante consulta SQL
    sqlite3_stmt* stmt;
    string sql_check = "SELECT remitente, contenido FROM mensajes WHERE receptor = ?;";
    
    ASSERT_EQ(sqlite3_prepare_v2(db, sql_check.c_str(), -1, &stmt, nullptr), SQLITE_OK);
    sqlite3_bind_text(stmt, 1, receptor.c_str(), -1, SQLITE_STATIC);

    // Comprobamos que hay un resultado y que los datos coinciden
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), remitente.c_str());
    EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), mensaje.c_str());

    sqlite3_finalize(stmt);
}