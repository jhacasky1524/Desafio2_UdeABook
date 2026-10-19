#include <iostream>
#include <string>
#include "GestorArchivos.h"
#include "Medidor.h"
#include "UdeABook.h"

using namespace std;

int main()
{
    UdeABook red;
    GestorArchivos gestor("datos");

    // I. Carga de datos (no aparece en el menu de los usuarios)
    Medidor::reiniciar();
    if (!gestor.cargar(red)) {
        cerr << "No se pudieron cargar los datos. Ejecute primero el generador.\n";
        return 1;
    }
    unsigned long iteraciones = Medidor::getIteraciones();   // antes de calcular la memoria
    unsigned long memoria = red.memoria();
    cout << "Datos cargados: " << red.getNumUsuarios() << " usuarios.\n";
    Medidor::mostrar(memoria, iteraciones, "<fstream> std::ifstream (lectura de archivos)");

    try {
        while (true) {
            cout << "\n=== UdeABook ===\n";
            cout << "Nombre de usuario (0 para salir): ";
            string nombre;
            if (!getline(cin, nombre))
                break;
            if (!nombre.empty() && nombre[nombre.size() - 1] == '\r')
                nombre.erase(nombre.size() - 1);
            if (nombre == "0")
                break;
            if (red.iniciarSesion(nombre))
                red.menuPrincipal();
        }
    } catch (const FinEntrada &) {
        // la entrada se cerro: se sale guardando los cambios
    }

    // I. Actualizacion de datos: se reescriben los archivos con los cambios de la sesion
    Medidor::reiniciar();
    if (gestor.guardar(red)) {
        iteraciones = Medidor::getIteraciones();
        memoria = red.memoria();
        cout << "\nCambios guardados en la carpeta 'datos'.\n";
        Medidor::mostrar(memoria, iteraciones, "<fstream> std::ofstream (escritura de archivos)");
    }
    return 0;
}
