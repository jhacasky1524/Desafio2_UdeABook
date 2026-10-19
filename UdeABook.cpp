#include "UdeABook.h"
#include <iostream>

using namespace std;

// ============================================================
//  Utilidades de consola
// ============================================================
static string leerTexto(const string &prompt)
{
    cout << prompt;
    string s;
    if (!getline(cin, s))
        throw FinEntrada();
    if (!s.empty() && s[s.size() - 1] == '\r')
        s.erase(s.size() - 1);
    return s;
}

// Devuelve el numero ingresado o -1 si no es un entero valido
static int leerNumero(const string &prompt)
{
    string s = leerTexto(prompt);
    try {
        size_t pos = 0;
        int v = stoi(s, &pos);
        return pos == s.size() ? v : -1;
    } catch (...) {
        return -1;
    }
}

static bool esLetra(const string &s, char minuscula)
{
    return s.size() == 1 && (s[0] == minuscula || s[0] == minuscula - 'a' + 'A');
}

// Lee "DD/MM/AAAA"
static bool leerFecha(const string &prompt, Fecha &f)
{
    string s = leerTexto(prompt);
    size_t a = s.find('/');
    size_t b = (a == string::npos) ? string::npos : s.find('/', a + 1);
    if (b == string::npos)
        return false;
    try {
        f = Fecha(static_cast<short>(stoi(s.substr(0, a))),
                  static_cast<short>(stoi(s.substr(a + 1, b - a - 1))),
                  static_cast<short>(stoi(s.substr(b + 1))));
    } catch (...) {
        return false;
    }
    return f.esValida();
}

// ============================================================
//  Datos de la plataforma
// ============================================================
UdeABook::UdeABook()
    : usuarios(nullptr), numUsuarios(0), capacidad(16), amistad(nullptr), actual(-1),
      generador(random_device{}()), bytesLocales(0)
{
    usuarios = new Usuario *[capacidad];
}

UdeABook::~UdeABook()
{
    for (int i = 0; i < numUsuarios; i++) {
        delete usuarios[i];
        Medidor::contar();
    }
    delete[] usuarios;
    delete amistad;
}

void UdeABook::crecer()
{
    int nuevaCap = capacidad * 2;
    Usuario **nuevo = new Usuario *[nuevaCap];
    for (int i = 0; i < numUsuarios; i++) {
        nuevo[i] = usuarios[i];
        Medidor::contar();
    }
    delete[] usuarios;
    usuarios = nuevo;
    capacidad = nuevaCap;
}

int UdeABook::agregarUsuario(Usuario *u)
{
    int a = 0, b = numUsuarios;
    while (a < b) {
        int mid = (a + b) / 2;
        Medidor::contar();
        if (usuarios[mid]->getNombreUsuario() < u->getNombreUsuario())
            a = mid + 1;
        else
            b = mid;
    }
    if (a < numUsuarios && usuarios[a]->getNombreUsuario() == u->getNombreUsuario())
        return -1;

    if (numUsuarios == capacidad)
        crecer();
    for (int k = numUsuarios; k > a; k--) {
        usuarios[k] = usuarios[k - 1];
        Medidor::contar();
    }
    usuarios[a] = u;
    numUsuarios++;
    return a;
}

void UdeABook::crearMatriz()
{
    for (int i = 0; i < numUsuarios; i++) {
        usuarios[i]->setIndice(i);
        Medidor::contar();
    }
    delete amistad;
    amistad = new MatrizAdyacencia(numUsuarios);
}

int UdeABook::buscarUsuario(const string &nombre) const
{
    int a = 0, b = numUsuarios;
    while (a < b) {
        int mid = (a + b) / 2;
        Medidor::contar();
        const string &actualNombre = usuarios[mid]->getNombreUsuario();
        if (actualNombre == nombre)
            return mid;
        if (actualNombre < nombre)
            a = mid + 1;
        else
            b = mid;
    }
    return -1;
}

Usuario *UdeABook::getUsuario(int i) const { return usuarios[i]; }
int UdeABook::getNumUsuarios() const { return numUsuarios; }
MatrizAdyacencia *UdeABook::getMatriz() const { return amistad; }
int UdeABook::getActual() const { return actual; }
void UdeABook::setActual(int i) { actual = i; }

