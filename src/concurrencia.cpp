#include "concurrencia.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
using namespace std;

queue<int> buffer;
mutex mtx;
condition_variable noLleno, noVacio;
int capacidad = 3;
int siguiente = 0;
int producidos = 0, consumidos = 0;

void productor(int id) {
    for (int i = 0; i < 3; i++) {
        this_thread::sleep_for(chrono::milliseconds(100));

        unique_lock<mutex> bloqueo(mtx);

        if (buffer.size() == capacidad)
            cout << "[P" << id << "] Buffer lleno. Esperando...\n";

        noLleno.wait(bloqueo, [] {
            return buffer.size() < capacidad;
        });

        int elemento = ++siguiente;
        buffer.push(elemento);
        producidos++;

        cout << "[P" << id << "] Produjo " << elemento
             << " | Buffer: " << buffer.size() << "/" << capacidad << endl;

        bloqueo.unlock();
        noVacio.notify_one();
    }

    lock_guard<mutex> bloqueo(mtx);
    cout << "[P" << id << "] Finalizo.\n";
}

void consumidor(int id) {
    for (int i = 0; i < 3; i++) {
        unique_lock<mutex> bloqueo(mtx);

        if (buffer.empty())
            cout << "[C" << id << "] Buffer vacio. Esperando...\n";

        noVacio.wait(bloqueo, [] {
            return !buffer.empty();
        });

        int elemento = buffer.front();
        buffer.pop();
        consumidos++;

        cout << "[C" << id << "] Retiro " << elemento
             << " | Buffer: " << buffer.size() << "/" << capacidad << endl;

        bloqueo.unlock();
        noLleno.notify_one();

        this_thread::sleep_for(chrono::milliseconds(200));
    }

    lock_guard<mutex> bloqueo(mtx);
    cout << "[C" << id << "] Finalizo.\n";
}

void ejecutarHilos() {
    {
        lock_guard<mutex> bloqueo(mtx);
        while (!buffer.empty()) buffer.pop();
        siguiente = 0;
        producidos = 0;
        consumidos = 0;
    }

    cout << "\n======= PRODUCTOR-CONSUMIDOR ========\n";
    cout << "Productores: 2\n";
    cout << "Consumidores: 2\n";
    cout << "Capacidad del buffer: " << capacidad << endl;

    thread p1(productor, 1);
    thread p2(productor, 2);
    thread c1(consumidor, 1);
    thread c2(consumidor, 2);

    p1.join();
    p2.join();
    c1.join();
    c2.join();

    cout << "\n============= RESULTADOS =============\n";
    cout << "Producidos: " << producidos << endl;
    cout << "Consumidos: " << consumidos << endl;
    cout << "Pendientes: " << buffer.size() << endl;
    cout << "Ejecucion finalizada.\n";
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

