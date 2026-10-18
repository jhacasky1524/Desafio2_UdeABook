#include "GestorArchivos.h"
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

// ---------- utilidades locales ----------

// Divide la linea en hasta maxCampos campos. El ultimo campo recibe el resto
// de la linea completo (asi un texto con ';' no se rompe). Devuelve cuantos campos hubo.
static int dividir(const string &linea, string *campos, int maxCampos)
{
    int n = 0;
    size_t pos = 0;
    while (n < maxCampos - 1) {
        size_t idx = linea.find(';', pos);
        if (idx == string::npos)
            break;
        campos[n++] = linea.substr(pos, idx - pos);
        pos = idx + 1;
        Medidor::contar();
    }
    campos[n++] = linea.substr(pos);
    return n;
}

// Lee una linea quitando el '\r' de los archivos creados en Windows
static bool leerLinea(ifstream &in, string &linea)
{
    while (getline(in, linea)) {
        Medidor::contar();
        if (!linea.empty() && linea[linea.size() - 1] == '\r')
            linea.erase(linea.size() - 1);
        if (!linea.empty())
            return true;
    }
    return false;
}

// "AAAA-MM-DD" o "AAAA-MM-DD HH:MM"
static bool parseFecha(const string &s, Fecha &f)
{
    if (s.size() < 10)
        return false;
    short hora = 0, minuto = 0;
    if (s.size() >= 16) {
        hora = static_cast<short>(stoi(s.substr(11, 2)));
        minuto = static_cast<short>(stoi(s.substr(14, 2)));
    }
    f = Fecha(static_cast<short>(stoi(s.substr(8, 2))),
              static_cast<short>(stoi(s.substr(5, 2))),
              static_cast<short>(stoi(s.substr(0, 4))), hora, minuto);
    return f.esValida();
}

static void escribirFecha(ostream &os, const Fecha &f, bool conHora)
{
    os << setfill('0') << setw(4) << f.getAnio() << '-' << setw(2) << f.getMes()
       << '-' << setw(2) << f.getDia();
    if (conHora)
        os << ' ' << setw(2) << f.getHora() << ':' << setw(2) << f.getMinuto();
    os << setfill(' ');
}

// ---------- clase ----------

GestorArchivos::GestorArchivos(const string &carpeta)
    : carpeta(carpeta), lineasIgnoradas(0)
{
}

int GestorArchivos::getLineasIgnoradas() const { return lineasIgnoradas; }

string GestorArchivos::ruta(const char *archivo) const
{
    return carpeta + "/" + archivo;
}

bool GestorArchivos::cargar(UdeABook &red)
{
    lineasIgnoradas = 0;
    // El orden importa: cada paso necesita lo que cargo el anterior
    if (!cargarUsuarios(red)) return false;
    red.crearMatriz();
    if (!cargarAmistades(red)) return false;
    if (!cargarSolicitudes(red)) return false;
    if (!cargarAlbumes(red)) return false;
    if (!cargarFotos(red)) return false;
    if (!cargarMensajes(red)) return false;
    for (int i = 0; i < red.getNumUsuarios(); i++)
        red.getUsuario(i)->recalcularPrimerNoLeido();
    return true;
}

bool GestorArchivos::cargarUsuarios(UdeABook &red)
{
    ifstream in(ruta("usuarios.txt"));
    if (!in) { cerr << "No se pudo abrir " << ruta("usuarios.txt") << "\n"; return false; }
    string linea, c[7];
    while (leerLinea(in, linea)) {
        try {
            Fecha nac, insc;
            if (dividir(linea, c, 7) != 7 || !parseFecha(c[1], nac) || !parseFecha(c[6], insc)) {
                lineasIgnoradas++;
                continue;
            }
            Usuario *u = new Usuario(c[0], nac, c[2], c[3], c[4], c[5], insc);
            if (red.agregarUsuario(u) < 0) {   // nombre repetido
                delete u;
                lineasIgnoradas++;
            }
        } catch (...) {
            lineasIgnoradas++;
        }
    }
    return true;
}

