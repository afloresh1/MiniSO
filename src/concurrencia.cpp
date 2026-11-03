#include "concurrencia.h"
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

mutex mutexSalida;

void productor(int id) {
    for (int i = 1; i <= 3; i++) {
        lock_guard<mutex> bloqueo(mutexSalida);
        cout << "[Productor " << id << "] Ejecucion " << i << endl;
    }
}

void consumidor(int id) {
    for (int i = 1; i <= 3; i++) {
        lock_guard<mutex> bloqueo(mutexSalida);
        cout << "[Consumidor " << id << "] Ejecucion " << i << endl;
    }
}

void ejecutarHilos() {
    cout << "\nIniciando hilos...\n";

    thread p1(productor, 1);
    thread p2(productor, 2);
    thread c1(consumidor, 1);
    thread c2(consumidor, 2);

    p1.join();
    p2.join();
    c1.join();
    c2.join();

    cout << "Todos los hilos finalizaron.\n";
}

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
                ejecutarHilos();
                break;
            case 0:
                cout << "Volviendo al menu principal...\n";
                break;
            default:
                cout << "Opcion no valida.\n";
        }
    } while (opcion != 0);
}

