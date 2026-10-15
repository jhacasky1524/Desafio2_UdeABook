#ifndef MENSAJE_H
#define MENSAJE_H

#include <iostream>
#include <string>
#include "Fecha.h"

class Mensaje
{
private:
    int idEmisor;
    int idReceptor;
    std::string texto;
    Fecha fechaEnvio;
    bool leido;

public:
    static const int MAX_TEXTO = 512;

    Mensaje(int idEmisor, int idReceptor, const std::string &texto, const Fecha &fecha);

    // Un texto es valido si no esta vacio (ni solo espacios) y no pasa de 512 caracteres
    static bool textoValido(const std::string &texto);

    int getIdEmisor() const;
    int getIdReceptor() const;
    const std::string &getTexto() const;
    const Fecha &getFecha() const;
    bool estaLeido() const;
    void marcarLeido();

    unsigned long memoria() const;

    friend std::ostream &operator<<(std::ostream &os, const Mensaje &m);
};

#endif // MENSAJE_H
