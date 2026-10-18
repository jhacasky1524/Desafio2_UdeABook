#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include "UdeABook.h"

// Carga los datos del almacenamiento permanente a memoria al iniciar y los
// vuelve a escribir al guardar. Los archivos NO se usan para procesar:
// toda la logica trabaja sobre las estructuras en memoria.
//
// Formato (texto, campos separados por ';', un registro por linea):
//   usuarios.txt    nombre;AAAA-MM-DD;correo;rutaFotoPerfil;ciudad;pais;AAAA-MM-DD
//   amistades.txt   usuarioA;usuarioB
//   solicitudes.txt receptor;emisor;AAAA-MM-DD HH:MM
//   albumes.txt     dueno;nombreAlbum;rutaAlbum;privacidad(P|V|A)
//   fotos.txt       dueno;album;rutaArchivo;AAAA-MM-DD;tamano;usuarioLike1,usuarioLike2,...
//   mensajes.txt    emisor;receptor;AAAA-MM-DD HH:MM;leido(0|1);texto
class GestorArchivos
{
private:
    std::string carpeta;
    int lineasIgnoradas;

    std::string ruta(const char *archivo) const;

    bool cargarUsuarios(UdeABook &red);
    bool cargarAmistades(UdeABook &red);
    bool cargarSolicitudes(UdeABook &red);
    bool cargarAlbumes(UdeABook &red);
    bool cargarFotos(UdeABook &red);
    bool cargarMensajes(UdeABook &red);

public:
    GestorArchivos(const std::string &carpeta = "datos");

    bool cargar(UdeABook &red);
    bool guardar(const UdeABook &red);
    int getLineasIgnoradas() const;
};

#endif // GESTORARCHIVOS_H
