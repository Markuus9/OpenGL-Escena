# Proyecto OpenGL y Qt: Movimiento de Objetos

Este proyecto es una aplicación gráfica desarrollada con **OpenGL** y **Qt** que simula el movimiento de un personaje en un entorno 3D. La aplicación está diseñada para ser educativa y se enfoca en los conceptos básicos de programación gráfica, interacción 3D y uso de la biblioteca Qt para construir interfaces gráficas.

![Vista de la interfaz de usuario](UI.png)
![Movimiento Personaje](luke.gif)

## Tabla de Contenidos

- [Características](#características)
- [Instalación](#instalación)
- [Uso](#uso)

## Características

- **Simulación de movimiento**: Control básico del movimiento en un plano 3D utilizando las teclas de dirección y controles de la UI.
- **Interfaz de usuario con Qt**: Controles en la UI para ajustar la posición y la orientación del barco y para modificar parámetros de iluminación como intensidad, color y posición.
- **Diseño modular**: Arquitectura de código que separa la lógica de movimiento, iluminación y UI, facilitando la expansión y el mantenimiento del código.

## Instalación

1. **Clona el repositorio**:
    ```bash
    git clone https://github.com/Markuus9/OpenGL-Escena.git
    cd OpenGL-Escena
    cd Activitat2
    ```

2. **Instala las dependencias** (si estás usando una distribución basada en Debian/Ubuntu):
    ```bash
    sudo apt update
    sudo apt install build-essential qt5-default qtbase5-dev libgl1-mesa-dev
    ```

## Uso

1. Ejecuta la aplicación desde la terminal:
    ```bash
    #compile the code
    make
    #run the program
    ./activitat2
    ```

2. **Controles de la UI**:
    - **Movimiento del Barco**: Utiliza las teclas W/S para la dirección del barco, A/D para el control del faro, y las flechas ↑/↓ para el control de la hora.
    - **Control de Luces**: Ajusta la intensidad, el color y la posición de la fuente de luz principal a través de los sliders y selectores en la UI.
