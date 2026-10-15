#ifndef SOLICITUDAMISTAD_H
#define SOLICITUDAMISTAD_H

#include "Fecha.h"

class SolicitudAmistad
{
private:
    int idEmisor;
    Fecha fecha;

public:
    SolicitudAmistad(int idEmisor, const Fecha &fecha);

    int getIdEmisor() const;
    const Fecha &getFecha() const;
    unsigned long memoria() const;
};

#endif // SOLICITUDAMISTAD_H
