#include "Mensaje.h"
#include "Medidor.h"

using namespace std;

Mensaje::Mensaje(int idEmisor, int idReceptor, const string &texto, const Fecha &fecha)
    : idEmisor(idEmisor), idReceptor(idReceptor), texto(texto),
      fechaEnvio(fecha), leido(false)
{
}

bool Mensaje::textoValido(const string &texto)
{
    if (texto.size() > static_cast<size_t>(MAX_TEXTO))
        return false;
    for (size_t i = 0; i < texto.size(); i++) {
        Medidor::contar();
        if (texto[i] != ' ' && texto[i] != '\t')
            return true;               // hay al menos un caracter visible
    }
    return false;                      // vacio o solo espacios
}

int Mensaje::getIdEmisor() const { return idEmisor; }
int Mensaje::getIdReceptor() const { return idReceptor; }
const string &Mensaje::getTexto() const { return texto; }
const Fecha &Mensaje::getFecha() const { return fechaEnvio; }
bool Mensaje::estaLeido() const { return leido; }
void Mensaje::marcarLeido() { leido = true; }

unsigned long Mensaje::memoria() const
{
    return sizeof(*this) + Medidor::bytesCadena(texto);
}

ostream &operator<<(ostream &os, const Mensaje &m)
{
    os << '[' << m.fechaEnvio << "] " << m.texto;
    return os;
}
