#include "memoria.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
using namespace std;


void limpiaPantalla() {
#ifdef _WIN32
    system("cls");  
#else
    system("clear");
#endif
}



void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}


//Variable global para la simulaciòn de Particionamiento
int CAPACIDAD;

// Variables globales para la simulación de Paginación
int TAM_PAGINA = 4096; // Tamaño por defecto: 4 KiB (4096 bytes)
int NUM_MARCOS = 12;   // 12 marcos en memoria física simulada
const int MAX_MARCOS_PAGINACION = 1000;
const int MAX_PROCESOS_PAGINACION = 10;
const int MAX_PAGINAS_PROCESO = 1000;



struct Ocupado {
    string nombre;
    int inicio;
    int tamano;
    bool esta_ocupado;
};


// Estructura para almacenar los resultados de la traducción lógica -> física
struct ResultadoTraduccion {
    int pagina, desplazamiento, marco;
    long long direccion_fisica;
};

struct ProcesoPaginado {
    string nombre;
    long long tamano;
    int num_paginas;
    bool activo;
    int tabla[MAX_PAGINAS_PROCESO];
};

ProcesoPaginado procesosPaginacion[MAX_PROCESOS_PAGINACION];
int procesoPorMarcoPaginacion[MAX_MARCOS_PAGINACION]; // -1 = libre


bool asignar_first_fit(Ocupado ocupados[], int n, string nombre, int tam) {
    int cursor = 0;


    for (int i = 0; i < n; ++i) {
        if (!ocupados[i].esta_ocupado) continue;

        int espacio_libre = ocupados[i].inicio - cursor;
        if (espacio_libre >= tam){
            for (int j = n - 1; j > i; --j) {
                ocupados[j] = ocupados[j - 1];
            }

            ocupados[i].nombre = nombre;
            ocupados[i].inicio = cursor;
            ocupados[i].tamano = tam;
            ocupados[i].esta_ocupado = true;

            cout << "Asignado " << nombre << ": [" << cursor << ", " << (cursor + tam) << ") MiB\n";
            return true;
        }
        cursor = ocupados[i].inicio + ocupados[i].tamano;
    }

    if (CAPACIDAD - cursor >= tam) {
        for (int j = 0; j < n; ++j) {
            if (!ocupados[j].esta_ocupado) {
                ocupados[j].nombre = nombre;
                ocupados[j].inicio = cursor;
                ocupados[j].tamano = tam;
                ocupados[j].esta_ocupado = true;

                cout << "Asignado " << nombre << ": [" << cursor << ", " << (cursor + tam) << ") MiB\n";
                return true;
            }
        }
    }

//IDENTIFICACION DE FRAGMENTACIÒN EXTERNA

    int libre_total = 0;
    int temp_cursor = 0;
    for (int i = 0; i < n; ++i) {
        if (!ocupados[i].esta_ocupado) continue;
        if (ocupados[i].inicio > temp_cursor) {
            libre_total += (ocupados[i].inicio - temp_cursor);
        }
        temp_cursor = ocupados[i].inicio + ocupados[i].tamano;
    }
    if (temp_cursor < CAPACIDAD) {
        libre_total += (CAPACIDAD - temp_cursor);
    }

    cout << "NO asignado " << nombre << ": requiere " << tam << " MiB contiguos\n";
    if (libre_total >= tam) {
        cout << "  [ALERTA DE FRAGMENTACION EXTERNA]: Hay " << libre_total 
             << " MiB libres en total, pero no contiguos.\n";
    }

    return false;
}

void liberar_proceso(Ocupado ocupados[], int n, string nom) {
    bool encontrado = false;
    for (int i = 0; i < n; ++i) {
        if (ocupados[i].esta_ocupado && ocupados[i].nombre == nom) {
            cout << "Liberado " << nom << ": [" << ocupados[i].inicio << ", " 
                 << (ocupados[i].inicio + ocupados[i].tamano) << ") MiB\n";

            for (int j = i; j < n - 1; ++j) {
                ocupados[j] = ocupados[j + 1];
            }

            ocupados[n - 1].nombre = "";
            ocupados[n - 1].inicio = 0;
            ocupados[n - 1].tamano = 0;
            ocupados[n - 1].esta_ocupado = false;
            encontrado = true;
            break;
        }
    }
    if (!encontrado) {
        cout << "Proceso " << nom << " no encontrado en la memoria.\n";
    }
}


