#include "MatrizAdyacencia.h"

MatrizAdyacencia::MatrizAdyacencia(int n)
    : datos(nullptr), n(n), palabrasPorFila((n + 63) / 64)
{
    long total = static_cast<long>(n) * palabrasPorFila;
    datos = new unsigned long long[total > 0 ? total : 1];
    for (long k = 0; k < total; k++) {
        datos[k] = 0;
        Medidor::contar();
    }
}

MatrizAdyacencia::MatrizAdyacencia(const MatrizAdyacencia &otra)
    : datos(nullptr), n(otra.n), palabrasPorFila(otra.palabrasPorFila)
{
    long total = static_cast<long>(n) * palabrasPorFila;
    datos = new unsigned long long[total > 0 ? total : 1];
    for (long k = 0; k < total; k++) {
        datos[k] = otra.datos[k];
        Medidor::contar();
    }
}

MatrizAdyacencia &MatrizAdyacencia::operator=(const MatrizAdyacencia &otra)
{
    if (this != &otra) {
        long total = static_cast<long>(otra.n) * otra.palabrasPorFila;
        unsigned long long *nuevo = new unsigned long long[total > 0 ? total : 1];
        for (long k = 0; k < total; k++) {
            nuevo[k] = otra.datos[k];
            Medidor::contar();
        }
        delete[] datos;
        datos = nuevo;
        n = otra.n;
        palabrasPorFila = otra.palabrasPorFila;
    }
    return *this;
}

MatrizAdyacencia::~MatrizAdyacencia()
{
    delete[] datos;
}

// Conteo de bits en 1 sin instrucciones especificas del compilador (SWAR)
int MatrizAdyacencia::contarBits(unsigned long long x)
{
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return static_cast<int>((x * 0x0101010101010101ULL) >> 56);
}

// Precondicion: 0 <= i, j < n
void MatrizAdyacencia::establecer(int i, int j)
{
    datos[static_cast<long>(i) * palabrasPorFila + j / 64] |= (1ULL << (j % 64));
    datos[static_cast<long>(j) * palabrasPorFila + i / 64] |= (1ULL << (i % 64));
}

bool MatrizAdyacencia::sonAmigos(int i, int j) const
{
    return (datos[static_cast<long>(i) * palabrasPorFila + j / 64] >> (j % 64)) & 1ULL;
}

int MatrizAdyacencia::contarComunes(int i, int j) const
{
    const unsigned long long *fi = datos + static_cast<long>(i) * palabrasPorFila;
    const unsigned long long *fj = datos + static_cast<long>(j) * palabrasPorFila;
    int total = 0;
    for (int w = 0; w < palabrasPorFila; w++) {
        total += contarBits(fi[w] & fj[w]);
        Medidor::contar();
    }
    return total;
}

int MatrizAdyacencia::grado(int i) const
{
    const unsigned long long *fi = datos + static_cast<long>(i) * palabrasPorFila;
    int total = 0;
    for (int w = 0; w < palabrasPorFila; w++) {
        total += contarBits(fi[w]);
        Medidor::contar();
    }
    return total;
}

void MatrizAdyacencia::amigosDe(int i, ListaDinamica<int> &salida) const
{
    const unsigned long long *fi = datos + static_cast<long>(i) * palabrasPorFila;
    for (int w = 0; w < palabrasPorFila; w++) {
        Medidor::contar();
        unsigned long long palabra = fi[w];
        if (palabra == 0)
            continue;                         // salta 64 usuarios de golpe
        for (int b = 0; b < 64; b++) {
            Medidor::contar();
            if ((palabra >> b) & 1ULL)
                salida.agregar(w * 64 + b);
        }
    }
}

int MatrizAdyacencia::getN() const
{
    return n;
}

unsigned long MatrizAdyacencia::memoria() const
{
    return sizeof(*this)
           + static_cast<unsigned long>(n) * palabrasPorFila * sizeof(unsigned long long);
}
