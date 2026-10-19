#include "Fecha.h"
#include <chrono>
#include <iomanip>

using namespace std;

Fecha::Fecha(short d, short m, short a, short h, short mi)
    : dia(d), mes(m), anio(a), hora(h), minuto(mi)
{
}

Fecha::Fecha(const Fecha &otra)
    : dia(otra.dia), mes(otra.mes), anio(otra.anio),
      hora(otra.hora), minuto(otra.minuto)
{
}

Fecha &Fecha::operator=(const Fecha &otra)
{
    if (this != &otra) {
        dia = otra.dia;
        mes = otra.mes;
        anio = otra.anio;
        hora = otra.hora;
        minuto = otra.minuto;
    }
    return *this;
}

short Fecha::getDia() const { return dia; }
short Fecha::getMes() const { return mes; }
short Fecha::getAnio() const { return anio; }
short Fecha::getHora() const { return hora; }
short Fecha::getMinuto() const { return minuto; }
void Fecha::setDia(short d) { dia = d; }
void Fecha::setMes(short m) { mes = m; }
void Fecha::setAnio(short a) { anio = a; }
void Fecha::setHora(short h) { hora = h; }
void Fecha::setMinuto(short mi) { minuto = mi; }

bool Fecha::esBisiesto(short a)
{
    return (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
}

short Fecha::diasDelMes(short m, short a)
{
    switch (m) {
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        return esBisiesto(a) ? 29 : 28;
    default:
        return 31;
    }
}

bool Fecha::esValida() const
{
    if (anio < 1 || mes < 1 || mes > 12)
        return false;
    if (dia < 1 || dia > diasDelMes(mes, anio))
        return false;
    if (hora < 0 || hora > 23 || minuto < 0 || minuto > 59)
        return false;
    return true;
}

Fecha Fecha::ahora()
{
    using namespace std::chrono;
    long long seg = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    seg -= 5 * 3600;                                   // UTC-5
    long long dias = seg / 86400, resto = seg % 86400;
    if (resto < 0) { resto += 86400; dias--; }
    // Dias desde 1970 -> fecha civil (algoritmo de H. Hinnant)
    dias += 719468;
    long long era = (dias >= 0 ? dias : dias - 146096) / 146097;
    long long doe = dias - era * 146097;
    long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long long y = yoe + era * 400;
    long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    long long mp = (5 * doy + 2) / 153;
    long long d = doy - (153 * mp + 2) / 5 + 1;
    long long m = mp < 10 ? mp + 3 : mp - 9;
    if (m <= 2) y++;
    return Fecha(static_cast<short>(d), static_cast<short>(m), static_cast<short>(y),
                 static_cast<short>(resto / 3600), static_cast<short>((resto % 3600) / 60));
}

bool Fecha::operator<(const Fecha &otra) const
{
    if (anio != otra.anio) return anio < otra.anio;
    if (mes != otra.mes) return mes < otra.mes;
    if (dia != otra.dia) return dia < otra.dia;
    if (hora != otra.hora) return hora < otra.hora;
    return minuto < otra.minuto;
}

bool Fecha::operator==(const Fecha &otra) const
{
    return anio == otra.anio && mes == otra.mes && dia == otra.dia
           && hora == otra.hora && minuto == otra.minuto;
}

ostream &operator<<(ostream &os, const Fecha &f)
{
    os << setfill('0') << setw(2) << f.dia << '/'
       << setw(2) << f.mes << '/'
       << setw(4) << f.anio << ' '
       << setw(2) << f.hora << ':'
       << setw(2) << f.minuto << setfill(' ');
    return os;
}
