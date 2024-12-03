## Inventory Manager

**Inventory Manager** es un programa de gestión de activos con interfaz de línea de comandos, desarrollado en C++. Utiliza SQLite como motor de almacenamiento de datos y un sistema inteligente de gestión de caché de productos.

El objetivo futuro para este software es ser compatible con "activos inteligentes", que son aquellos capaces de conectarse a través de la red para facilitar la gestión de inventarios.

### Construcción del proyecto:

```bash
git clone https://github.com/panprogramadorgh/inventory-manager
cd inventory-manager

# En caso de que cmake no esté instalado
sudo apt update && sudo apt upgrade
sudo apt install cmake

mkdir build && cd build
cmake ..

# Lanzar el programa
./main
```