#include "memoria.h"
#include <iostream>
#include <string>

using namespace std;
int CAPACIDAD;

struct Ocupado {
    string nombre;
    int inicio;
    int tamano;
    bool esta_ocupado;
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




void moduloMemoria() {
    cout << "Ejecutando modulo de memoria..." << endl;
}

