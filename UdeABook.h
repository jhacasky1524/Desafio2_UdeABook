#ifndef UDEABOOK_H
#define UDEABOOK_H

#include <string>
#include "MatrizAdyacencia.h"
#include "Usuario.h"

// Raiz del sistema. Los usuarios se mantienen ordenados por nombre para poder
// buscarlos con busqueda binaria; la posicion de cada usuario es su indice en
// la matriz de amistad.
class UdeABook
{
private:
    Usuario **usuarios;
    int numUsuarios;
    int capacidad;
    MatrizAdyacencia *amistad;
    int actual;                // indice del usuario con sesion iniciada (-1 si ninguno)

    void crecer();

public:
    UdeABook();
    UdeABook(const UdeABook &) = delete;
    UdeABook &operator=(const UdeABook &) = delete;
    ~UdeABook();

    // Inserta el usuario en su posicion ordenada. Devuelve la posicion o -1 si
    // el nombre ya existe (en ese caso NO asume la propiedad).
    int agregarUsuario(Usuario *u);
    // Fija los indices definitivos y crea la matriz de amistad (n final)
    void crearMatriz();

    int buscarUsuario(const std::string &nombre) const;   // -1 si no existe
    Usuario *getUsuario(int i) const;
    int getNumUsuarios() const;
    MatrizAdyacencia *getMatriz() const;
    int getActual() const;
    void setActual(int i);

    unsigned long memoria() const;
};

#endif // UDEABOOK_H