unsigned long UdeABook::memoria() const
{
    unsigned long total = sizeof(*this)
        + static_cast<unsigned long>(capacidad) * sizeof(Usuario *)
        + amigosSesion.bytesDinamicos() + pesosSesion.bytesDinamicos()
        + grupoFotos.bytesDinamicos() + Medidor::bytesCadena(externos);
    if (amistad)
        total += amistad->memoria();
    for (int i = 0; i < numUsuarios; i++) {
        total += usuarios[i]->memoria();
        Medidor::contar();
    }
    return total;
}

// ============================================================
//  Medicion de recursos (funcionalidad VII)
// ============================================================
void UdeABook::iniciarMedicion()
{
    Medidor::reiniciar();
    bytesLocales = 0;
    externos = "";
}

void UdeABook::mostrarMedicion()
{
    unsigned long it = Medidor::getIteraciones();    // antes de calcular la memoria
    unsigned long mem = memoria() + bytesLocales;
    Medidor::mostrar(mem, it, externos);
}

// ============================================================
//  II. Ingreso y fotos de perfil de amigos
// ============================================================
void UdeABook::prepararSesion()
{
    amigosSesion.vaciar();
    pesosSesion.vaciar();
    grupoFotos.vaciar();
    amistad->amigosDe(actual, amigosSesion);
    for (int i = 0; i < amigosSesion.getTam(); i++) {
        Medidor::contar();
        int comunes = amistad->contarComunes(actual, amigosSesion[i]);
        // Peso entero (el doble del enunciado): 1 -> 2, 2.5 -> 5, 3.5 -> 7
        pesosSesion.agregar(comunes <= 10 ? 2 : (comunes <= 20 ? 5 : 7));
    }
    bytesLocales += 2 * sizeof(int);
}

// Elige hasta 3 amigos distintos con probabilidad proporcional a su peso.
// Primero se sortea entre quienes NO estaban en el grupo anterior; solo si no
// alcanzan se completa con los del grupo anterior.
void UdeABook::sortearGrupo()
{
    int d = amigosSesion.getTam();
    int objetivo = d < 3 ? d : 3;
    unsigned char *estado = new unsigned char[d > 0 ? d : 1]();   // 0 libre, 1 grupo anterior, 2 elegido
    bytesLocales += static_cast<unsigned long>(d) + 6 * sizeof(int) + sizeof(unsigned char *);

    for (int k = 0; k < grupoFotos.getTam(); k++) {
        estado[grupoFotos[k]] = 1;
        Medidor::contar();
    }
    grupoFotos.vaciar();

    for (int pasada = 0; pasada < 2; pasada++) {
        unsigned char buscado = (pasada == 0) ? 0 : 1;
        while (grupoFotos.getTam() < objetivo) {
            int total = 0;
            for (int i = 0; i < d; i++) {
                Medidor::contar();
                if (estado[i] == buscado)
                    total += pesosSesion[i];
            }
            if (total == 0)
                break;
            int r = uniform_int_distribution<int>(0, total - 1)(generador);
            for (int i = 0; i < d; i++) {
                Medidor::contar();
                if (estado[i] != buscado)
                    continue;
                if (r < pesosSesion[i]) {
                    estado[i] = 2;
                    grupoFotos.agregar(i);
                    break;
                }
                r -= pesosSesion[i];
            }
        }
    }
    delete[] estado;
}

bool UdeABook::iniciarSesion(const string &nombre)
{
    iniciarMedicion();
    bytesLocales += sizeof(string) + Medidor::bytesCadena(nombre) + sizeof(int);
    int idx = buscarUsuario(nombre);
    if (idx < 0) {
        cout << "Usuario no encontrado.\n";
        mostrarMedicion();
        return false;
    }
    actual = idx;
    prepararSesion();
    sortearGrupo();
    externos = "<random> std::mt19937 (generador de numeros aleatorios)";
    mostrarMedicion();
    return true;
}

