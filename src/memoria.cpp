#include "memoria.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
using namespace std;

void limpiarPantalla() {
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



struct Ocupado {
    string nombre;
    int inicio;
    int tamano;
    bool esta_ocupado;
};


// Estructura para almacenar los resultados de la traducción lógica -> física
struct ResultadoTraduccion {
    int pagina;
    int desplazamiento;
    int marco;
    int direccion_fisica;
};





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


    limpiarPantalla();
    cout << "\n--- CONFIGURACIÓN DE PARTICIONAMIENTO ---\n";
    cout << "Ingrese la memoria total del sistema (ej. 512): ";
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

	limpiarPantalla();

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





void ejecutar_escenario_paginacion(int id_escenario, string nombre, int tam_proceso, const int marcos[], int num_paginas) {
    cout << "\n=======================================================\n";
    cout << "  ESCENARIO " << id_escenario << ": Asignación y Fragmentación Interna\n";
    cout << "=======================================================\n";
    cout << "Proceso: " << nombre << " (" << tam_proceso << " bytes)\n";
    cout << "Tamaño de página/marco: " << TAM_PAGINA << " bytes\n";
    cout << "Páginas del proceso: " << num_paginas << "\n";
    cout << "Tabla de páginas (Página lógica -> Marco físico):\n";
    for (int i = 0; i < num_paginas; ++i) {
        cout << "  Página " << i << " -> Marco " << marcos[i] << "\n";
    }

    int reservados = num_paginas * TAM_PAGINA;
    int fragmentacion = reservados - tam_proceso;

    cout << "Bytes cubiertos por " << num_paginas << " marcos: " << reservados << " bytes\n";
    cout << "Fragmentación interna en la última página: " << fragmentacion << " bytes\n";
}




bool traducir_paginacion(int direccion, int tam_proceso, const int marcos[], ResultadoTraduccion &resultado) {
    if (direccion < 0 || direccion >= tam_proceso) {
        return false; // Dirección fuera del espacio de direcciones del proceso
    }
    resultado.pagina = direccion / TAM_PAGINA;
    resultado.desplazamiento = direccion % TAM_PAGINA;
    resultado.marco = marcos[resultado.pagina];
    resultado.direccion_fisica = (resultado.marco * TAM_PAGINA) + resultado.desplazamiento;
   return true;
}



static void subMenuPaginacion() {
    limpiarPantalla();
    cout << "--- CONFIGURACIÓN DE PAGINACIÓN ---\n";
    cout << "Ingrese el tamaño de página en bytes (por defecto 4096): ";
    while (!(cin >> TAM_PAGINA) || TAM_PAGINA <= 0) {
        cout << "[ERROR] Ingrese un tamaño válido de página: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Ingrese el número total de marcos físicos (ej. 12): ";
    while (!(cin >> NUM_MARCOS) || NUM_MARCOS <= 0) {
        cout << "[ERROR] Ingrese un número de marcos válido: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Configuración inicial predeterminada (Proceso A de 10240 bytes)
    int tam_proceso = 10 * 1024;
    int paginas = (tam_proceso + TAM_PAGINA - 1) / TAM_PAGINA;
    int marcos[10] = {5, 2, 8}; // Asignación simulada
    int num_marcos_asignados = paginas;

    int opcion;
    do {
        limpiarPantalla();
        cout << "=== SUBMÓDULO: PAGINACIÓN DE MEMORIA ===\n";
        cout << "Tamaño de Página: " << TAM_PAGINA << " bytes | Marcos Totales: " << NUM_MARCOS << "\n";
        cout << "Proceso actual cargado: " << tam_proceso << " bytes (" << paginas << " páginas)\n";
        cout << "---------------------------------------------------------\n";
        cout << "1. Mostrar Tabla de Páginas actual\n";
        cout << "2. Traducir dirección lógica a física (Interactivo)\n";
        cout << "3. Configurar un nuevo proceso (Asignación manual)\n";
        cout << "4. Cargar simulación de Asignación y Liberación (Fragmentación)\n";
        cout << "0. Volver al menú de Gestión de Memoria\n";
        cout << "Seleccione una opción: ";

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (opcion) {
            case 1: {
                limpiarPantalla();
                cout << "=== TABLA DE PÁGINAS DEL PROCESO ===\n";
                for (int i = 0; i < num_marcos_asignados; ++i) {
                    cout << "  Página " << i << " -> Marco " << marcos[i] << "\n";
                }
                int reservados = num_marcos_asignados * TAM_PAGINA;
                cout << "\nBytes cubiertos: " << reservados << " bytes\n";
                cout << "Fragmentación interna: " << (reservados - tam_proceso) << " bytes\n";
                pausar();
                break;
            }
            case 2: {
                limpiarPantalla();
                cout << "--- TRADUCCIÓN INTERACTIVA DE DIRECCIONES ---\n";
                cout << "Espacio de direcciones válido: 0 a " << (tam_proceso - 1) << " bytes\n\n";
                
                int dir;
                cout << "Ingrese la dirección lógica en bytes: ";
                cin >> dir;

                ResultadoTraduccion res;
                if (traducir_paginacion(dir, tam_proceso, marcos, res)) {
                    cout << "\n[ÉXITO EN TRADUCCIÓN]\n";
                    cout << "  └─> Página Lógica: " << res.pagina << "\n";
                    cout << "  └─> Desplazamiento (offset): " << res.desplazamiento << " bytes\n";
                    cout << "  └─> Marco Físico Asignado: " << res.marco << "\n";
                    cout << "  └─> Dirección Física Calculada: " << res.direccion_fisica << " bytes\n";
                } else {
                    cout << "\n[ERROR] Dirección lógica " << dir << " fuera del espacio asignado al proceso.\n";
                }
                pausar();
                break;
            }
            case 3: {
                limpiarPantalla();
                cout << "--- CONFIGURAR NUEVO PROCESO ---\n";
                cout << "Tamaño del nuevo proceso en bytes: ";
                cin >> tam_proceso;

                paginas = (tam_proceso + TAM_PAGINA - 1) / TAM_PAGINA;
                num_marcos_asignados = paginas;

                cout << "\nEl proceso requiere " << paginas << " páginas.\n";
                cout << "Ingrese los " << paginas << " marcos asignados a cada página:\n";
                for (int i = 0; i < paginas; ++i) {
                    cout << "  Página " << i << " -> Marco físico: ";
                    cin >> marcos[i];
                }
                cout << "\n[ÉXITO] Proceso cargado correctamente.\n";
                pausar();
                break;
            }
            case 4: {
                limpiarPantalla();
                cout << "--- EJECUTANDO EVIDENCIA DE ASIGNACIÓN, LIBERACIÓN Y FRAGMENTACIÓN ---\n";

                // Escenario 1: Proceso A (10 KiB)
                int tam_p1 = 10 * 1024;
                int paginas_p1 = (tam_p1 + TAM_PAGINA - 1) / TAM_PAGINA;
                int marcos_p1[] = {5, 2, 8};
                ejecutar_escenario_paginacion(1, "Proceso A", tam_p1, marcos_p1, paginas_p1);

                cout << "\n[ACCIÓN]: Finaliza Proceso A. Se liberan los marcos {5, 2, 8}.\n";

                // Escenario 2: Proceso B (6 KiB) ocupando marcos liberados
                int tam_p2 = 6 * 1024;
                int paginas_p2 = (tam_p2 + TAM_PAGINA - 1) / TAM_PAGINA;
                int marcos_p2[] = {8, 1}; // Reutiliza marcos
                ejecutar_escenario_paginacion(2, "Proceso B", tam_p2, marcos_p2, paginas_p2);

                pausar();
                break;
            }
            case 0:
                cout << "Regresando al menú de Memoria...\n";
                break;
            default:
                cout << "Opción no válida.\n";
                pausar();
        }
    } while (opcion != 0);
}



void moduloMemoria() {
    int opcion;
    do {
	limpiarPantalla();
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
                cout << "\n[!] Submódulo de Paginaciòn...\n";
                break;
            case 0:
                cout << "Saliendo del Módulo de Memoria...\n";
                break;
            default:
                cout << "Opción no válida.\n";
        }
    } while (opcion != 0);
}
