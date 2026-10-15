#include "Album.h"

using namespace std;

Album::Album(const string &nombre, const string &ruta, char privacidad)
    : nombre(nombre), ruta(ruta), privacidad(privacidad)
{
}

Album::~Album()
{
    for (int i = 0; i < fotos.getTam(); i++) {
        delete fotos[i];
        Medidor::contar();
    }
}

const string &Album::getNombre() const { return nombre; }
const string &Album::getRuta() const { return ruta; }
char Album::getPrivacidad() const { return privacidad; }

void Album::agregarFoto(Foto *foto)
{
    fotos.agregar(foto);
}

Foto *Album::getFoto(int i) const
{
    return fotos[i];
}

int Album::getNumFotos() const
{
    return fotos.getTam();
}

int Album::indiceSiguiente(int actual) const
{
    return (actual + 1) % fotos.getTam();
}

int Album::indiceAnterior(int actual) const
{
    return (actual - 1 + fotos.getTam()) % fotos.getTam();
}

bool Album::visiblePara(bool esDueno, bool esAmigo) const
{
    switch (privacidad) {
    case 'P': return true;
    case 'A': return esDueno || esAmigo;
    default:  return esDueno;        // 'V' privado
    }
}

unsigned long Album::memoria() const
{
    unsigned long total = sizeof(*this) + Medidor::bytesCadena(nombre)
                          + Medidor::bytesCadena(ruta) + fotos.bytesDinamicos();
    for (int i = 0; i < fotos.getTam(); i++) {
        total += fotos[i]->memoria();
        Medidor::contar();
    }
    return total;
}