void UdeABook::cerrarSesion()
{
    actual = -1;
    amigosSesion.vaciar();
    pesosSesion.vaciar();
    grupoFotos.vaciar();
}

void UdeABook::refrescarFotosAmigos()
{
    iniciarMedicion();
    sortearGrupo();
    externos = "<random> std::mt19937 (generador de numeros aleatorios)";
    mostrarMedicion();
}

void UdeABook::menuPrincipal()
{
    while (actual >= 0) {
        cout << "\n==============================================\n";
        if (grupoFotos.getTam() == 0)
            cout << "(Aun no tiene amigos cuyas fotos mostrar)\n";
        for (int k = 0; k < grupoFotos.getTam(); k++)
            cout << "Foto perfil de amigo " << k + 1 << ": "
                 << usuarios[amigosSesion[grupoFotos[k]]]->getRutaFoto() << "\n";
        cout << "\nBienvenido a UdeaBook " << usuarios[actual]->getNombreUsuario() << "\n\n"
             << "Menu de opciones:\n"
             << "  1.- Administrar amistades\n"
             << "  2.- Visitar perfil de un amigo\n"
             << "  3.- Ver sugerencias de amistad\n"
             << "  4.- Administrar mensajes\n\n"
             << ".- Ingrese 'N' o 'n' para alternar el grupo de fotos de perfil de amigos.\n"
             << ".- Ingrese 'S' o 's' para cerrar su sesion.\n";
        string op = leerTexto("Su opcion: ");
        if (op == "1") administrarAmistades();
        else if (op == "2") visitarPerfil();
        else if (op == "3") sugerencias();
        else if (op == "4") administrarMensajes();
        else if (esLetra(op, 'n')) refrescarFotosAmigos();
        else if (esLetra(op, 's')) cerrarSesion();
        else cout << "Opcion no valida.\n";
    }
}

// ============================================================
//  III. Administrar amistades
// ============================================================
bool UdeABook::esAmigoDelActual(int idx) const
{
    return idx >= 0 && idx != actual && amistad->sonAmigos(actual, idx);
}

bool UdeABook::validarSolicitud(int destino, string &motivo) const
{
    if (destino == actual) {
        motivo = "No puede enviarse una solicitud a si mismo.";
        return false;
    }
    if (amistad->sonAmigos(actual, destino)) {
        motivo = "Ya son amigos.";
        return false;
    }
    if (usuarios[destino]->tieneSolicitudDe(actual)) {
        motivo = "Ya le envio una solicitud pendiente a ese usuario.";
        return false;
    }
    if (usuarios[actual]->tieneSolicitudDe(destino)) {
        motivo = "Ese usuario ya le envio una solicitud: aceptela en 'Aceptar solicitudes recibidas'.";
        return false;
    }
    return true;
}

void UdeABook::crearSolicitud(int destino)
{
    usuarios[destino]->agregarSolicitud(new SolicitudAmistad(actual, Fecha::ahora()));
}

void UdeABook::administrarAmistades()
{
    while (true) {
        cout << "\n--- Administrar amistades ---\n"
             << "1.- Enviar solicitud de amistad\n"
             << "2.- Aceptar solicitudes recibidas\n"
             << "0.- Volver\n";
        int op = leerNumero("Su opcion: ");
        if (op == 1) enviarSolicitud();
        else if (op == 2) aceptarSolicitudes();
        else if (op == 0) return;
        else cout << "Opcion no valida.\n";
    }
}

void UdeABook::enviarSolicitud()
{
    iniciarMedicion();
    string nombre = leerTexto("Nombre de usuario al que desea enviar la solicitud: ");
    bytesLocales += sizeof(string) * 2 + Medidor::bytesCadena(nombre) + 2 * sizeof(int);
    int destino = buscarUsuario(nombre);
    string motivo;
    if (destino < 0) {
        cout << "No existe un usuario con ese nombre.\n";
    } else if (!validarSolicitud(destino, motivo)) {
        cout << motivo << "\n";
    } else {
        crearSolicitud(destino);
        externos = "<chrono> std::chrono::system_clock (fecha y hora del sistema)";
        cout << "Solicitud enviada a " << nombre << ".\n";
    }
    mostrarMedicion();
}

