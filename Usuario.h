#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include "Album.h"
#include "Fecha.h"
#include "ListaDinamica.h"
#include "Mensaje.h"
#include "SolicitudAmistad.h"

// Propiedad de la memoria:
//  - albumes, enviados y solicitudes: el Usuario es dueno y los libera.
//  - recibidos: solo apunta a mensajes cuyo dueno es el emisor (no los libera).
class Usuario
{
private:
    std::string nombreUsuario, correo, rutaFoto, ciudad, pais;
    Fecha fechaNacimiento, fechaInscripcion;
    int indice;            // posicion en la lista de usuarios y en la matriz
    int primerNoLeido;     // los mensajes no leidos estan desde esta posicion
    ListaDinamica<Album *> albumes;
    ListaDinamica<Mensaje *> enviados;
    ListaDinamica<Mensaje *> recibidos;       // ordenados por fecha
    ListaDinamica<SolicitudAmistad *> solicitudes;

public:
    Usuario(const std::string &nombreUsuario, const Fecha &nacimiento,
            const std::string &correo, const std::string &rutaFoto,
            const std::string &ciudad, const std::string &pais,
            const Fecha &inscripcion, int indice = -1);
    // Posee punteros: la copia esta prohibida (evita liberar dos veces)
    Usuario(const Usuario &) = delete;
    Usuario &operator=(const Usuario &) = delete;
    ~Usuario();

    // Getters y setters
    const std::string &getNombreUsuario() const;
    const std::string &getCorreo() const;
    const std::string &getRutaFoto() const;
    const std::string &getCiudad() const;
    const std::string &getPais() const;
    const Fecha &getFechaNacimiento() const;
    const Fecha &getFechaInscripcion() const;
    int getIndice() const;
    void setIndice(int i);
    void setCorreo(const std::string &c);
    void setRutaFoto(const std::string &r);
    void setCiudad(const std::string &c);
    void setPais(const std::string &p);

    // Albumes
    void agregarAlbum(Album *album);
    Album *getAlbum(int i) const;
    int getNumAlbumes() const;

    // Solicitudes recibidas
    void agregarSolicitud(SolicitudAmistad *s);
    bool tieneSolicitudDe(int idEmisor) const;
    SolicitudAmistad *getSolicitud(int i) const;
    int getNumSolicitudes() const;
    void eliminarSolicitud(int i);             // la libera y la quita de la lista

    // Mensajes
    void enviarMensaje(Mensaje *m);            // asume la propiedad
    void recibirMensaje(Mensaje *m);           // lo deja ordenado por fecha
    int getNumEnviados() const;
    Mensaje *getEnviado(int i) const;
    int getNumRecibidos() const;
    Mensaje *getRecibido(int i) const;
    void recalcularPrimerNoLeido();            // tras cargar datos de archivo
    // Reune en 'nuevos' los mensajes no leidos y los marca como leidos
    int leerNuevos(ListaDinamica<Mensaje *> &nuevos);
    // Mensajes recibidos entre dos fechas, ambas inclusive (comparacion por dia)
    int buscarPorRango(const Fecha &inicio, const Fecha &fin,
                       ListaDinamica<Mensaje *> &resultado) const;

    bool operator==(const Usuario &otro) const;   // mismo nombre de usuario
    unsigned long memoria() const;
};

#endif // USUARIO_H
