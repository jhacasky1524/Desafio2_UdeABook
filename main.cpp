#include <iostream>
#include "Medidor.h"
#include "Usuario.h"

using namespace std;

static void imprimirLista(const char *titulo, const ListaDinamica<Mensaje *> &l)
{
    cout << titulo << " (" << l.getTam() << ")\n";
    for (int i = 0; i < l.getTam(); i++)
        cout << "   " << *l[i] << "\n";
}

int main()
{
    Usuario u0("juanito76", Fecha(3, 5, 1999), "juan@udea.edu.co",
               "/users/X/juanito76/albums/profile.png", "Medellin", "Colombia",
               Fecha(1, 1, 2025), 0);
    Usuario u1("maria22", Fecha(9, 8, 2001), "maria@udea.edu.co",
               "/users/X/maria22/albums/profile.png", "Bogota", "Colombia",
               Fecha(2, 2, 2025), 1);

    cout << "u0 == u1: " << (u0 == u1) << "\n";

    // --- Album con una foto ---
    Album *al = new Album("Cartagena2026", "/users/X/juanito76/albums/Cartagena2026", 'A');
    al->agregarFoto(new Foto("/users/X/juanito76/albums/Cartagena2026/Malecon.png",
                             Fecha(5, 1, 2026), 2.4f));
    u0.agregarAlbum(al);
    cout << "albumes de u0: " << u0.getNumAlbumes() << "\n";

    // --- Mensajes enviados fuera de orden: u1 debe dejarlos ordenados ---
    Fecha fechas[5] = { Fecha(30, 9, 2026, 10, 0), Fecha(1, 9, 2026, 8, 0),
                        Fecha(5, 10, 2026, 23, 30), Fecha(15, 9, 2026, 12, 0),
                        Fecha(6, 10, 2026, 0, 5) };
    for (int i = 0; i < 5; i++) {
        Mensaje *m = new Mensaje(0, 1, "mensaje " + to_string(i), fechas[i]);
        u0.enviarMensaje(m);
        u1.recibirMensaje(m);
    }
    cout << "orden recibidos de u1:";
    for (int i = 0; i < u1.getNumRecibidos(); i++)
        cout << ' ' << u1.getRecibido(i)->getTexto();
    cout << "   (esperado: 1 3 0 2 4)\n";

    // --- Busqueda por rango 15/09 a 05/10 (el del 5/10 23:30 entra, el del 6/10 no) ---
    ListaDinamica<Mensaje *> enRango;
    int r = u1.buscarPorRango(Fecha(15, 9, 2026), Fecha(5, 10, 2026), enRango);
    cout << "\nrango 15/09-05/10: " << r << " mensajes (esperado 3)\n";
    imprimirLista("resultado", enRango);

    // --- Leer nuevos ---
    ListaDinamica<Mensaje *> nuevos;
    cout << "\nleerNuevos: " << u1.leerNuevos(nuevos) << " (esperado 5)\n";
    ListaDinamica<Mensaje *> otra;
    cout << "leerNuevos otra vez: " << u1.leerNuevos(otra) << " (esperado 0)\n";

    // Mensaje viejo llega despues de leer: debe contar como nuevo
    Mensaje *tarde = new Mensaje(0, 1, "tardio", Fecha(20, 9, 2026, 9, 0));
    u0.enviarMensaje(tarde);
    u1.recibirMensaje(tarde);
    ListaDinamica<Mensaje *> n3;
    cout << "leerNuevos con mensaje viejo recien llegado: " << u1.leerNuevos(n3)
         << " (esperado 1)\n";

    // --- Solicitudes ---
    u1.agregarSolicitud(new SolicitudAmistad(0, Fecha(12, 10, 2026)));
    cout << "\nu1 tiene solicitud de 0: " << u1.tieneSolicitudDe(0)
         << ", de 5: " << u1.tieneSolicitudDe(5) << " (esperado 1 0)\n";
    u1.eliminarSolicitud(0);
    cout << "tras aceptar: " << u1.getNumSolicitudes() << " solicitudes (esperado 0)\n";

    Medidor::mostrar(u0.memoria() + u1.memoria());
    return 0;
}
