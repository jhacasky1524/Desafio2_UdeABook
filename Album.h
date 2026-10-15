#ifndef ALBUM_H
#define ALBUM_H

#include <string>
#include "Foto.h"
#include "ListaDinamica.h"

class Album
{
private:
    std::string nombre;
    std::string ruta;
    char privacidad;                 // 'P' publico, 'V' privado, 'A' solo amigos
    ListaDinamica<Foto *> fotos;     // el album es dueno de sus fotos

public:
    Album(const std::string &nombre, const std::string &ruta, char privacidad);
    // El album posee punteros: copiarlo duplicaria los punteros y las fotos
    // se liberarian dos veces. Por eso se prohibe la copia.
    Album(const Album &) = delete;
    Album &operator=(const Album &) = delete;
    ~Album();

    const std::string &getNombre() const;
    const std::string &getRuta() const;
    char getPrivacidad() const;

    void agregarFoto(Foto *foto);          // el album asume la propiedad
    Foto *getFoto(int i) const;            // precondicion: 0 <= i < getNumFotos()
    int getNumFotos() const;

    // Navegacion circular del carrusel
    int indiceSiguiente(int actual) const;
    int indiceAnterior(int actual) const;

    bool visiblePara(bool esDueno, bool esAmigo) const;
    unsigned long memoria() const;
};

#endif // ALBUM_H
