#include "concurrencia.h"
#include <iostream>
using namespace std;

void moduloConcurrencia() {
    int opcion;

    do {
        cout << "\n===== MODULO II: HILOS Y CONCURRENCIA =====\n";
        cout << "1. Ejecutar productor-consumidor\n";
        cout << "0. Volver al menu principal\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "Productor-consumidor pendiente de implementacion.\n";
                break;
            case 0:
                cout << "Volviendo al menu principal...\n";
                break;
            default:
                cout << "Opcion no valida.\n";
        }
    } while (opcion != 0);
}