bool GestorArchivos::cargarAmistades(UdeABook &red)
{
    ifstream in(ruta("amistades.txt"));
    if (!in) { cerr << "No se pudo abrir " << ruta("amistades.txt") << "\n"; return false; }
    string linea, c[2];
    while (leerLinea(in, linea)) {
        if (dividir(linea, c, 2) != 2) { lineasIgnoradas++; continue; }
        int a = red.buscarUsuario(c[0]);
        int b = red.buscarUsuario(c[1]);
        if (a < 0 || b < 0 || a == b) { lineasIgnoradas++; continue; }
        red.getMatriz()->establecer(a, b);
    }
    return true;
}

bool GestorArchivos::cargarSolicitudes(UdeABook &red)
{
    ifstream in(ruta("solicitudes.txt"));
    if (!in) { cerr << "No se pudo abrir " << ruta("solicitudes.txt") << "\n"; return false; }
    string linea, c[3];
    while (leerLinea(in, linea)) {
        try {
            Fecha f;
            if (dividir(linea, c, 3) != 3 || !parseFecha(c[2], f)) { lineasIgnoradas++; continue; }
            int receptor = red.buscarUsuario(c[0]);
            int emisor = red.buscarUsuario(c[1]);
            if (receptor < 0 || emisor < 0) { lineasIgnoradas++; continue; }
            red.getUsuario(receptor)->agregarSolicitud(new SolicitudAmistad(emisor, f));
        } catch (...) {
            lineasIgnoradas++;
        }
    }
    return true;
}

bool GestorArchivos::cargarAlbumes(UdeABook &red)
{
    ifstream in(ruta("albumes.txt"));
    if (!in) { cerr << "No se pudo abrir " << ruta("albumes.txt") << "\n"; return false; }
    string linea, c[4];
    while (leerLinea(in, linea)) {
        if (dividir(linea, c, 4) != 4 || c[3].size() != 1) { lineasIgnoradas++; continue; }
        int dueno = red.buscarUsuario(c[0]);
        if (dueno < 0) { lineasIgnoradas++; continue; }
        red.getUsuario(dueno)->agregarAlbum(new Album(c[1], c[2], c[3][0]));
    }
    return true;
}

bool GestorArchivos::cargarFotos(UdeABook &red)
{
    ifstream in(ruta("fotos.txt"));
    if (!in) { cerr << "No se pudo abrir " << ruta("fotos.txt") << "\n"; return false; }
    string linea, c[6];
    while (leerLinea(in, linea)) {
        try {
            Fecha f;
            if (dividir(linea, c, 6) != 6 || !parseFecha(c[3], f)) { lineasIgnoradas++; continue; }
            int dueno = red.buscarUsuario(c[0]);
            if (dueno < 0) { lineasIgnoradas++; continue; }

            // Un usuario tiene pocos albumes: busqueda lineal por nombre
            Usuario *u = red.getUsuario(dueno);
            Album *album = nullptr;
            for (int k = 0; k < u->getNumAlbumes(); k++) {
                Medidor::contar();
                if (u->getAlbum(k)->getNombre() == c[1]) { album = u->getAlbum(k); break; }
            }
            if (!album) { lineasIgnoradas++; continue; }

            Foto *foto = new Foto(c[2], f, stof(c[4]));
            // Likes: nombres separados por comas
            size_t pos = 0;
            while (pos < c[5].size()) {
                size_t coma = c[5].find(',', pos);
                if (coma == string::npos) coma = c[5].size();
                int quien = red.buscarUsuario(c[5].substr(pos, coma - pos));
                if (quien >= 0) foto->agregarLike(quien);
                pos = coma + 1;
                Medidor::contar();
            }
            album->agregarFoto(foto);
        } catch (...) {
            lineasIgnoradas++;
        }
    }
    return true;
}

