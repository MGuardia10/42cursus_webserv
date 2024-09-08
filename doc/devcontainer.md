# Pasos para usar Docker + devcontainer

## 1. Instalaciones

- Instalar [docker](https://docs.docker.com/get-started/get-docker/) en tu máquina.
- Instalar [extensión de Docker](https://marketplace.visualstudio.com/items?itemName=ms-azuretools.vscode-docker) para VSCode
- Instalar [extensión de Devcontainer](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) para VSCode

## 2. Compartir credenciales de git con contenedor remoto

Esta todo explicado en este [articulo](https://code.visualstudio.com/remote/advancedcontainers/sharing-git-credentials)

Basicamente tienes que añadir la clave ssh que tienes en tu cuenta de github al agente de SSH y la extension de devcontainer la recogerá automáticamente si uno está en funcionamiento.

Puedes añadir tus claves SSH locales al agente si está en funcionamiento utilizando el comando ssh-add. Por ejemplo, ejecuta esto desde una terminal o PowerShell:

```bash
ssh-add $HOME/.ssh/<tu clave ssh privada>
```

En Windows y Linux, es posible que recibas un error porque el agente no está en funcionamiento (macOS típicamente lo tiene en funcionamiento por defecto). Sigue estos pasos para resolver el problema:

#### WINDOWS

Inicia una PowerShell local como Administrador y ejecuta los siguientes comandos:

```powershell
	# Asegúrate de que estás ejecutando como Administrador
	Set-Service ssh-agent -StartupType Automatic
	Start-Service ssh-agent
	Get-Service ssh-agent
```

#### Linux:

Primero, inicia el Agente SSH en segundo plano ejecutando lo siguiente en una terminal:


```bash
eval "$(ssh-agent -s)"
````

Luego añade estas líneas a tu `~/.bash_profile` o `~/.zprofile` (para Zsh) para que se inicie al iniciar sesión:

```bash
if [ -z "$SSH_AUTH_SOCK" ]; then
   # Verifica si hay una instancia actualmente en funcionamiento del agente
   RUNNING_AGENT="`ps -ax | grep 'ssh-agent -s' | grep -v grep | wc -l | tr -d '[:space:]'`"
   if [ "$RUNNING_AGENT" = "0" ]; then
        # Lanza una nueva instancia del agente
        ssh-agent -s &> $HOME/.ssh/ssh-agent
   fi
   eval `cat $HOME/.ssh/ssh-agent` > /dev/null
   ssh-add $HOME/.ssh/<tu clave ssh> 2> /dev/null
fi
```
En la última línea, reemplaza `<tu clave ssh>` con tu clave SSH específica.

Por ejemplo, ```ssh-add $HOME/.ssh/id_ed25519 2> /dev/null```

## 3. Abrir el proyecto en VSCode y lanzar el contenedor remoto

Una vez abierto el directorio del proyecto en VSCode. Abres una paleta de comandos `Ctrl + Shift + P` o `CMD + Shift + P` y buscas la opción:

- **Remote-Containers: Rebuild and Reopen in Container**

o en español:

- **Contenedores de desarrollo: Recompilar y Reabir en contenedor**

Se abrirá una nueva ventana de VSCode y te cargará el directorio de proyecto con las extensiones y dependencias de desarrollo especificadas.