void UdeABook::aceptarSolicitudes()
{
    iniciarMedicion();
    Usuario *yo = usuarios[actual];
    bool huboCambios = false;
    bytesLocales += sizeof(Usuario *) + sizeof(bool) + 4 * sizeof(int) + sizeof(string);

    while (true) {
        int n = yo->getNumSolicitudes();
        if (n == 0) {
            cout << "No tiene solicitudes de amistad pendientes.\n";
            break;
        }
        cout << "\nSolicitudes recibidas:\n";
        for (int i = 0; i < n; i++) {
            Medidor::contar();
            SolicitudAmistad *s = yo->getSolicitud(i);
            cout << "  " << i + 1 << ") " << usuarios[s->getIdEmisor()]->getNombreUsuario()
                 << "  (" << s->getFecha() << ")\n";
        }
        int op = leerNumero("Numero de la solicitud a responder (0 para volver): ");
        if (op < 1 || op > n)
            break;
        string r = leerTexto("A) Aceptar   R) Rechazar: ");
        int emisor = yo->getSolicitud(op - 1)->getIdEmisor();
        if (esLetra(r, 'a')) {
            amistad->establecer(actual, emisor);
            yo->eliminarSolicitud(op - 1);
            huboCambios = true;
            cout << "Ahora es amigo de " << usuarios[emisor]->getNombreUsuario() << ".\n";
        } else if (esLetra(r, 'r')) {
            yo->eliminarSolicitud(op - 1);
            cout << "Solicitud rechazada.\n";
        } else {
            cout << "Opcion no valida.\n";
        }
    }
    if (huboCambios) {            // hay amigos nuevos: se recalculan las fotos de perfil
        prepararSesion();
        sortearGrupo();
        externos = "<random> std::mt19937 (generador de numeros aleatorios)";
    }
    mostrarMedicion();
}

// ============================================================
//  IV. Visitar perfil de un amigo
// ============================================================
void UdeABook::carrusel(Album *album)
{
    int n = album->getNumFotos();
    int pos = 0;
    bytesLocales += 3 * sizeof(int) + sizeof(Foto *) + sizeof(string) + sizeof(bool);
    while (true) {
        Medidor::contar();
        Foto *f = album->getFoto(pos);
        cout << "\nFoto " << pos + 1 << " de " << n << ": ";
        f->mostrar();
        if (f->tieneLikeDe(actual))
            cout << "   (usted le dio like)\n";
        string op = leerTexto("[S] Salir del album   [N] Siguiente   [P] Previa   [L] Dar/quitar like: ");
        if (esLetra(op, 's')) return;
        else if (esLetra(op, 'n')) pos = album->indiceSiguiente(pos);
        else if (esLetra(op, 'p')) pos = album->indiceAnterior(pos);
        else if (esLetra(op, 'l'))
            cout << (f->alternarLike(actual) ? "Le dio like a la foto.\n" : "Quito su like.\n");
        else cout << "Opcion no valida.\n";
    }
}

void UdeABook::visitarPerfil()
{
    iniciarMedicion();
    string nombre = leerTexto("Nombre de usuario del amigo: ");
    bytesLocales += sizeof(string) + Medidor::bytesCadena(nombre) + 3 * sizeof(int);
    int idx = buscarUsuario(nombre);
    if (!esAmigoDelActual(idx)) {
        cout << "Solo puede visitar el perfil de usuarios que son sus amigos.\n";
    } else {
        Usuario *amigo = usuarios[idx];
        ListaDinamica<int> visibles;
        for (int i = 0; i < amigo->getNumAlbumes(); i++) {
            Medidor::contar();
            if (amigo->getAlbum(i)->visiblePara(false, true))   // no es el dueno, si es amigo
                visibles.agregar(i);
        }
        bytesLocales += visibles.memoria() + sizeof(Usuario *);

        while (true) {
            if (visibles.getTam() == 0) {
                cout << "Este amigo no tiene albumes visibles para usted.\n";
                break;
            }
            cout << "\nAlbumes de " << amigo->getNombreUsuario() << ":\n";
            for (int k = 0; k < visibles.getTam(); k++) {
                Medidor::contar();
                Album *a = amigo->getAlbum(visibles[k]);
                cout << "  " << k + 1 << ") " << a->getNombre() << "  [" << a->getNumFotos() << " fotos, "
                     << (a->getPrivacidad() == 'P' ? "publico" : "amigos") << "]\n";
            }
            int op = leerNumero("Elija un album (0 para volver): ");
            if (op < 1 || op > visibles.getTam())
                break;
            Album *elegido = amigo->getAlbum(visibles[op - 1]);
            if (elegido->getNumFotos() == 0)
                cout << "El album no tiene fotos.\n";
            else
                carrusel(elegido);
        }
    }
    mostrarMedicion();
}

