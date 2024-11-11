# TP2 - Sistemas Operativos (72.11) - x64BareBones

Este proyecto forma parte de la asignatura 72.11 - Sistemas Operativos, y se basa en el setup básico **x64BareBones** para el desarrollo de sistemas operativos. El sistema operativo implementado proporciona servicios y funcionalidades básicas, entre ellas, una API que permite la comunicación entre el espacio de kernel y el espacio de usuario mediante **syscalls**.

## Instrucciones de Compilación y Ejecución

Sigue los pasos a continuación para compilar y ejecutar el proyecto correctamente dentro del contenedor Docker provisto por la cátedra:

1. Dirígete al directorio raíz del proyecto en el contenedor Docker.
2. En el directorio raíz del proyecto ejecuta:
    ```bash
    make clean
    ```
3. Para compilar el proyecto utilizando el gestor de memoria basado en **bitmap**, ejecuta:
    ```bash
    make all MM=BITMAP
    ```
4. Para compilar el proyecto utilizando el gestor de memoria basado en **buddy**, ejecuta:
    ```bash
    make all MM=BUDDY
    ```
5. Salir del contenedor y correr el programa ejecutando el siguiente script:
    ```bash
    ./run.sh
    ```
    - Si es necesario, ejecuta el comando anterior con permisos de superusuario:
    ```bash
    sudo ./run.sh
    ```

## Aclaracion
Si no se aclara un gestor de memoria, corre por defecto el Bitmap.

## Requisitos Previos

- **Docker**: El contenedor Docker provisto por la cátedra es necesario para ejecutar el entorno de compilación y pruebas de manera controlada.
- **Toolchain**: Las herramientas necesarias para compilar el sistema operativo se encuentran dentro de la carpeta `Toolchain`, las cuales deben ser compiladas antes de construir el proyecto principal.
