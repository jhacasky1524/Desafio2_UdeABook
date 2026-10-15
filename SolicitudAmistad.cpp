#include "SolicitudAmistad.h"

SolicitudAmistad::SolicitudAmistad(int idEmisor, const Fecha &fecha)
    : idEmisor(idEmisor), fecha(fecha)
{
}

int SolicitudAmistad::getIdEmisor() const { return idEmisor; }
const Fecha &SolicitudAmistad::getFecha() const { return fecha; }

unsigned long SolicitudAmistad::memoria() const
{
    return sizeof(*this);
}
