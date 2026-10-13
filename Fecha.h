#ifndef FECHA_H
#define FECHA_H

#include <iostream>

class Fecha
{
private:
    short dia, mes, anio, hora, minuto;

    static bool esBisiesto(short a);
    static short diasDelMes(short m, short a);

public:
    Fecha(short d = 1, short m = 1, short a = 1970, short h = 0, short mi = 0);
    Fecha(const Fecha &otra);
    Fecha &operator=(const Fecha &otra);

    short getDia() const;
    short getMes() const;
    short getAnio() const;
    short getHora() const;
    short getMinuto() const;
    void setDia(short d);
    void setMes(short m);
    void setAnio(short a);
    void setHora(short h);
    void setMinuto(short mi);

    bool esValida() const;

    // Orden cronologico: anio, mes, dia, hora, minuto
    bool operator<(const Fecha &otra) const;
    bool operator==(const Fecha &otra) const;

    friend std::ostream &operator<<(std::ostream &os, const Fecha &f);
};

#endif // FECHA_H