void mostrar_huecos(Ocupado ocupados[], int n) {
    int cursor = 0, libre_total = 0;
    cout << "Huecos libres actuales: [";
    bool primero = true;

    for (int i = 0; i < n; ++i) {
        if (!ocupados[i].esta_ocupado) continue;
        if (ocupados[i].inicio > cursor) {
            int tam_hueco = ocupados[i].inicio - cursor;
            libre_total += tam_hueco;
            cout << (primero ? "" : ", ") << "(" << cursor << ", " << tam_hueco << ")";
            primero = false;
        }
        cursor = ocupados[i].inicio + ocupados[i].tamano;
    }
    if (cursor < CAPACIDAD) {
        int tam_hueco = CAPACIDAD - cursor;
        libre_total += tam_hueco;
        cout << (primero ? "" : ", ") << "(" << cursor << ", " << tam_hueco << ")";
    }
    cout << "]\nMemoria libre total: " << libre_total << " MiB\n\n";
}




static void subMenuParticionamiento() {


    limpiaPantalla();
    cout << "\n--- CONFIGURACIÓN DE PARTICIONAMIENTO ---\n";
    cout << "Ingrese la memoria total del sistema: ";
    cin >> CAPACIDAD;

    Ocupado ocupados[10];
    for (int i = 0; i < 10; ++i) {
        ocupados[i].nombre = "";
        ocupados[i].inicio = 0;
        ocupados[i].tamano = 0;
        ocupados[i].esta_ocupado = false;
    }

    int opcion;
    do {
        cout << "\n=== PARTICIONAMIENTO DE MEMORIA (FIRST-FIT) ===\n";
        cout << "1. Solicitar espacio (Asignar Proceso)\n";
        cout << "2. Liberar espacio (Eliminar Proceso)\n";
        cout << "3. Mostrar estado de los bloques libres\n";
        cout << "4. Cargar simulación automática (Escenarios 1 y 2)\n";
        cout << "0. Volver al menú de Gestión de Memoria\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

	limpiaPantalla();

        switch (opcion) {
            case 1: {
                string nom;
                int tam;
                cout << "Nombre del proceso: ";
                cin >> nom;
                cout << "Tamaño requerido (MiB): ";
                cin >> tam;
                asignar_first_fit(ocupados, 10, nom, tam);
                break;
            }
            case 2: {
                string nom;
                cout << "Nombre del proceso a liberar: ";
                cin >> nom;
                liberar_proceso(ocupados, 10, nom);
                break;
            }
            case 3:
                mostrar_huecos(ocupados, 10);
		cout << "\nPresione ENTER para continuar...";
		cin.ignore();
		cin.get();


                break;
            case 4:
                cout << "\n=== ESCENARIO 1: Fragmentacion Externa (Proceso Grande) ===\n";
                asignar_first_fit(ocupados, 10, "A", 100);
                asignar_first_fit(ocupados, 10, "B", 150);
                asignar_first_fit(ocupados, 10, "C", 100);
                asignar_first_fit(ocupados, 10, "D", 100);

                liberar_proceso(ocupados, 10, "B");
                liberar_proceso(ocupados, 10, "D");
                mostrar_huecos(ocupados, 10);

                asignar_first_fit(ocupados, 10, "E", 200);

                cout << "\n=== ESCENARIO 2: Reutilizacion Exitosa de Espacios ===\n";
                asignar_first_fit(ocupados, 10, "F", 120);
                mostrar_huecos(ocupados, 10);
                break;
            case 0:
                cout << "Regresando al menú de Memoria...\n";
                break;
            default:
                cout << "Opción no válida.\n";
        }
    } while (opcion != 0);
}



//FUNCIONES PARA EL SUBMODULO DE PAGINACION




void moduloMemoria() {
    int opcion;
    do {
	limpiaPantalla();
        cout << "\n=========================================\n";
        cout << "       GESTIÓN DE MEMORIA (MINISO)        \n";
        cout << "=========================================\n";
        cout << "1. Particionamiento de memoria (First-Fit)\n";
        cout << "2. Paginación\n";
        cout << "0. Volver al menú principal\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                subMenuParticionamiento();
                break;
            case 2:
               subMenuPaginacion();
                break;
            case 0:
                cout << "Saliendo del Módulo de Memoria...\n";
                break;
            default:
                cout << "Opción no válida.\n";
        }
    } while (opcion != 0);
}