// ============================================================
//  V. Sugerencias de amistad
// ============================================================
void UdeABook::sugerencias()
{
    iniciarMedicion();
    int k = leerNumero("Cuantas sugerencias desea ver (1 a 20): ");
    bytesLocales += 6 * sizeof(int);
    if (k < 1 || k > 20) {
        cout << "El valor debe estar entre 1 y 20.\n";
        mostrarMedicion();
        return;
    }

    // cuenta[g] = cuantos amigos directos del usuario tambien son amigos de g
    int n = numUsuarios;
    int *cuenta = new int[n]();
    bytesLocales += static_cast<unsigned long>(n) * sizeof(int) + sizeof(int *);
    ListaDinamica<int> deAmigo;
    for (int i = 0; i < amigosSesion.getTam(); i++) {
        Medidor::contar();
        deAmigo.vaciar();
        amistad->amigosDe(amigosSesion[i], deAmigo);
        for (int j = 0; j < deAmigo.getTam(); j++) {
            Medidor::contar();
            int g = deAmigo[j];
            if (g != actual && !amistad->sonAmigos(actual, g))
                cuenta[g]++;
        }
    }

    // Los k mejores: se extrae el maximo k veces (k <= 20, no vale la pena ordenar todo).
    // Se omiten quienes ya tienen una solicitud pendiente con el usuario.
    ListaDinamica<int> sugeridos, comunes;
    while (sugeridos.getTam() < k) {
        int mejor = -1, mejorCuenta = 0;
        for (int g = 0; g < n; g++) {
            Medidor::contar();
            if (cuenta[g] > mejorCuenta) {
                mejorCuenta = cuenta[g];
                mejor = g;
            }
        }
        if (mejor < 0)
            break;
        cuenta[mejor] = 0;
        if (usuarios[mejor]->tieneSolicitudDe(actual) || usuarios[actual]->tieneSolicitudDe(mejor))
            continue;
        sugeridos.agregar(mejor);
        comunes.agregar(mejorCuenta);
    }
    delete[] cuenta;
    bytesLocales += deAmigo.memoria() + sugeridos.memoria() + comunes.memoria();

    if (sugeridos.getTam() == 0) {
        cout << "No hay sugerencias de amistad disponibles.\n";
    } else {
        cout << "\nSugerencias de amistad (ordenadas por amigos en comun):\n";
        for (int i = 0; i < sugeridos.getTam(); i++) {
            Medidor::contar();
            cout << "  " << i + 1 << ") " << usuarios[sugeridos[i]]->getNombreUsuario()
                 << "  -  " << comunes[i] << " amigos en comun\n";
        }
        int op = leerNumero("Numero del usuario al que desea enviar solicitud (0 para no enviar): ");
        if (op >= 1 && op <= sugeridos.getTam()) {
            crearSolicitud(sugeridos[op - 1]);
            externos = "<chrono> std::chrono::system_clock (fecha y hora del sistema)";
            cout << "Solicitud enviada a " << usuarios[sugeridos[op - 1]]->getNombreUsuario() << ".\n";
        }
    }
    mostrarMedicion();
}

