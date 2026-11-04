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

Al ejecutar MiniSO, se muestra un menú principal que permite acceder a los tres módulos:

```text
1. Gestion de procesos
2. Gestion de hilos y concurrencia
3. Gestion de memoria
0. Salir
```

### Módulo I: Gestión de procesos

Permite observar la creación y ejecución de procesos hijos mediante llamadas reales al sistema operativo Linux.

**Funcionamiento:**
- Muestra los identificadores PID y PPID del proceso principal.
- Crea dos procesos hijos mediante `fork()`.
- El primer hijo ejecuta el comando `date` mediante `execvp()`.
- El segundo hijo espera 10 segundos y ejecuta `ls -l`.
- El proceso padre utiliza `waitpid()` para esperar la finalización de ambos hijos y mostrar sus estados de salida.

**Utilización:** seleccionar la opción `1` del menú principal. El módulo se ejecuta automáticamente y, al finalizar los procesos hijos, regresa al menú principal.

### Módulo II: Gestión de hilos y concurrencia

Implementa el problema productor-consumidor mediante múltiples hilos que comparten un búfer de capacidad limitada.

**Funcionamiento:**
- Crea hilos productores y consumidores mediante `std::thread`.
- Permite configurar la cantidad de productores, consumidores, elementos y capacidad del búfer.
- Utiliza `mutex` y variables de condición para coordinar el acceso al búfer compartido.
- Muestra las operaciones de producción, consumo, espera, reanudación y finalización.
- Comprueba que todos los elementos se consuman sin pérdidas ni duplicaciones.

**Utilización:** seleccionar la opción `2` del menú principal y luego `1` para ejecutar productor-consumidor. Introducir los parámetros solicitados y observar la ejecución. Al finalizar, se muestra un resumen de resultados. La opción `0` permite regresar al menú principal.

### Módulo III: Gestión de memoria

Permite simular la administración de memoria mediante particionamiento dinámico First-Fit y paginación.

**Funcionamiento:**
- Implementa First-Fit para asignar memoria al primer espacio libre de tamaño suficiente.
- Permite asignar y liberar memoria de procesos, consultar espacios libres e identificar fragmentación externa.
- Simula la paginación mediante páginas, marcos físicos y tablas de páginas.
- Permite traducir direcciones lógicas a físicas y observar la fragmentación interna.
- Incluye escenarios de simulación para comprobar la asignación, liberación y reutilización de memoria.

**Utilización:** seleccionar la opción `3` del menú principal. Después, elegir `1` para particionamiento First-Fit o `2` para paginación.

- **First-Fit:** ingresar la capacidad total de memoria y utilizar las opciones para asignar procesos, liberar espacio, consultar bloques libres o ejecutar escenarios automáticos.
- **Paginación:** configurar el tamaño de página y la cantidad de marcos físicos. Después, consultar la tabla de páginas, traducir direcciones, configurar procesos o ejecutar escenarios de fragmentación.

La opción `0` permite regresar al menú anterior.

Para finalizar MiniSO, seleccionar la opción `0` del menú principal.
