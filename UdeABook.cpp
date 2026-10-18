#include "UdeABook.h"

using namespace std;

UdeABook::UdeABook()
    : usuarios(nullptr), numUsuarios(0), capacidad(16), amistad(nullptr), actual(-1)
{
    usuarios = new Usuario *[capacidad];
}

UdeABook::~UdeABook()
{
    for (int i = 0; i < numUsuarios; i++) {
        delete usuarios[i];
        Medidor::contar();
    }
    delete[] usuarios;
    delete amistad;
}

void UdeABook::crecer()
{
    int nuevaCap = capacidad * 2;
    Usuario **nuevo = new Usuario *[nuevaCap];
    for (int i = 0; i < numUsuarios; i++) {
        nuevo[i] = usuarios[i];
        Medidor::contar();
    }
    delete[] usuarios;
    usuarios = nuevo;
    capacidad = nuevaCap;
}

int UdeABook::agregarUsuario(Usuario *u)
{
    // Busqueda binaria del punto de insercion
    int a = 0, b = numUsuarios;
    while (a < b) {
        int mid = (a + b) / 2;
        Medidor::contar();
        if (usuarios[mid]->getNombreUsuario() < u->getNombreUsuario())
            a = mid + 1;
        else
            b = mid;
    }
    if (a < numUsuarios && usuarios[a]->getNombreUsuario() == u->getNombreUsuario())
        return -1;                         // nombre repetido

    if (numUsuarios == capacidad)
        crecer();
    for (int k = numUsuarios; k > a; k--) {   // con datos ya ordenados no se ejecuta
        usuarios[k] = usuarios[k - 1];
        Medidor::contar();
    }
    usuarios[a] = u;
    numUsuarios++;
    return a;
}

void UdeABook::crearMatriz()
{
    for (int i = 0; i < numUsuarios; i++) {
        usuarios[i]->setIndice(i);
        Medidor::contar();
    }
    delete amistad;
    amistad = new MatrizAdyacencia(numUsuarios);
}

int UdeABook::buscarUsuario(const string &nombre) const
{
    int a = 0, b = numUsuarios;
    while (a < b) {
        int mid = (a + b) / 2;
        Medidor::contar();
        const string &actualNombre = usuarios[mid]->getNombreUsuario();
        if (actualNombre == nombre)
            return mid;
        if (actualNombre < nombre)
            a = mid + 1;
        else
            b = mid;
    }
    return -1;
}

Usuario *UdeABook::getUsuario(int i) const { return usuarios[i]; }
int UdeABook::getNumUsuarios() const { return numUsuarios; }
MatrizAdyacencia *UdeABook::getMatriz() const { return amistad; }
int UdeABook::getActual() const { return actual; }
void UdeABook::setActual(int i) { actual = i; }

unsigned long UdeABook::memoria() const
{
    unsigned long total = sizeof(*this)
                          + static_cast<unsigned long>(capacidad) * sizeof(Usuario *);
    if (amistad)
        total += amistad->memoria();
    for (int i = 0; i < numUsuarios; i++) {
        total += usuarios[i]->memoria();
        Medidor::contar();
    }
    return total;
}
