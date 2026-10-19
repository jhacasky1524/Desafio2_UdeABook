#ifndef FOTO_H
#define FOTO_H

#include <string>
#include "Fecha.h"
#include "ListaDinamica.h"

class Foto
{
private:
    Fecha fechaCaptura;
    float tamano;
    std::string ruta;
    ListaDinamica<int> likes;   // indices de los usuarios que dieron like

public:
    Foto(const std::string &ruta, const Fecha &fecha, float tamano);

    const Fecha &getFecha() const;
    float getTamano() const;
    const std::string &getRuta() const;

    // Agrega un like sin verificar (uso del cargador de archivos)
    void agregarLike(int idUsuario);
    // Da o quita el like; devuelve true si la foto queda con like de ese usuario
    bool alternarLike(int idUsuario);
    int contarLikes() const;
    bool tieneLikeDe(int idUsuario) const;
    int getLike(int i) const;          // indice del usuario que dio el like i-esimo

    void mostrar() const;   // visualizar = mostrar la ruta
    unsigned long memoria() const;
};

#endif // FOTO_H
