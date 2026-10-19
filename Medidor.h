#ifndef MEDIDOR_H
#define MEDIDOR_H

#include <string>

// Mide el consumo de recursos de cada funcionalidad.
// Los miembros son static porque el contador es global: cualquier clase
// (listas, matriz, busquedas) debe poder sumar iteraciones sin tener una instancia.
class Medidor
{
private:
    static unsigned long iteraciones;

public:
    static void contar(unsigned long n = 1);
    static unsigned long getIteraciones();
    static void reiniciar();
    static void mostrar(unsigned long memoriaBytes);
    // Variante: iteraciones capturadas antes de calcular la memoria (para no sumarlas)
    static void mostrar(unsigned long memoriaBytes, unsigned long iter,
                        const std::string &externos = "");
    // Bytes que una cadena ocupa FUERA del objeto string (en el heap)
    static unsigned long bytesCadena(const std::string &s);
};

#endif // MEDIDOR_H
