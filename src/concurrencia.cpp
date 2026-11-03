#include "concurrencia.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
using namespace std;

queue<int> buffer;
mutex mtx;
condition_variable noLleno, noVacio;
int capacidad, totalElementos, totalProductores;
int producidos, consumidos, productoresFinalizados;

void productor(int id) {
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(100));
        unique_lock<mutex> bloqueo(mtx);

        if (producidos < totalElementos && buffer.size() == capacidad)
            cout << "[P" << id << "] Buffer lleno. Esperando...\n";

        noLleno.wait(bloqueo, [] {
            return buffer.size() < capacidad || producidos == totalElementos;
        });

        if (producidos == totalElementos) {
            productoresFinalizados++;
            cout << "[P" << id << "] Finalizo.\n";
            bloqueo.unlock();
            noVacio.notify_all();
            noLleno.notify_all();
            return;
        }

        int elemento = ++producidos;
        buffer.push(elemento);

        cout << "[P" << id << "] Produjo " << elemento
             << " | Buffer: " << buffer.size() << "/" << capacidad << endl;

        bool ultimo = producidos == totalElementos;
        bloqueo.unlock();
        noVacio.notify_one();
        if (ultimo) noLleno.notify_all();
    }
}

void consumidor(int id) {
    while (true) {
        unique_lock<mutex> bloqueo(mtx);

        if (buffer.empty() && productoresFinalizados < totalProductores)
            cout << "[C" << id << "] Buffer vacio. Esperando...\n";

        noVacio.wait(bloqueo, [] {
            return !buffer.empty() || productoresFinalizados == totalProductores;
        });

        if (buffer.empty()) {
            cout << "[C" << id << "] Finalizo.\n";
            return;
        }

        int elemento = buffer.front();
        buffer.pop();
        consumidos++;

        cout << "[C" << id << "] Retiro " << elemento
             << " | Buffer: " << buffer.size() << "/" << capacidad << endl;

        bloqueo.unlock();
        noLleno.notify_one();

        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void ejecutarHilos() {
    int numProductores, numConsumidores;

    cout << "\n============ CONFIGURACION ==============\n";
    cout << "Numero de productores (minimo 2): ";
    cin >> numProductores;
    cout << "Numero de consumidores (minimo 2): ";
    cin >> numConsumidores;
    cout << "Capacidad del buffer: ";
    cin >> capacidad;
    cout << "Cantidad total de elementos: ";
    cin >> totalElementos;

    if (numProductores < 2 || numConsumidores < 2 ||
        capacidad < 1 || totalElementos < 1) {
        cout << "Parametros no validos.\n";
        return;
    }

    while (!buffer.empty()) buffer.pop();
    producidos = 0;
    consumidos = 0;
    productoresFinalizados = 0;
    totalProductores = numProductores;

    cout << "\n=============== PRODUCTOR-CONSUMIDOR ===============\n";
    cout << "Iniciando hilos...\n";

    vector<thread> hilos;

    for (int i = 1; i <= numProductores; i++)
        hilos.emplace_back(productor, i);

    for (int i = 1; i <= numConsumidores; i++)
        hilos.emplace_back(consumidor, i);

    for (auto &hilo : hilos)
        hilo.join();

    cout << "\n============= RESULTADOS =============\n";
    cout << "Productores: " << numProductores << endl;
    cout << "Consumidores: " << numConsumidores << endl;
    cout << "Producidos: " << producidos << endl;
    cout << "Consumidos: " << consumidos << endl;
    cout << "Pendientes: " << buffer.size() << endl;
    cout << "Ejecucion finalizada.\n";
}

void moduloConcurrencia() {
    int opcion;

    do {
        cout << "\n======== MODULO II: HILOS Y CONCURRENCIA ========\n";
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

