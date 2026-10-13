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
