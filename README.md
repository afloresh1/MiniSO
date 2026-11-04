# MiniSO - Sistema Operativo Educativo

MiniSO es una aplicación de consola desarrollada en C++ para Linux, cuyo objetivo es demostrar el funcionamiento de mecanismos fundamentales de los sistemas operativos mediante tres módulos: gestión de procesos, hilos y concurrencia, y gestión de memoria.

## 1. Requisitos

- Sistema operativo Linux (Ubuntu).
- Compilador `g++` compatible con C++17.
- Soporte para hilos mediante POSIX Threads.
- Terminal para compilar y ejecutar la aplicación.
- Git para descargar el repositorio.

Para instalar las herramientas necesarias en Ubuntu:

```bash
sudo apt update
sudo apt install build-essential git -y
```

## 2. Compilación y ejecución

Primero, clonar el repositorio:

```bash
git clone https://github.com/afloresh1/MiniSO.git
cd MiniSO
```

Compilar todos los módulos desde la carpeta principal:

```bash
g++ -std=c++17 -pthread src/*.cpp -o main
```

Ejecutar la aplicación:

```bash
./main
```

## 3. Estructura del proyecto

```text
MiniSO/
├── src/
│   ├── main.cpp
│   ├── procesos.cpp
│   ├── procesos.h
│   ├── concurrencia.cpp
│   ├── concurrencia.h
│   ├── memoria.cpp
│   └── memoria.h
├── pruebas/
│   └── Archivos y evidencias de pruebas
├── .gitignore
├── Makefile
└── README.md
```

- **src/**: contiene el código fuente de los tres módulos y el menú principal.
- **pruebas/**: contiene los archivos utilizados para realizar pruebas y registrar sus resultados.
- **main.cpp**: integra los módulos mediante un menú interactivo.
- **.gitignore**: identifica archivos que no deben incluirse en el repositorio.

## 4. Utilización de la aplicación

Al ejecutar MiniSO, se muestra el siguiente menú:

```text
1. Gestion de procesos
2. Gestion de hilos y concurrencia
3. Gestion de memoria
0. Salir
```

### Módulo I: Gestión de procesos

Permite observar la creación y ejecución de procesos hijos mediante llamadas al sistema como `fork()`, `execvp()` y `waitpid()`, así como sus identificadores PID y PPID y estados de finalización.

### Módulo II: Gestión de hilos y concurrencia

Implementa el problema productor-consumidor utilizando hilos reales de C++.

Al seleccionar la opción 2, el usuario puede configurar:

- Número de productores y consumidores (mínimo 2 cada uno).
- Capacidad del búfer compartido (1 a 100).
- Cantidad total de elementos (1 a 1000).

Durante la ejecución se muestran las operaciones de producción, consumo, espera y reanudación. Al finalizar, se presentan los elementos producidos, consumidos, perdidos, duplicados y pendientes.

La sincronización se realiza mediante `mutex` y variables de condición.

### Módulo III: Gestión de memoria

Permite realizar simulaciones relacionadas con la administración de memoria, incluyendo particionamiento dinámico mediante First-Fit y paginación con tablas de páginas.

## 5. Pruebas

Las pruebas se encuentran en la carpeta `pruebas/`, donde se conservan los registros de ejecución utilizados para verificar el funcionamiento de los módulos.

En el Módulo II se realizaron pruebas modificando la capacidad del búfer, la cantidad de productores y consumidores y el número total de elementos, comprobando que no se produjeran pérdidas ni duplicaciones.
