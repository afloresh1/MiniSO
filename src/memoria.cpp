#include "memoria.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
using namespace std;


void limpiaPantalla() {
  if (getenv("NO_LIMPIAR") != NULL) return;
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


void limpiarEntradaPaginacion() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

long long leerNumeroPaginacion(const string& mensaje, long long minimo, long long maximo) {
    long long valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor >= minimo && valor <= maximo) return valor;
        cout << "[ERROR] Ingrese un numero entre " << minimo << " y " << maximo << ".\n";
        limpiarEntradaPaginacion();
    }
}

void inicializarPaginacion() {
    for (int i = 0; i < MAX_MARCOS_PAGINACION; ++i) procesoPorMarcoPaginacion[i] = -1;
    for (int i = 0; i < MAX_PROCESOS_PAGINACION; ++i) {
        procesosPaginacion[i].nombre = "";
        procesosPaginacion[i].tamano = 0;
        procesosPaginacion[i].num_paginas = 0;
        procesosPaginacion[i].activo = false;
    }
}

int buscarProcesoPaginacion(const string& nombre) {
    for (int i = 0; i < MAX_PROCESOS_PAGINACION; ++i)
        if (procesosPaginacion[i].activo && procesosPaginacion[i].nombre == nombre) return i;
    return -1;
}

int buscarEspacioProcesoPaginacion() {
    for (int i = 0; i < MAX_PROCESOS_PAGINACION; ++i)
        if (!procesosPaginacion[i].activo) return i;
    return -1;
}

int contarMarcosLibresPaginacion() {
    int libres = 0;
    for (int i = 0; i < NUM_MARCOS; ++i)
        if (procesoPorMarcoPaginacion[i] == -1) ++libres;
    return libres;
}

bool hayProcesosActivosPaginacion() {
    for (int i = 0; i < MAX_PROCESOS_PAGINACION; ++i)
        if (procesosPaginacion[i].activo) return true;
    return false;
}

void mostrarProcesoPaginacion(int indice) {
    if (indice < 0 || indice >= MAX_PROCESOS_PAGINACION || !procesosPaginacion[indice].activo) return;
    ProcesoPaginado& p = procesosPaginacion[indice];
    long long reservados = (long long)p.num_paginas * TAM_PAGINA;
    long long usadosUltima = p.tamano - (long long)(p.num_paginas - 1) * TAM_PAGINA;
    cout << "\nProceso: " << p.nombre << " | Tamano: " << p.tamano << " bytes\n";
    cout << "Paginas: " << p.num_paginas << "\nPagina logica -> Marco fisico\n";
    for (int i = 0; i < p.num_paginas; ++i) cout << "  " << i << " -> " << p.tabla[i] << "\n";
    cout << "Bytes reservados: " << reservados << " | Usados en ultima pagina: "
         << usadosUltima << " de " << TAM_PAGINA << "\n";
    cout << "Fragmentacion interna: " << reservados - p.tamano << " bytes\n";
}

void mostrarTablasPaginacion() {
    bool hayProcesos = false;
    cout << "\n========== TABLAS DE PAGINAS ==========\n";
    for (int i = 0; i < MAX_PROCESOS_PAGINACION; ++i)
        if (procesosPaginacion[i].activo) { mostrarProcesoPaginacion(i); hayProcesos = true; }
    if (!hayProcesos) cout << "No hay procesos asignados.\n";
}

bool asignarProcesoPaginacion(const string& nombre, long long tamano) {
    if (nombre.empty() || tamano <= 0) { cout << "[ERROR] Nombre o tamano no valido.\n"; return false; }
    if (buscarProcesoPaginacion(nombre) != -1) { cout << "[ERROR] Ya existe ese proceso.\n"; return false; }
    long long paginas = (tamano + TAM_PAGINA - 1) / TAM_PAGINA;
    if (paginas > MAX_PAGINAS_PROCESO) { cout << "[ERROR] Supera el limite de paginas por proceso.\n"; return false; }
    if (paginas > NUM_MARCOS || paginas > contarMarcosLibresPaginacion()) {
        cout << "[ERROR] Marcos insuficientes. Necesita " << paginas << " y hay "
             << contarMarcosLibresPaginacion() << " libres.\n";
        return false;
    }
    int indice = buscarEspacioProcesoPaginacion();
    if (indice == -1) { cout << "[ERROR] Se alcanzo el limite de procesos.\n"; return false; }
    ProcesoPaginado& p = procesosPaginacion[indice];
    p.nombre = nombre; p.tamano = tamano; p.num_paginas = (int)paginas; p.activo = true;
    for (int i = 0; i < p.num_paginas; ++i) p.tabla[i] = -1;
    int pagina = 0;
    for (int marco = 0; marco < NUM_MARCOS && pagina < p.num_paginas; ++marco) {
        if (procesoPorMarcoPaginacion[marco] == -1) {
            p.tabla[pagina++] = marco;
            procesoPorMarcoPaginacion[marco] = indice;
        }
    }
    cout << "\n[ASIGNACION SIMULADA CORRECTA] " << nombre << " ocupa " << paginas << " marcos.\n";
    mostrarProcesoPaginacion(indice);
    return true;
}

