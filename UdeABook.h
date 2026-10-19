#ifndef UDEABOOK_H
#define UDEABOOK_H

#include <random>
#include <string>
#include "MatrizAdyacencia.h"
#include "Usuario.h"

// Se lanza cuando la entrada de consola se cierra (fin de archivo)
class FinEntrada {};

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

    // Datos de la sesion actual (fotos de perfil de amigos)
    std::mt19937 generador;
    ListaDinamica<int> amigosSesion;   // indices de los amigos del usuario actual
    ListaDinamica<int> pesosSesion;    // peso de cada amigo (2, 5 o 7)
    ListaDinamica<int> grupoFotos;     // posiciones (en amigosSesion) del grupo mostrado

    // Medicion de recursos de la funcionalidad en curso
    unsigned long bytesLocales;
    std::string externos;

    void crecer();
    void iniciarMedicion();
    void mostrarMedicion();

    void prepararSesion();
    void sortearGrupo();
    bool validarSolicitud(int destino, std::string &motivo) const;
    void crearSolicitud(int destino);
    bool esAmigoDelActual(int idx) const;
    void carrusel(Album *album);

public:
    UdeABook();
    UdeABook(const UdeABook &) = delete;
    UdeABook &operator=(const UdeABook &) = delete;
    ~UdeABook();

    // ---- Datos ----
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

    // ---- Funcionalidades (cada una termina mostrando iteraciones y memoria) ----
    bool iniciarSesion(const std::string &nombre);      // II
    void cerrarSesion();
    void menuPrincipal();
    void refrescarFotosAmigos();                         // II (opcion N)
    void administrarAmistades();                         // III
    void enviarSolicitud();                              // III.a
    void aceptarSolicitudes();                           // III.b
    void visitarPerfil();                                // IV
    void sugerencias();                                  // V
    void administrarMensajes();                          // VI
    void enviarMensaje();                                // VI.a
    void leerNuevos();                                   // VI.b
    void buscarMensajesPorRango();                       // VI.c
};

#endif // UDEABOOK_H
