2. [Requisitos Subject](#1.0)
1. [Documentación directivas necesarias - Nginx](#2.0)
	- [listen](#2.1)
	- [server_name](#2.2)
	- [error_page](#2.3)
	- [client_max_body_size](#2.4)
	- [return](#2.5)
	- [proxy_pass](#2.6)
	- [alias](#2.7)
	- [root](#2.8)
	- [autoindex](#2.9)
	- [index](#2.10)
	- [fastcgi_pass](#2.11)
3. [Directivas Propias](#3.0)
4. [Useful Links](#4.0)
5. [Tokens table](#5.0)


## <b id="1.0">REQUISITOS SUBJECT</b>

In the configuration file, you should be able to:

1. Choose the port and host of each ’server’. --> directiva `listen`.
2. Set up the server_names or not. --> directiva `server_name`, tener en cuenta que puede ser especificada o no.
3. The first server for a host:port will be the default for this host:port (that means it will answer to all the requests that don’t belong to an other server). --> establecer por defecto el primero como servidor por defecto, si quisieramos añadir la opción de poder especificar el servidor por defecto --> directiva `listen` parámetro despues de host:port.
4. Set up default error pages --> directiva `error_page` para setear pagina .html por defecto para error/es especificos. Por defecto habrá que setear las paginas predeterminadas de nuestro servidor. Deberiamos hacer una por cada codigo de error ???
5. Limit client size body. --> directiva `client_max_body_size`, Debemos establecer un valor por defecto pero permitir cambiarlo a algo > 0 siempre. En Nginx si se establece a 0 se desabilita que haya un max size pero nosotros tenemos que establerlo si o si por el subject.
6. Setup routes with one or multiple of the following rules/configuration (routes wont be using regexp):
	- Define a list of accepted HTTP methods for the route. --> directiva `limit_except`, pero en nignx se utiliza usando llaves por que acepta bastantes mas configuraciones de las que necesitamos. Lo suyo seria crear una directiva propia que acepte varios parámetros que sean tipos de métodos HTTP (tener un diccionario con los métodos que gestionemos).
	- Define a HTTP redirection. --> Pensabamos que era la directiva `proxy_pass`, pero es la directiva `return`. `proxy_pass` NO hace una redirección HTTP en sentido tradicional como lo hace `return`, sino que reenvía las solicitudes a otro servidor backend, actuando de intermediario entre cliente y servidor destino (lo que es un proxy vaya)
	- Define a directory or a file from where the file should be searched (for example, if url `/kapouet` is rooted to `/tmp/www`, url `/kapouet/pouic/toto/pouet` is `/tmp/www/pouic/toto/pouet`). --> directiva `alias`, aunque también debemos hacer directiva `root`.
	- Turn on / off directory listing --> directiva `autoindex`.
	- Set a default file to answer  if the request is a directory. --> directiva `index`.
	- Execute CGI based on certain file extension (for example .php). --> directiva `fastcgi_pass` pero cambiando bastantes cosas. Ver más adelante.
	- Make it work with POST and GET methods.
	- Make the route able to accept uploaded files and configure where they should be saved. --> crear directiva propia, no encuentro una en nginx que a nivel de location parecida.

## <b id="2.0">DOCUMENTACION DIRECTIVAS NECESARIAS - NGINX</b>
### <b id="2.1">listen</b>
Establece la dirección y el puerto para IP, o la ruta de un socket de dominio UNIX en el cual el servidor aceptará solicitudes. Se puede especificar tanto la dirección como el puerto, o solo la dirección o solo el puerto. Una dirección también puede ser un nombre de host (ej. localhost).

```bash
Syntax:	listen address[:port] [default_server] [ssl] [http2 | quic] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];

listen port [default_server] [ssl] [http2 | quic] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];

listen unix:path [default_server] [ssl] [http2 | quic] [proxy_protocol] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];

Default:	listen *:80 | *:8000;
Context:	server
```

Ejemplos de cosas validas:
```bash
listen 127.0.0.1:8000;
listen 127.0.0.1;
listen 8000;
listen *:8000;
listen localhost:8000;
```

### <b id="2.2">server_name</b>

EN EL SUBJECT PONE QUE SE PUEDE ESTABLECER O NO DESDE EL ARCHIVO DE CONFIGURACION

Basicamente lo que hace esta directiva es decirle a Nginx para qué dominios o subdominios debe responder un bloque de configuración de `server`, permitiendo que un solo servidor Nginx maneje múltiples sitios o aplicaciones basados en diferentes nombres de dominio.

```bash
Syntax:		server_name name ...;
Default:	server_name "";
Context:	server
```
Cuando un cliente realiza una solicitud HTTP, incluye un encabezado `Host` que indica el nombre del dominio solicitado. Nginx utiliza la directiva `server_name` para comparar ese valor con los nombres configurados y decidir qué configuración de servidor usar para esa solicitud.

Ejemplo de uso:
```bash
server {
    listen 80;
    server_name ejemplo.com www.ejemplo.com;

    location / {
        root /var/www/ejemplo;
        index index.html;
    }
}
```

En este caso, Nginx manejará solicitudes para `ejemplo.com` y `www.ejemplo.com`. Si un usuario ingresa cualquiera de estos dominios en su navegador, Nginx servirá contenido desde el directorio `/var/www/ejemplo`.

### <b id="2.3">error_page</b>

La directiva `error_page` se utiliza para definir páginas personalizadas o redirecciones que se mostrarán cuando ocurra un error en el servidor, como un 404 (Página no encontrada) o un 500 (Error interno del servidor). Con esta directiva, puedes especificar qué página mostrar o a dónde redirigir al usuario cuando ocurren ciertos códigos de error HTTP.

En nuestro caso solo tenemos que coger los codigos de errores y la pagina personalizada que se quiere mostrar. Creo que no es necesario cambiar el codigo de error.

```bash
Syntax:		error_page code ... [=[response]] uri;
Default:	—
Context:	http, server, location, if in location
```

Por defecto en la documentación pone que no se establece nada, pero nosotros debemos establecer nuestras paginas de error como predeterminadas por que esta directiva puede o no existir.

### <b id="2.4">client_max_body_size</b>

Establece el tamaño máximo permitido del cuerpo de la solicitud del cliente. Si el tamaño de una solicitud excede el valor configurado, se devuelve el error 413 (Entidad de Solicitud Demasiado Grande) al cliente. Ten en cuenta que los navegadores no pueden mostrar correctamente este error. (?) 

```bash
Syntax:		client_max_body_size size;
Default:	client_max_body_size 1m;
Context:	http, server, location
```

Configurar el tamaño en 0 desactiva la verificación del tamaño del cuerpo de la solicitud del cliente, pero nosotros no tenemos que implementar esto sino lanzar error si la solicitud es 0 o menor que 0. Haría falta ver como se pasan los valores bytes, mb, gb, etc.

### <b id="2.5">return</b>

La directiva `return` se utiliza para enviar una respuesta de vuelta al cliente con un código de estado HTTP y, opcionalmente, un mensaje o una URL. Es una forma simple y eficaz de realizar redirecciones o devolver respuestas HTTP específicas.

```bash
Syntax:		return code [text];
			return code URL;
			return URL;
Default:	—
Context:	server, location, if
```

Hay 4 posibles casos al usar `return`:

1. Devolver un codigo de estado HTTP con un mensaje personalizado. `return 403 "Access Forbidden";`
2. Redirigir a una URL diferente. `return <error_code> <URL>;`. No se puede especificar una redireccion sin un código de error antes (gestionar). Razón: Para redirigir a otra URL, necesitas especificar un código de estado de redirección para que Nginx pueda instruir al cliente (navegador) para que realice una nueva solicitud a la URL proporcionada. Sin un código de estado, Nginx no sabría cómo manejar la redirección.
3. Devolver una respuesta HTTP simple. `return 403;` El mensaje devuelto será el de por defecto "403 forbidden".
4. Se puede especificar un mensaje sin código de error, se asume que el código de error en este caso es (200:OK). `return "Hola Mundo";`



### <b id="2.6">proxy_pass</b>

CREO QUE NO HAY QUE IMPLEMENTARLA, pero la dejo por si acaso

Establece el protocolo y la dirección de un servidor proxy y una URI opcional a la que se debe mapear una ubicación. Como protocolo, se puede especificar "http" o "https". La dirección puede ser un nombre de dominio o una dirección IP, y un puerto opcional.

```bash
Syntax:		proxy_pass URL;
Default:	—
Context:	location, if in location, limit_except
```

Ejemplo de uso:
```bash
proxy_pass http://localhost:8000/uri/;
```

docs: https://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_pass

### <b id="2.7">alias</b>

La directiva `alias` se usa para redirigir una URL a una ruta específica del sistema de archivos. Sustituye completamente la ruta especificada en el bloque `location` por la ruta establecida en el `alias`.

alias solo existe en Nginx en el bloque location, creo que nosotros tb deberiamos hacerlo así.

```bash
Syntax:		alias path;
Default:	—
Context:	location
```

Un ejemplo:

```bash
server {
    listen 80;
    server_name ejemplo.com;

    location /kapouet/ {
        alias /tmp/www/;
    }
}
```

- `alias /tmp/www/`: Especifica que la ruta `/kapouet` está mapeada a `/tmp/www/` en el sistema de archivos. Entonces, cualquier solicitud posterior a `/kapouet` se interpretará como un archivo o directorio dentro de `/tmp/www/`.
- Por ejemplo: La solicitud a `/kapouet/pouic/toto/pouet` se traducirá a `/tmp/www/pouic/toto/pouet` en el sistema de archivos.

### <b id="2.8">root</b>

La directiva `root` en Nginx se utiliza para especificar el directorio raíz del sistema de archivos desde el cual Nginx servirá los archivos para una ubicación específica. Es decir, define el directorio base donde Nginx buscará los archivos solicitados.

```bash
Syntax:		root path;
Default:	root html;
Context:	http, server, location, if in location
```

La directiva root en cambio puede existir en los bloques `server` y `location`.

Un ejemplo:
Supongamos que se tiene la siguiente estructura de directorios en tu servidor:

```css
/var/www/html/
    ├── index.html
    ├── about.html
    └── images/
        └── logo.png
```

Y deseas servir archivos desde el directorio `/var/www/html` cuando un cliente accede a tu servidor web.

Configuración de nginx:

```bash
server {
    listen 80;
    server_name ejemplo.com;

    root /var/www/html; # Define el directorio raíz para esta configuración

    location / {
        index index.html; # Archivo predeterminado para directorios
    }

    location /images/ {
        # Aquí se puede definir otra configuración específica para la ubicación /images/
    }
}
```

Desglose del ejemplo:
- `root /var/www/html;`: Establece `/var/www/html` como el directorio raíz para todas las solicitudes que coincidan con el bloque server y sus ubicaciones internas.

- `location / { ... }`: Cuando el cliente solicita la URL `http://ejemplo.com/`, Nginx buscará el archivo `index.html` en el directorio `/var/www/html` (es decir, `/var/www/html/index.html`).

- `location /images/ { ... }`: Cuando el cliente solicita la URL `http://ejemplo.com/images/logo.png`, Nginx buscará el archivo `logo.png` en el directorio `/var/www/html/images` (es decir, `/var/www/html/images/logo.png`).

### <b id="2.9">autoindex</b>

Si se hace una petición que termine en `/` y no se encuentra ningún archivo index, cuando autoindex está activado muestra una lista con los archivos / directorios disponibles. En caso de estar desactivado se muestra un 403 forbidden.

```bash
Syntax:		autoindex on | off;
Default:	autoindex off;
Context:	http, server, location
```

### <b id="2.10">index</b>

Define archivo/s que serán utilizados como un index cuando la petición que se realiza termina en `/`. En Nginx puede contener variables. Se pueden poner varios archivos index, se buscarán en el orden establecido. Cuando no se encuentra un archivo se mostrará 404 not found o 403 forbidden según configuración.

```bash
Syntax:		index file ...;
Default:	index index.html;
Context:	http, server, location
```

### <b id="2.11">fastcgi_pass</b>




## <b id="3.0">DIRECTIVAS PROPIAS</b>

Ir documentando aquí las directivas que hagamos nuestras para luego hacer documentación.


## <b id="4.0">USEFUL LINKS</b>

- [Indice de directivas Nginx](https://nginx.org/en/docs/dirindex.html)
- [Como Nginx procesa una request](https://nginx.org/en/docs/http/request_processing.html)


## <b id="5.0">TOKENS TABLE</b>
|Tokens|Server|Location|Added 2 both class|Default|Implemented|
|-|-|-|-|-|-|
|listen|✔||✔ (port)|[NP] -1|[X]|
|server_name|✔||✔|[P] "Server X"|[X]|
|error_page|✔|✔|✔|[P] <40x and 50x> -> pages/error|[ ]|
|client_max_body_size|✔|✔|✔|[P] 100b|[X]|
|return|✔|✔|✔|[NP] {-1, ""}|[ ]|
|alias||✔|✔|[P] ""|[X]|
|root|✔|✔|✔|[P] "pages/"|[X]|
|index|✔|✔|✔|[NP] <empty> (-> error)|[X]|
|autoindex|✔|✔|✔|[P] false|[X]|
|cgi_pass|✔|✔|✔|[NP] \<empty\>|[X]|
