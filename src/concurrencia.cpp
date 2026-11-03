#include "concurrencia.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <limits>
#include <unistd.h>
using namespace std;

queue<int> buffer;
mutex mtx;
condition_variable noLleno, noVacio;
int capacidad, totalElementos, totalProductores;
int producidos, consumidos, productoresFinalizados;
vector<int> registroConsumo;

void limpiarPantalla() {
    if (isatty(STDOUT_FILENO))
        cout << "\033[2J\033[H";
}

bool leerNumero(const string& mensaje, int& valor, int minimo, int maximo) {
    cout << mensaje;

    if (!(cin >> valor)) {
        if (cin.eof()) return false;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada no valida.\n";
        return false;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (valor < minimo || valor > maximo) {
        cout << "Valor fuera de rango.\n";
        return false;
    }

    return true;
}

void productor(int id) {
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(100));
        unique_lock<mutex> bloqueo(mtx);

        bool esperando = producidos < totalElementos &&
                         buffer.size() == capacidad;

        if (esperando)
            cout << "[P" << id << "] Buffer lleno. Esperando...\n";

        noLleno.wait(bloqueo, [] {
            return buffer.size() < capacidad ||
                   producidos == totalElementos;
        });

        if (producidos == totalElementos) {
            productoresFinalizados++;
            cout << "[P" << id << "] Finalizo.\n";
            bloqueo.unlock();
            noVacio.notify_all();
            noLleno.notify_all();
            return;
        }

        if (esperando)
            cout << "[P" << id << "] Espacio disponible. Reanudando...\n";

        int elemento = ++producidos;
        buffer.push(elemento);

        cout << "[P" << id << "] Produjo " << elemento
             << " | Buffer: " << buffer.size() << "/" << capacidad << '\n';

        bool ultimo = producidos == totalElementos;
        bloqueo.unlock();
        noVacio.notify_one();

        if (ultimo)
            noLleno.notify_all();
    }
}

void consumidor(int id) {
    while (true) {
        unique_lock<mutex> bloqueo(mtx);

        bool esperando = buffer.empty() &&
                         productoresFinalizados < totalProductores;

        if (esperando)
            cout << "[C" << id << "] Buffer vacio. Esperando...\n";

        noVacio.wait(bloqueo, [] {
            return !buffer.empty() ||
                   productoresFinalizados == totalProductores;
        });

        if (buffer.empty()) {
            cout << "[C" << id << "] Finalizo.\n";
            return;
        }

        if (esperando)
            cout << "[C" << id << "] Elemento disponible. Reanudando...\n";

        int elemento = buffer.front();
        buffer.pop();
        consumidos++;
        registroConsumo[elemento]++;

        cout << "[C" << id << "] Retiro " << elemento
             << " | Buffer: " << buffer.size() << "/" << capacidad << '\n';

        bloqueo.unlock();
        noLleno.notify_one();

        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void ejecutarHilos() {
    int numProductores, numConsumidores;

    limpiarPantalla();

    cout << "=======================================================\n";
    cout << "                    CONFIGURACION\n";
    cout << "=======================================================\n\n";

    if (!leerNumero("Numero de productores (2 a 20): ", numProductores, 2, 20) ||
        !leerNumero("Numero de consumidores (2 a 20): ", numConsumidores, 2, 20) ||
        !leerNumero("Capacidad del buffer (1 a 100): ", capacidad, 1, 100) ||
        !leerNumero("Cantidad total de elementos (1 a 1000): ", totalElementos, 1, 1000)) {
        cout << "Configuracion cancelada o no valida.\n";
        return;
    }

    while (!buffer.empty())
        buffer.pop();

    producidos = 0;
    consumidos = 0;
    productoresFinalizados = 0;
    totalProductores = numProductores;
    registroConsumo.assign(totalElementos + 1, 0);

    cout << "\n=======================================================\n";
    cout << "                PRODUCTOR-CONSUMIDOR\n";
    cout << "=======================================================\n\n";
    cout << "Iniciando hilos...\n\n";

    vector<thread> hilos;

    for (int i = 1; i <= numProductores; i++)
        hilos.emplace_back(productor, i);

    for (int i = 1; i <= numConsumidores; i++)
        hilos.emplace_back(consumidor, i);

    for (auto& hilo : hilos)
        hilo.join();

    int perdidos = 0, duplicados = 0;

    for (int i = 1; i <= totalElementos; i++) {
        if (registroConsumo[i] == 0)
            perdidos++;
        if (registroConsumo[i] > 1)
            duplicados += registroConsumo[i] - 1;
    }

    cout << "\n=======================================================\n";
    cout << "                      RESULTADOS\n";
    cout << "=======================================================\n\n";

    cout << "Productores: " << numProductores << '\n';
    cout << "Consumidores: " << numConsumidores << '\n';
    cout << "Producidos: " << producidos << '\n';
    cout << "Consumidos: " << consumidos << '\n';
    cout << "Perdidos: " << perdidos << '\n';
    cout << "Duplicados: " << duplicados << '\n';
    cout << "Pendientes: " << buffer.size() << "\n\n";

    if (producidos == totalElementos && consumidos == totalElementos &&
        perdidos == 0 && duplicados == 0 && buffer.empty())
        cout << "Estado: EJECUCION CORRECTA\n";
    else
        cout << "Estado: ERROR EN LA EJECUCION\n";
}

void moduloConcurrencia() {
    int opcion;
    limpiarPantalla();

    while (true) {
        cout << "\n=======================================================\n";
        cout << "            MODULO II: HILOS Y CONCURRENCIA\n";
        cout << "=======================================================\n\n";

        cout << "1. Ejecutar productor-consumidor\n";
        cout << "0. Volver al menu principal\n\n";
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            if (cin.eof()) return;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada no valida.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (opcion) {
            case 1:
                ejecutarHilos();
                break;
            case 0:
                cout << "Volviendo al menu principal...\n";
                return;
            default:
                cout << "Opcion no valida.\n";
        }
    }
}

