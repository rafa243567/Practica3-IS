cmake_minimum_required(VERSION 3.10)
project(ProyectoSoftwareTests)

set(CMAKE_CXX_STANDARD 11)

# Buscar paquetes necesarios
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Crear el ejecutable de pruebas
add_executable(ejecutar_tests test_alertas.cpp proyecto.cpp)

# Enlazar librerías (GTest, SQLite3 y hilos)
target_link_libraries(ejecutar_tests ${GTEST_LIBRARIES} sqlite3 pthread)