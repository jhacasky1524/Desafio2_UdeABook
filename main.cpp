#include <iostream>
#include "Fecha.h"
#include "ListaDinamica.h"
#include "MatrizAdyacencia.h"
#include "Medidor.h"

using namespace std;

int main()
{
    // --- Fecha ---
    Fecha a(13, 10, 2026, 9, 30);
    Fecha b(29, 2, 2026);            // 2026 no es bisiesto
    cout << "a = " << a << (a.esValida() ? " (valida)" : " (invalida)") << "\n";
    cout << "b = " << b << (b.esValida() ? " (valida)" : " (invalida)") << "\n";
    Fecha c(a);
    cout << "c == a: " << (c == a) << ", a < 14/10/2026: "
         << (a < Fecha(14, 10, 2026)) << "\n";

    // --- ListaDinamica ---
    ListaDinamica<int> lista;
    for (int i = 0; i < 10; i++)
        lista.agregar(i * i);
    ListaDinamica<int> copia(lista);
    copia[0] = 999;
    cout << "\noriginal[0] = " << lista[0] << ", copia[0] = " << copia[0] << "\n";

    // --- MatrizAdyacencia: n = 130 cruza el limite de palabra de 64 bits ---
    MatrizAdyacencia m(130);
    m.establecer(0, 1);
    m.establecer(0, 2);
    m.establecer(0, 100);
    m.establecer(1, 2);
    m.establecer(1, 100);
    m.establecer(1, 129);
    cout << "\nsonAmigos(0,1) = " << m.sonAmigos(0, 1)
         << ", sonAmigos(1,0) = " << m.sonAmigos(1, 0)
         << ", sonAmigos(0,129) = " << m.sonAmigos(0, 129) << "\n";
    cout << "comunes(0,1) = " << m.contarComunes(0, 1) << " (esperado 2: usuarios 2 y 100)\n";
    cout << "grado(0) = " << m.grado(0) << ", grado(1) = " << m.grado(1) << "\n";

    ListaDinamica<int> amigos;
    m.amigosDe(1, amigos);
    cout << "amigos de 1:";
    for (int i = 0; i < amigos.getTam(); i++)
        cout << ' ' << amigos[i];
    cout << "\n";

    MatrizAdyacencia m2(m);
    m2.establecer(5, 6);
    cout << "copia independiente: m(5,6) = " << m.sonAmigos(5, 6)
         << ", m2(5,6) = " << m2.sonAmigos(5, 6) << "\n";

    Medidor::mostrar(lista.memoria() + copia.memoria() + amigos.memoria()
                     + m.memoria() + m2.memoria());
    return 0;
}