bool GestorArchivos::cargarMensajes(UdeABook &red)
{
    ifstream in(ruta("mensajes.txt"));
    if (!in) { cerr << "No se pudo abrir " << ruta("mensajes.txt") << "\n"; return false; }
    string linea, c[5];
    while (leerLinea(in, linea)) {
        try {
            Fecha f;
            if (dividir(linea, c, 5) != 5 || !parseFecha(c[2], f)
                || !Mensaje::textoValido(c[4])) { lineasIgnoradas++; continue; }
            int emisor = red.buscarUsuario(c[0]);
            int receptor = red.buscarUsuario(c[1]);
            if (emisor < 0 || receptor < 0) { lineasIgnoradas++; continue; }
            Mensaje *m = new Mensaje(emisor, receptor, c[4], f);
            if (c[3] == "1") m->marcarLeido();
            red.getUsuario(emisor)->enviarMensaje(m);      // el emisor es el dueno
            red.getUsuario(receptor)->recibirMensaje(m);   // el receptor solo apunta
        } catch (...) {
            lineasIgnoradas++;
        }
    }
    return true;
}

bool GestorArchivos::guardar(const UdeABook &red)
{
    ofstream us(ruta("usuarios.txt")), am(ruta("amistades.txt")),
             so(ruta("solicitudes.txt")), al(ruta("albumes.txt")),
             fo(ruta("fotos.txt")), me(ruta("mensajes.txt"));
    if (!us || !am || !so || !al || !fo || !me) {
        cerr << "No se pudo escribir en la carpeta '" << carpeta << "'\n";
        return false;
    }
    int n = red.getNumUsuarios();
    ListaDinamica<int> amigos;
    for (int i = 0; i < n; i++) {
        Usuario *u = red.getUsuario(i);
        Medidor::contar();

        us << u->getNombreUsuario() << ';';
        escribirFecha(us, u->getFechaNacimiento(), false);
        us << ';' << u->getCorreo() << ';' << u->getRutaFoto() << ';' << u->getCiudad()
           << ';' << u->getPais() << ';';
        escribirFecha(us, u->getFechaInscripcion(), false);
        us << '\n';

        amigos.vaciar();
        red.getMatriz()->amigosDe(i, amigos);
        for (int k = 0; k < amigos.getTam(); k++) {
            Medidor::contar();
            if (amigos[k] > i)                       // cada par una sola vez
                am << u->getNombreUsuario() << ';'
                   << red.getUsuario(amigos[k])->getNombreUsuario() << '\n';
        }

        for (int k = 0; k < u->getNumSolicitudes(); k++) {
            Medidor::contar();
            SolicitudAmistad *s = u->getSolicitud(k);
            so << u->getNombreUsuario() << ';'
               << red.getUsuario(s->getIdEmisor())->getNombreUsuario() << ';';
            escribirFecha(so, s->getFecha(), true);
            so << '\n';
        }

        for (int k = 0; k < u->getNumAlbumes(); k++) {
            Album *a = u->getAlbum(k);
            al << u->getNombreUsuario() << ';' << a->getNombre() << ';' << a->getRuta()
               << ';' << a->getPrivacidad() << '\n';
            for (int p = 0; p < a->getNumFotos(); p++) {
                Medidor::contar();
                Foto *f = a->getFoto(p);
                fo << u->getNombreUsuario() << ';' << a->getNombre() << ';' << f->getRuta() << ';';
                escribirFecha(fo, f->getFecha(), false);
                fo << ';' << f->getTamano() << ';';
                for (int l = 0; l < f->contarLikes(); l++) {
                    Medidor::contar();
                    if (l > 0) fo << ',';
                    fo << red.getUsuario(f->getLike(l))->getNombreUsuario();
                }
                fo << '\n';
            }
        }

        for (int k = 0; k < u->getNumEnviados(); k++) {
            Medidor::contar();
            Mensaje *m = u->getEnviado(k);
            me << u->getNombreUsuario() << ';'
               << red.getUsuario(m->getIdReceptor())->getNombreUsuario() << ';';
            escribirFecha(me, m->getFecha(), true);
            me << ';' << (m->estaLeido() ? 1 : 0) << ';' << m->getTexto() << '\n';
        }
    }
    return true;
}