bool liberarProcesoPaginacion(const string& nombre) {
    int indice = buscarProcesoPaginacion(nombre);
    if (indice == -1) { cout << "[ERROR] Proceso inexistente o ya liberado.\n"; return false; }
    ProcesoPaginado& p = procesosPaginacion[indice];
    cout << "\n[LIBERACION SIMULADA] Proceso " << p.nombre << ":\n";
    for (int i = 0; i < p.num_paginas; ++i) {
        int marco = p.tabla[i];
        if (marco >= 0 && marco < NUM_MARCOS && procesoPorMarcoPaginacion[marco] == indice) {
            cout << "Pagina " << i << " libera marco " << marco << ".\n";
            procesoPorMarcoPaginacion[marco] = -1;
        }
        p.tabla[i] = -1;
    }
    p.nombre = ""; p.tamano = 0; p.num_paginas = 0; p.activo = false;
    cout << "Proceso liberado correctamente.\n";
    return true;
}

void mostrarEstadoMarcosPaginacion() {
    int ocupados = 0;
    cout << "\n========== ESTADO DE MARCOS FISICOS ==========\n";
    for (int marco = 0; marco < NUM_MARCOS; ++marco) {
        int indice = procesoPorMarcoPaginacion[marco];
        cout << "Marco " << marco << ": ";
        if (indice == -1) cout << "LIBRE\n";
        else if (indice >= 0 && indice < MAX_PROCESOS_PAGINACION && procesosPaginacion[indice].activo) {
            ++ocupados;
            int pagina = -1;
            for (int i = 0; i < procesosPaginacion[indice].num_paginas; ++i)
                if (procesosPaginacion[indice].tabla[i] == marco) { pagina = i; break; }
            cout << "OCUPADO por " << procesosPaginacion[indice].nombre << " (pagina " << pagina << ")\n";
        } else cout << "[ERROR: registro inconsistente]\n";
    }
    cout << "Marcos ocupados: " << ocupados << " de " << NUM_MARCOS
         << " | Libres: " << NUM_MARCOS - ocupados << "\n";
    cout << "Capacidad fisica simulada: " << (long long)NUM_MARCOS * TAM_PAGINA << " bytes\n";
}


bool traducir_paginacion(int indice, long long direccion, ResultadoTraduccion& r) {
    if (indice < 0 || indice >= MAX_PROCESOS_PAGINACION || !procesosPaginacion[indice].activo) return false;
    ProcesoPaginado& p = procesosPaginacion[indice];
    if (direccion < 0 || direccion >= p.tamano) return false;
    int pagina = (int)(direccion / TAM_PAGINA), desplazamiento = (int)(direccion % TAM_PAGINA);
    if (pagina >= p.num_paginas) return false;
    int marco = p.tabla[pagina];
    if (marco < 0 || marco >= NUM_MARCOS || procesoPorMarcoPaginacion[marco] != indice) return false;
    r.pagina = pagina; r.desplazamiento = desplazamiento; r.marco = marco;
    r.direccion_fisica = (long long)marco * TAM_PAGINA + desplazamiento;
    return true;
}


