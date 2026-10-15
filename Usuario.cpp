#include "Usuario.h"

using namespace std;

Usuario::Usuario(const string &nombreUsuario, const Fecha &nacimiento,
                 const string &correo, const string &rutaFoto,
                 const string &ciudad, const string &pais,
                 const Fecha &inscripcion, int indice)
    : nombreUsuario(nombreUsuario), correo(correo), rutaFoto(rutaFoto),
      ciudad(ciudad), pais(pais), fechaNacimiento(nacimiento),
      fechaInscripcion(inscripcion), indice(indice), primerNoLeido(0)
{
}

Usuario::~Usuario()
{
    for (int i = 0; i < albumes.getTam(); i++) {
        delete albumes[i];
        Medidor::contar();
    }
    for (int i = 0; i < enviados.getTam(); i++) {
        delete enviados[i];
        Medidor::contar();
    }
    for (int i = 0; i < solicitudes.getTam(); i++) {
        delete solicitudes[i];
        Medidor::contar();
    }
    // 'recibidos' no se libera: esos mensajes pertenecen a sus emisores
}

const string &Usuario::getNombreUsuario() const { return nombreUsuario; }
const string &Usuario::getCorreo() const { return correo; }
const string &Usuario::getRutaFoto() const { return rutaFoto; }
const string &Usuario::getCiudad() const { return ciudad; }
const string &Usuario::getPais() const { return pais; }
const Fecha &Usuario::getFechaNacimiento() const { return fechaNacimiento; }
const Fecha &Usuario::getFechaInscripcion() const { return fechaInscripcion; }
int Usuario::getIndice() const { return indice; }
void Usuario::setIndice(int i) { indice = i; }
void Usuario::setCorreo(const string &c) { correo = c; }
void Usuario::setRutaFoto(const string &r) { rutaFoto = r; }
void Usuario::setCiudad(const string &c) { ciudad = c; }
void Usuario::setPais(const string &p) { pais = p; }

// ---------- Albumes ----------
void Usuario::agregarAlbum(Album *album) { albumes.agregar(album); }
Album *Usuario::getAlbum(int i) const { return albumes[i]; }
int Usuario::getNumAlbumes() const { return albumes.getTam(); }

// ---------- Solicitudes ----------
void Usuario::agregarSolicitud(SolicitudAmistad *s) { solicitudes.agregar(s); }

bool Usuario::tieneSolicitudDe(int idEmisor) const
{
    for (int i = 0; i < solicitudes.getTam(); i++) {
        Medidor::contar();
        if (solicitudes[i]->getIdEmisor() == idEmisor)
            return true;
    }
    return false;
}

SolicitudAmistad *Usuario::getSolicitud(int i) const { return solicitudes[i]; }
int Usuario::getNumSolicitudes() const { return solicitudes.getTam(); }

void Usuario::eliminarSolicitud(int i)
{
    delete solicitudes[i];
    solicitudes.eliminar(i);
}

// ---------- Mensajes ----------
void Usuario::enviarMensaje(Mensaje *m) { enviados.agregar(m); }
int Usuario::getNumEnviados() const { return enviados.getTam(); }
Mensaje *Usuario::getEnviado(int i) const { return enviados[i]; }
int Usuario::getNumRecibidos() const { return recibidos.getTam(); }
Mensaje *Usuario::getRecibido(int i) const { return recibidos[i]; }

void Usuario::recibirMensaje(Mensaje *m)
{
    recibidos.agregar(m);
    // Lo normal es que sea el mas reciente (0 desplazamientos); si no,
    // se desplaza hacia atras hasta quedar ordenado.
    int pos = recibidos.getTam() - 1;
    while (pos > 0 && recibidos[pos]->getFecha() < recibidos[pos - 1]->getFecha()) {
        Mensaje *tmp = recibidos[pos];
        recibidos[pos] = recibidos[pos - 1];
        recibidos[pos - 1] = tmp;
        pos--;
        Medidor::contar();
    }
    // Si quedo antes de primerNoLeido, sigue siendo "no leido": se retrocede el indice
    if (pos < primerNoLeido)
        primerNoLeido = pos;
}

void Usuario::recalcularPrimerNoLeido()
{
    primerNoLeido = recibidos.getTam();
    for (int i = 0; i < recibidos.getTam(); i++) {
        Medidor::contar();
        if (!recibidos[i]->estaLeido()) {
            primerNoLeido = i;
            return;
        }
    }
}

int Usuario::leerNuevos(ListaDinamica<Mensaje *> &nuevos)
{
    int cuantos = 0;
    for (int i = primerNoLeido; i < recibidos.getTam(); i++) {
        Medidor::contar();
        if (!recibidos[i]->estaLeido()) {
            nuevos.agregar(recibidos[i]);
            recibidos[i]->marcarLeido();
            cuantos++;
        }
    }
    primerNoLeido = recibidos.getTam();
    return cuantos;
}

int Usuario::buscarPorRango(const Fecha &inicio, const Fecha &fin,
                            ListaDinamica<Mensaje *> &resultado) const
{
    // Comparacion por dia: el inicio abarca desde las 00:00 y el fin hasta las 23:59
    Fecha lo(inicio.getDia(), inicio.getMes(), inicio.getAnio(), 0, 0);
    Fecha hi(fin.getDia(), fin.getMes(), fin.getAnio(), 23, 59);
    int n = recibidos.getTam();

    // Primera posicion con fecha >= lo (busqueda binaria)
    int a = 0, b = n;
    while (a < b) {
        int mid = (a + b) / 2;
        Medidor::contar();
        if (recibidos[mid]->getFecha() < lo)
            a = mid + 1;
        else
            b = mid;
    }
    int primero = a;

    // Primera posicion con fecha > hi
    b = n;
    while (a < b) {
        int mid = (a + b) / 2;
        Medidor::contar();
        if (hi < recibidos[mid]->getFecha())
            b = mid;
        else
            a = mid + 1;
    }
    int fin_ = a;

    for (int k = primero; k < fin_; k++) {
        resultado.agregar(recibidos[k]);
        Medidor::contar();
    }
    return fin_ - primero;
}

bool Usuario::operator==(const Usuario &otro) const
{
    return nombreUsuario == otro.nombreUsuario;
}

unsigned long Usuario::memoria() const
{
    unsigned long total = sizeof(*this)
        + Medidor::bytesCadena(nombreUsuario) + Medidor::bytesCadena(correo)
        + Medidor::bytesCadena(rutaFoto) + Medidor::bytesCadena(ciudad)
        + Medidor::bytesCadena(pais)
        + albumes.bytesDinamicos() + enviados.bytesDinamicos()
        + recibidos.bytesDinamicos() + solicitudes.bytesDinamicos();

    for (int i = 0; i < albumes.getTam(); i++) {
        total += albumes[i]->memoria();
        Medidor::contar();
    }
    for (int i = 0; i < enviados.getTam(); i++) {     // solo los que posee
        total += enviados[i]->memoria();
        Medidor::contar();
    }
    for (int i = 0; i < solicitudes.getTam(); i++) {
        total += solicitudes[i]->memoria();
        Medidor::contar();
    }
    return total;
}
