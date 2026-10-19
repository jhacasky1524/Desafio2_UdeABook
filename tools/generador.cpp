// Generador de datos de PRUEBA para UdeABook (herramienta aparte, no es parte del core).
// Compilar:  g++ -std=c++17 tools/generador.cpp -o generador
// Ejecutar:  ./generador [numUsuarios=1000] [carpeta=datos]
//
// Genera usuarios en grupos de 40 con alta amistad dentro del grupo, de modo que
// existan usuarios con pocos, varios y muchos amigos en comun (los 3 niveles de
// prioridad de las fotos). Cada usuario tiene entre 2 y 4 albumes con 5 a 10 fotos,
// y hay mensajes entre amigos que cubren del 01/08/2026 al 12/10/2026 (10 semanas).
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std;

static mt19937 rng(2026);
static int rnd(int a, int b) { return uniform_int_distribution<int>(a, b)(rng); }
static bool prob(double p) { return uniform_real_distribution<double>(0.0, 1.0)(rng) < p; }

static string pad(int v, int w)
{
    string s = to_string(v);
    while (static_cast<int>(s.size()) < w) s = "0" + s;
    return s;
}
static string nombre(int i) { return "user" + pad(i, 4); }
static string fechaDia(int a, int m, int d) { return pad(a, 4) + "-" + pad(m, 2) + "-" + pad(d, 2); }
static string fechaHora(int a, int m, int d, int h, int mi)
{
    return fechaDia(a, m, d) + " " + pad(h, 2) + ":" + pad(mi, 2);
}

int main(int argc, char **argv)
{
    int n = argc > 1 ? stoi(argv[1]) : 1000;
    string dir = argc > 2 ? argv[2] : "datos";
    if (n < 2) { cerr << "Se necesitan al menos 2 usuarios\n"; return 1; }
    filesystem::create_directories(dir);

    const char *ciudades[] = {"Medellin", "Bogota", "Cali", "Barranquilla", "Cartagena", "Bucaramanga", "Pereira", "Manizales"};
    const char *frases[] = {"Hola, como vas?", "Nos vemos hoy; confirma por favor", "Que buena foto!", "Ya hiciste el desafio?",
                            "Feliz cumpleanos", "Vamos por un cafe", "Te debo una", "Mira lo que encontre", "Gracias por todo", "Hablamos luego"};
    const char *nombresAlbum[] = {"Cartagena2026", "Familia", "Amigos", "Viaje", "Universidad", "Mascotas"};
    const int G = 40;                                    // tamano de cada grupo

    // ---- grafo de amistad ----
    bool *ady = new bool[static_cast<long>(n) * n]();
    int *grado = new int[n]();
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (prob(i / G == j / G ? 0.70 : 0.012)) {
                ady[static_cast<long>(i) * n + j] = ady[static_cast<long>(j) * n + i] = true;
                grado[i]++; grado[j]++;
            }
    int **amigos = new int *[n];
    for (int i = 0; i < n; i++) {
        amigos[i] = new int[grado[i] > 0 ? grado[i] : 1];
        int k = 0;
        for (int j = 0; j < n; j++)
            if (ady[static_cast<long>(i) * n + j]) amigos[i][k++] = j;
    }

    ofstream us(dir + "/usuarios.txt"), am(dir + "/amistades.txt"), so(dir + "/solicitudes.txt"),
             al(dir + "/albumes.txt"), fo(dir + "/fotos.txt"), me(dir + "/mensajes.txt");
    if (!us || !am || !so || !al || !fo || !me) { cerr << "No se pudo escribir en " << dir << "\n"; return 1; }

    // ---- usuarios y amistades ----
    for (int i = 0; i < n; i++) {
        us << nombre(i) << ';' << fechaDia(rnd(1980, 2006), rnd(1, 12), rnd(1, 28)) << ';'
           << nombre(i) << "@udea.edu.co;/users/storage/" << nombre(i) << "/albums/profile.png;"
           << ciudades[rnd(0, 7)] << ";Colombia;" << fechaDia(rnd(2024, 2026), rnd(1, 9), rnd(1, 28)) << '\n';
        for (int j = i + 1; j < n; j++)
            if (ady[static_cast<long>(i) * n + j]) am << nombre(i) << ';' << nombre(j) << '\n';
    }

    // ---- solicitudes pendientes (nunca entre amigos ni en ambos sentidos) ----
    bool *sol = new bool[static_cast<long>(n) * n]();
    for (int r = 0; r < n; r++) {
        int cuantas = rnd(0, 3);
        for (int k = 0; k < cuantas; k++) {
            int e = rnd(0, n - 1);
            if (e == r || ady[static_cast<long>(r) * n + e] || sol[static_cast<long>(e) * n + r]
                || sol[static_cast<long>(r) * n + e]) continue;
            sol[static_cast<long>(e) * n + r] = true;
            so << nombre(r) << ';' << nombre(e) << ';'
               << fechaHora(2026, 10, rnd(1, 12), rnd(7, 22), rnd(0, 59)) << '\n';
        }
    }

    // ---- albumes y fotos ----
    fo << fixed << setprecision(2);
    for (int i = 0; i < n; i++) {
        int numAlbumes = rnd(2, 4);
        for (int k = 0; k < numAlbumes; k++) {
            string nom = nombresAlbum[(i + k) % 6];
            // los 2 primeros albumes son visibles para amigos; los demas pueden ser privados
            char priv = (k < 2) ? (prob(0.5) ? 'P' : 'A') : "PVA"[rnd(0, 2)];
            string base = "/users/storage/" + nombre(i) + "/albums/" + nom;
            al << nombre(i) << ';' << nom << ';' << base << ';' << priv << '\n';
            int numFotos = rnd(5, 10);
            for (int p = 1; p <= numFotos; p++) {
                fo << nombre(i) << ';' << nom << ';' << base << "/foto" << p << ".png;"
                   << fechaDia(rnd(2025, 2026), rnd(1, 9), rnd(1, 28)) << ';' << rnd(50, 900) / 100.0 << ';';
                int likes = rnd(0, 8), elegidos[8], cant = 0;
                for (int l = 0; l < likes; l++) {
                    int q = rnd(0, n - 1);
                    bool repetido = false;
                    for (int x = 0; x < cant; x++) if (elegidos[x] == q) repetido = true;
                    if (repetido) continue;
                    fo << (cant > 0 ? "," : "") << nombre(q);
                    elegidos[cant++] = q;
                }
                fo << '\n';
            }
        }
    }

    // ---- mensajes entre amigos, en orden cronologico (01/08 al 12/10 de 2026) ----
    int diasMes[] = {0, 0, 0, 0, 0, 0, 0, 0, 31, 30, 12};
    int porDia = n * 3 / 10 > 0 ? n * 3 / 10 : 1;
    for (int m = 8; m <= 10; m++)
        for (int d = 1; d <= diasMes[m]; d++)
            for (int k = 0; k < porDia; k++) {
                int a = rnd(0, n - 1);
                if (grado[a] == 0) continue;
                int b = amigos[a][rnd(0, grado[a] - 1)];
                bool antiguo = (m < 10 || d < 8);
                bool leido = antiguo ? prob(0.97) : prob(0.2);
                me << nombre(a) << ';' << nombre(b) << ';' << fechaHora(2026, m, d, rnd(7, 23), rnd(0, 59))
                   << ';' << (leido ? 1 : 0) << ';' << frases[rnd(0, 9)] << '\n';
            }

    cout << "Datos generados en '" << dir << "' para " << n << " usuarios.\n";
    return 0;
}
