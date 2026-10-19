#include "Foto.h"
#include <iostream>

using namespace std;

Foto::Foto(const string &ruta, const Fecha &fecha, float tamano)
    : fechaCaptura(fecha), tamano(tamano), ruta(ruta)
{
}

const Fecha &Foto::getFecha() const { return fechaCaptura; }
float Foto::getTamano() const { return tamano; }
const string &Foto::getRuta() const { return ruta; }

void Foto::agregarLike(int idUsuario)
{
    likes.agregar(idUsuario);
}

bool Foto::alternarLike(int idUsuario)
{
    for (int k = 0; k < likes.getTam(); k++) {
        Medidor::contar();
        if (likes[k] == idUsuario) {
            likes.eliminar(k);
            return false;
        }
    }
    likes.agregar(idUsuario);
    return true;
}

int Foto::contarLikes() const
{
    return likes.getTam();
}

bool Foto::tieneLikeDe(int idUsuario) const
{
    for (int k = 0; k < likes.getTam(); k++) {
        Medidor::contar();
        if (likes[k] == idUsuario)
            return true;
    }
    return false;
}

int Foto::getLike(int i) const
{
    return likes[i];
}

void Foto::mostrar() const
{
    cout << ruta << "  (" << contarLikes() << " likes)\n";
}

unsigned long Foto::memoria() const
{
    // sizeof(*this) ya incluye los objetos Fecha, string y ListaDinamica
    return sizeof(*this) + Medidor::bytesCadena(ruta) + likes.bytesDinamicos();
}
