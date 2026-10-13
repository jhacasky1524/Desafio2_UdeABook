#ifndef MEDIDOR_H
#define MEDIDOR_H

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
};

#endif // MEDIDOR_H
