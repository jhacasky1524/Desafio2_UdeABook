#ifndef LISTADINAMICA_H
#define LISTADINAMICA_H

#include "Medidor.h"

// Arreglo dinamico generico propio (reemplaza a std::vector).
// Regla de los tres: como posee memoria con new[], necesita
// destructor, constructor de copia y operador de asignacion propios.
template <typename T>
class ListaDinamica
{
private:
    T *datos;
    int tam;
    int cap;

    void crecer()
    {
        int nuevaCap = (cap > 0) ? cap * 2 : 4;   // duplicar: O(1) amortizado
        T *nuevo = new T[nuevaCap];
        for (int i = 0; i < tam; i++) {
            nuevo[i] = datos[i];
            Medidor::contar();
        }
        delete[] datos;
        datos = nuevo;
        cap = nuevaCap;
    }

public:
    ListaDinamica(int capInicial = 4)
        : datos(nullptr), tam(0), cap(capInicial > 0 ? capInicial : 4)
    {
        datos = new T[cap];
    }

    // Copia profunda: reserva su propio bloque y copia elemento por elemento
    ListaDinamica(const ListaDinamica &otra)
        : datos(nullptr), tam(otra.tam), cap(otra.cap)
    {
        datos = new T[cap];
        for (int i = 0; i < tam; i++) {
            datos[i] = otra.datos[i];
            Medidor::contar();
        }
    }

    ListaDinamica &operator=(const ListaDinamica &otra)
    {
        if (this != &otra) {
            T *nuevo = new T[otra.cap];
            for (int i = 0; i < otra.tam; i++) {
                nuevo[i] = otra.datos[i];
                Medidor::contar();
            }
            delete[] datos;
            datos = nuevo;
            tam = otra.tam;
            cap = otra.cap;
        }
        return *this;
    }

    ~ListaDinamica()
    {
        delete[] datos;
    }

    void agregar(const T &valor)
    {
        if (tam == cap)
            crecer();
        datos[tam++] = valor;
    }

    // Precondicion: 0 <= i < tam (quien llama debe validar el indice)
    T &operator[](int i) { return datos[i]; }
    const T &operator[](int i) const { return datos[i]; }

    // Elimina el elemento i desplazando los siguientes hacia la izquierda
    void eliminar(int i)
    {
        for (int k = i; k < tam - 1; k++) {
            datos[k] = datos[k + 1];
            Medidor::contar();
        }
        tam--;
    }

    int getTam() const { return tam; }

    // Bytes ocupados: el objeto mismo + el bloque reservado (capacidad, no tamano)
    unsigned long memoria() const
    {
        return sizeof(*this) + static_cast<unsigned long>(cap) * sizeof(T);
    }
};

#endif // LISTADINAMICA_H
