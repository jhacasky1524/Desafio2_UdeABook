#include "Medidor.h"
#include <iostream>

using namespace std;

unsigned long Medidor::iteraciones = 0;

void Medidor::contar(unsigned long n)
{
    iteraciones += n;
}

unsigned long Medidor::getIteraciones()
{
    return iteraciones;
}

void Medidor::reiniciar()
{
    iteraciones = 0;
}

void Medidor::mostrar(unsigned long memoriaBytes)
{
    cout << "\n--- Consumo de recursos ---\n";
    cout << "Iteraciones: " << iteraciones << "\n";
    cout << "Memoria: " << memoriaBytes << " bytes\n";
}

unsigned long Medidor::bytesCadena(const string &s)
{
    // Con cadenas cortas (<= 15 caracteres) el texto vive dentro del propio objeto
    return s.capacity() > 15 ? s.capacity() + 1 : 0;
}

void Medidor::mostrar(unsigned long memoriaBytes, unsigned long iter, const string &externos)
{
    cout << "\n--- Consumo de recursos ---\n";
    cout << "Iteraciones: " << iter << "\n";
    cout << "Memoria: " << memoriaBytes << " bytes\n";
    if (!externos.empty())
        cout << "Componente externo usado: " << externos << "\n";
}