// ============================================================
//  VI. Administrar mensajes
// ============================================================
void UdeABook::administrarMensajes()
{
    while (true) {
        cout << "\n--- Administrar mensajes ---\n"
             << "1.- Enviar mensaje\n"
             << "2.- Leer nuevos mensajes\n"
             << "3.- Buscar mensajes por rango de fecha\n"
             << "0.- Volver\n";
        int op = leerNumero("Su opcion: ");
        if (op == 1) enviarMensaje();
        else if (op == 2) leerNuevos();
        else if (op == 3) buscarMensajesPorRango();
        else if (op == 0) return;
        else cout << "Opcion no valida.\n";
    }
}

void UdeABook::enviarMensaje()
{
    iniciarMedicion();
    string nombre = leerTexto("Nombre del amigo destinatario: ");
    bytesLocales += sizeof(string) * 2 + Medidor::bytesCadena(nombre) + 2 * sizeof(int) + sizeof(Mensaje *);
    int idx = buscarUsuario(nombre);
    if (!esAmigoDelActual(idx)) {
        cout << "Solo puede enviar mensajes a sus amigos.\n";
    } else {
        string texto = leerTexto("Mensaje (maximo 512 caracteres): ");
        bytesLocales += Medidor::bytesCadena(texto);
        if (texto.size() > static_cast<size_t>(Mensaje::MAX_TEXTO)) {
            cout << "El mensaje supera los 512 caracteres.\n";
        } else if (!Mensaje::textoValido(texto)) {
            cout << "El mensaje no puede estar vacio.\n";
        } else {
            Mensaje *m = new Mensaje(actual, idx, texto, Fecha::ahora());
            usuarios[actual]->enviarMensaje(m);      // el emisor es el dueno
            usuarios[idx]->recibirMensaje(m);        // el receptor solo apunta
            externos = "<chrono> std::chrono::system_clock (fecha y hora del sistema)";
            cout << "Mensaje enviado a " << nombre << ".\n";
        }
    }
    mostrarMedicion();
}

void UdeABook::leerNuevos()
{
    iniciarMedicion();
    ListaDinamica<Mensaje *> nuevos;
    int cuantos = usuarios[actual]->leerNuevos(nuevos);
    if (cuantos == 0) {
        cout << "No tiene mensajes nuevos.\n";
    } else {
        cout << "\nTiene " << cuantos << " mensajes nuevos:\n";
        for (int i = 0; i < nuevos.getTam(); i++) {
            Medidor::contar();
            cout << "  De " << usuarios[nuevos[i]->getIdEmisor()]->getNombreUsuario() << " "
                 << *nuevos[i] << "\n";
        }
    }
    bytesLocales += nuevos.memoria() + 2 * sizeof(int);
    mostrarMedicion();
}

void UdeABook::buscarMensajesPorRango()
{
    iniciarMedicion();
    Fecha inicio, fin;
    bytesLocales += 2 * sizeof(Fecha) + sizeof(int);
    if (!leerFecha("Fecha de inicio (DD/MM/AAAA): ", inicio)
        || !leerFecha("Fecha de fin    (DD/MM/AAAA): ", fin)) {
        cout << "Fecha invalida. Use el formato DD/MM/AAAA.\n";
    } else if (fin < inicio) {
        cout << "La fecha de fin no puede ser anterior a la de inicio.\n";
    } else {
        ListaDinamica<Mensaje *> encontrados;
        int cuantos = usuarios[actual]->buscarPorRango(inicio, fin, encontrados);
        if (cuantos == 0) {
            cout << "No hay mensajes recibidos en ese rango.\n";
        } else {
            cout << "\nMensajes recibidos entre " << inicio.getDia() << "/" << inicio.getMes() << "/"
                 << inicio.getAnio() << " y " << fin.getDia() << "/" << fin.getMes() << "/"
                 << fin.getAnio() << " (ambas inclusive): " << cuantos << "\n";
            for (int i = 0; i < encontrados.getTam(); i++) {
                Medidor::contar();
                cout << "  De " << usuarios[encontrados[i]->getIdEmisor()]->getNombreUsuario() << " "
                     << *encontrados[i] << "\n";
            }
        }
        bytesLocales += encontrados.memoria();
    }
    mostrarMedicion();
}