void ejecutarEscenariosPaginacion() {
    if (hayProcesosActivosPaginacion()) { 
	cout << "[AVISO] Libere los procesos activos antes de ejecutar los escenarios.\n"; 
	return; 
    }
    cout << "\n===== ESCENARIO 1: ASIGNACION Y FRAGMENTACION =====\n";

    if (NUM_MARCOS >= 8) {
        asignarProcesoPaginacion("A", 2LL * TAM_PAGINA + 1);
        asignarProcesoPaginacion("B", (long long)TAM_PAGINA + 1);
        asignarProcesoPaginacion("C", 2LL * TAM_PAGINA);
        cout << "\nSe libera B para dejar marcos libres entre marcos ocupados.\n";
        liberarProcesoPaginacion("B"); mostrarEstadoMarcosPaginacion();
        cout << "\nSe asigna D; puede utilizar marcos no contiguos.\n";
        asignarProcesoPaginacion("D", 2LL * TAM_PAGINA + 1); mostrarEstadoMarcosPaginacion();
        liberarProcesoPaginacion("A"); liberarProcesoPaginacion("C"); liberarProcesoPaginacion("D");
    } else cout << "Se necesitan al menos 8 marcos para este escenario.\n";
    cout << "\n===== ESCENARIO 2: REUTILIZACION DE MARCOS =====\n";
    if (NUM_MARCOS >= 3) {
        asignarProcesoPaginacion("E", (long long)TAM_PAGINA + 1);
        asignarProcesoPaginacion("F", TAM_PAGINA);
        cout << "\nSe libera E y G reutiliza sus marcos.\n";
        liberarProcesoPaginacion("E"); asignarProcesoPaginacion("G", (long long)TAM_PAGINA + 1);
        mostrarEstadoMarcosPaginacion(); liberarProcesoPaginacion("F"); liberarProcesoPaginacion("G");
    } else cout << "Se necesitan al menos 3 marcos para este escenario.\n";
    cout << "\nFin de los escenarios.\n"; mostrarEstadoMarcosPaginacion();
}



static void subMenuPaginacion() {
    cout << "\n--- CONFIGURACION DE PAGINACION ---\n";
    TAM_PAGINA = (int)leerNumeroPaginacion("Tamano de pagina en bytes (1 a 1000000; sugerido 4096): ", 1, 1000000);
    NUM_MARCOS = (int)leerNumeroPaginacion("Numero de marcos (1 a 1000; sugerido 12): ", 1, MAX_MARCOS_PAGINACION);
    inicializarPaginacion();
    cout << "\n[MODO SIMULACION] No se reserva RAM real ni se crean o modifican procesos/hilos de Linux.\n";
    int opcion;
    do {
        cout << "\n===== SUBMODULO: PAGINACION =====\n";
        cout << "Pagina: " << TAM_PAGINA << " bytes | Marcos: " << NUM_MARCOS << "\n";
        cout << "1. Mostrar tablas de paginas\n2. Traducir direccion logica a fisica\n";
        cout << "3. Crear proceso y asignar paginas\n4. Liberar proceso\n";
        cout << "5. Mostrar estado de marcos\n6. Ejecutar escenarios automaticos\n0. Volver\nOpcion: ";
        if (!(cin >> opcion)) { cout << "[ERROR] Ingrese una opcion numerica.\n"; limpiarEntradaPaginacion(); continue; }
        switch (opcion) {
            case 1: mostrarTablasPaginacion(); pausar(); break;
            case 2: {
                string nombre; cout << "Nombre del proceso: "; cin >> nombre;
                int indice = buscarProcesoPaginacion(nombre);
                if (indice == -1) cout << "[ERROR] Proceso inexistente o liberado.\n";
                else {
                    long long dir = leerNumeroPaginacion("Direccion logica en bytes: ", numeric_limits<long long>::min(), numeric_limits<long long>::max());
                    ResultadoTraduccion r;
                    if (traducir_paginacion(indice, dir, r)) {
                        cout << "\n[TRADUCCION CORRECTA]\nDireccion logica: " << dir << " bytes\n";
                        cout << "Pagina: " << r.pagina << " | Desplazamiento: " << r.desplazamiento << " bytes\n";
                        cout << "Marco fisico: " << r.marco << "\nCalculo: (" << r.marco << " x " << TAM_PAGINA << ") + " << r.desplazamiento << "\n";
                        cout << "Direccion fisica: " << r.direccion_fisica << " bytes\n";
                    } else cout << "[ERROR] Direccion invalida. Rango valido: 0 a " << procesosPaginacion[indice].tamano - 1 << " bytes.\n";
                }
                pausar(); break;
            }
            case 3: {
                string nombre; cout << "Nombre del proceso (sin espacios): "; cin >> nombre;
                long long tamano = leerNumeroPaginacion("Tamano del proceso en bytes: ", 1, (long long)TAM_PAGINA * MAX_PAGINAS_PROCESO);
                asignarProcesoPaginacion(nombre, tamano); pausar(); break;
            }
            case 4: { string nombre; cout << "Proceso a liberar: "; cin >> nombre; liberarProcesoPaginacion(nombre); pausar(); break; }
            case 5: mostrarEstadoMarcosPaginacion(); pausar(); break;
            case 6: ejecutarEscenariosPaginacion(); pausar(); break;
            case 0: cout << "Regresando al menu de Gestion de Memoria...\n"; break;
            default: cout << "[ERROR] Opcion no valida.\n"; pausar();
        }
    } while (opcion != 0);
}

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
