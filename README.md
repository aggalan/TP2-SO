# TP2 - Sistemas Operativos (72.11) - x64BareBones

Este proyecto forma parte de la asignatura 72.11 - Sistemas Operativos, y se basa en el setup básico **x64BareBones** para el desarrollo de sistemas operativos. El sistema operativo implementado proporciona servicios y funcionalidades básicas, entre ellas, una API que permite la comunicación entre el espacio de kernel y el espacio de usuario mediante **syscalls**.

## Instrucciones de Compilación y Ejecución

Sigue los pasos a continuación para compilar y ejecutar el proyecto correctamente dentro del contenedor Docker provisto por la cátedra:

1. Dirígete al directorio raíz del proyecto en el contenedor Docker.
2. Accede a la carpeta `Toolchain` y ejecuta el siguiente comando para limpiar y compilar las herramientas necesarias:
    ```bash
    make clean all
    ```
3. Regresa al directorio raíz del proyecto y ejecuta:
    ```bash
    make clean
    ```
4. Para compilar el proyecto utilizando el gestor de memoria basado en **bitmap**, ejecuta:
    ```bash
    make all
    ```
5. Para compilar el proyecto utilizando el gestor de memoria basado en **buddy**, ejecuta:
    ```bash
    make all buddy
    ```
6. Corre el programa ejecutando el siguiente script:
    ```bash
    ./run.sh
    ```
    - Si es necesario, ejecuta el comando anterior con permisos de superusuario:
    ```bash
    sudo ./run.sh
    ```

## Requisitos Previos

- **Docker**: El contenedor Docker provisto por la cátedra es necesario para ejecutar el entorno de compilación y pruebas de manera controlada.
- **Toolchain**: Las herramientas necesarias para compilar el sistema operativo se encuentran dentro de la carpeta `Toolchain`, las cuales deben ser compiladas antes de construir el proyecto principal.
