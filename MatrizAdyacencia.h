#ifndef MATRIZADYACENCIA_H
#define MATRIZADYACENCIA_H

#include "ListaDinamica.h"

// Relacion de amistad entre n usuarios con bits empaquetados:
// cada fila usa ceil(n/64) palabras de 64 bits (~n^2/8 bytes en total).
// La amistad es mutua, asi que la matriz siempre es simetrica.
class MatrizAdyacencia
{
private:
    unsigned long long *datos;
    int n;
    int palabrasPorFila;

    static int contarBits(unsigned long long x);

public:
    MatrizAdyacencia(int n = 0);
    MatrizAdyacencia(const MatrizAdyacencia &otra);
    MatrizAdyacencia &operator=(const MatrizAdyacencia &otra);
    ~MatrizAdyacencia();

    // Marca la amistad entre i y j en ambos sentidos
    void establecer(int i, int j);
    bool sonAmigos(int i, int j) const;
    int contarComunes(int i, int j) const;
    int grado(int i) const;
    // Agrega a 'salida' los indices de los amigos de i
    void amigosDe(int i, ListaDinamica<int> &salida) const;

    int getN() const;
    unsigned long memoria() const;
};

#endif // MATRIZADYACENCIA_H
