#include <iostream>
#include "Album.h"
#include "Fecha.h"
#include "ListaDinamica.h"
#include "MatrizAdyacencia.h"
#include "Medidor.h"
#include "Mensaje.h"
#include "SolicitudAmistad.h"

using namespace std;

int main()
{
    // ---------- Foto y Album ----------
    Album album("Cartagena2026", "/users/X/juanito76/albums/Cartagena2026", 'A');
    album.agregarFoto(new Foto("/users/X/juanito76/albums/Cartagena2026/Malecon2612.png",
                               Fecha(5, 1, 2026), 2.4f));
    album.agregarFoto(new Foto("/users/X/juanito76/albums/Cartagena2026/Bocagrande1.png",
                               Fecha(6, 1, 2026), 3.1f));
    album.agregarFoto(new Foto("/users/X/juanito76/albums/Cartagena2026/Castillo.png",
                               Fecha(7, 1, 2026), 1.8f));

    cout << "Album '" << album.getNombre() << "' con " << album.getNumFotos() << " fotos\n";
    cout << "visible (dueno, amigo, extrano): " << album.visiblePara(true, false)
         << album.visiblePara(false, true) << album.visiblePara(false, false)
         << "  (esperado 110)\n";

    int pos = 0;
    cout << "carrusel siguiente x3: ";
    for (int k = 0; k < 3; k++) {
        pos = album.indiceSiguiente(pos);
        cout << pos << ' ';
    }
    cout << " | previa desde 0: " << album.indiceAnterior(0) << " (esperado 2)\n";

    Foto *f = album.getFoto(1);
    cout << "like 7: " << f->alternarLike(7) << ", like 9: " << f->alternarLike(9)
         << ", likes = " << f->contarLikes();
    cout << ", quitar like 7: " << f->alternarLike(7) << ", likes = " << f->contarLikes() << "\n";
    f->mostrar();

    // ---------- Mensaje ----------
    Mensaje m(1, 2, "Hola, como vas?", Fecha(13, 10, 2026, 10, 15));
    cout << "\n" << m << " | leido: " << m.estaLeido();
    m.marcarLeido();
    cout << " -> " << m.estaLeido() << "\n";
    string largo(513, 'a');
    cout << "validos: normal=" << Mensaje::textoValido("hola")
         << " vacio=" << Mensaje::textoValido("")
         << " espacios=" << Mensaje::textoValido("   ")
         << " 513car=" << Mensaje::textoValido(largo) << "  (esperado 1000)\n";

    // ---------- SolicitudAmistad ----------
    SolicitudAmistad s(4, Fecha(12, 10, 2026));
    cout << "solicitud de " << s.getIdEmisor() << " el " << s.getFecha() << "\n";

    // ---------- Matriz (prueba anterior resumida) ----------
    MatrizAdyacencia mat(130);
    mat.establecer(0, 1); mat.establecer(0, 2); mat.establecer(0, 100);
    mat.establecer(1, 2); mat.establecer(1, 100); mat.establecer(1, 129);
    cout << "\ncomunes(0,1) = " << mat.contarComunes(0, 1) << " (esperado 2)\n";

    Medidor::mostrar(album.memoria() + m.memoria() + s.memoria() + mat.memoria());
    return 0;
}
