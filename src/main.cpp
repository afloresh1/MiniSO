#include <iostream>
#include <limits>

#include "procesos.h"
#include "concurrencia.h"
#include "memoria.h"

using namespace std;

int main() {
    int opcion;

    do {
        cout << "                    ================================                  ";
	cout << endl;
        cout << "                                 MINISO";
	cout << endl;
        cout << "                    ===============================";
	cout << endl;
        cout << "1. Gestion de procesos\n";
        cout << "2. Gestion de hilos y concurrencia\n";
        cout << "3. Gestion de memoria\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";


        switch (opcion) {
            case 1:
                moduloProcesos();
                break;

            case 2:
                moduloConcurrencia();
                break;

            case 3:
                moduloMemoria();
                break;

            case 0:
                cout << "Saliendo de MiniSO...\n";
                break;

            default:
                cout << "Opcion no valida.\n";
        }

    } while (opcion != 0);

    return 0;
}
