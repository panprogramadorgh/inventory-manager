# Inventory Manager

## Introduccion

**Inventory Manager** es un programa de **gestión de activos con interfaz de línea de comandos**, desarrollado en C++. Utiliza **SQLite** como motor de almacenamiento de datos y un sistema inteligente de gestión de caché de productos (por el momento esta caracteristica no es aprovechada dado el estado actual del software).

## Herramientas necesarias para la construccion

### CMake

En primer lugar vamos a necesitar la popular herramienta de generacion de Makefiles **CMake**, la cual podremos obtener facilmente desde los repositorios estandar de Ubuntu de la siguiente manera:

> [Instalar CMake](https://atekihcan.com/blog/random/installing-latest-cmake-from-source-on-linux-distro/) con un make file directamente

```bash
# En caso de que cmake no esté instalado
sudo apt update && sudo apt upgrade
sudo apt install cmake
```

### Python 3

Si ejecutaramos el archivo CMakeLists.txt sin tener instalado un interprete de python 3 **experimentaremos problemas a la hora de inicializar la base de datos sqlite**. El proceso de inicializacion es automatizado con un script de python que lanza el siguiente [codigo sql](https://github.com/panprogramadorgh/inventory-manager/blob/main/scripts/database/initializer.sql) ubicado en este mismo repositorio. El script de python emplea el modulo sqlite3 como driver de base de datos.

Para instalar python en distribuciones basadas en debian:

> O caso de que se prefiera [instalar desde los fuentes de python](https://chatgpt.com/share/675dfb0a-f198-8003-8b3f-217ae1ced0f1), en cuyo caso especifico si sera necesario instalar pos separado la libreria de python `libsqlite3-dev` (ya es incluida si se instala con el gestor apt).

```bash
sudo apt install python3
```

Adicionalmente instalaremos la libreria pandas para la representacion, manipulacion y analisis.

```bash
pip install pandas
```

## Construccion del proyecto:

Comenzaremos clonando el repositorio y generando el principal Makefile del proyecto:

```bash
git clone https://github.com/panprogramadorgh/inventory-manager
cd inventory-manager

mkdir build && cd build
cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
make

# Lanzamos el programa ubicado en este mismo directorio
./invman get --id 2
```

Si lo que se busca es **instalar el software en la maquina** podemos hacer lo siguiente:

```bash
sudo make install
# Lanzamos el programa instalado en /usr/local/bin
invman get --id 3
```

> En los ejemplos de arriba se utiliza a modo de prueba la interaz de comandos del programa, no obstante el interface de comandos cambiara drasticamente en versiones posteriores a v1.1.0.

Es posible que al momento de lanza el programa obtengamos un **error relacionado con la libreria del sistema ld.so**. Esta libreria se encarga de gestionar en tiempo de ejecucion las dependencias (libreria dinamica) para el programa. De manera predeterminada en la mayoria de sistemas ld.so no tiene configurara la ruta de busqueda de librerias dinamicas para /usr/local, puesto que esta no es una ruta estandar para el sistema y mas bien esta orientada a instalaciones de terceros.

Para agregar una nueva ruta de enlace de librerias dinamicas en ls.so podemos modificar una variable de entorno de la siguiente manera:

```bash
# Adds a new library dir
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

Es altamente **recomendable agregar la anterior linea** a un script de inicio de sesion como `$HOME/.bashrc`

**Para desinstalar el software** podemos ejecutar el pseudo-target `uninstall` de la siguiente manera:

```bash
sudo make uninstall
```

> La ubicacion del archivo de base de datos sqlite se encuentra en $HOME/.local/share/invman (en caso de que usted este interesado en hacer copias de seguridad o simplemente eliminar los datos). El archivo de base de datos es generado al momento de configuracion de CMake, mientras que la inicializacion de la base de datos (lanzar el script de python) se hace al momento de construccion (ya sea que se esta instalando en el directorio de construccion o en /usr/local).
