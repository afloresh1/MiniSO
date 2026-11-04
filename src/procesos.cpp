#include "procesos.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>     // fork(), getpid(), getppid(), execvp(), sleep()
#include <sys/wait.h>   // waitpid(), WIFEXITED(), WEXITSTATUS()

using namespace std;

void moduloProcesos() {
    system("clear");

    cout << "\n=========================================\n";
    cout << "     MÓDULO I: GESTIÓN DE PROCESOS      \n";
    cout << "=========================================\n";
    
    // 1. Mostrar PID y PPID del proceso Padre
    cout << "[PADRE] PID del proceso principal: " << getpid() << "\n";
    cout << "[PADRE] PPID (PID de la terminal/shell): " << getppid() << "\n\n";

    // 2. Crear 2 procesos hijos concurrentes con fork()
    pid_t pid1, pid2;

    // --- PRIMER HIJO ---
    pid1 = fork();
    if (pid1 < 0) {
        perror("[ERROR] Falló la creación del Hijo 1");
        return;
    } 
    else if (pid1 == 0) {
        cout << "[HIJO 1] Iniciado | PID: " << getpid() << " | PPID: " << getppid() << "\n";
        cout << "[HIJO 1] Ejecutando comando externo 'date'...\n";
        
        char *args[] = {(char*)"date", NULL};
        execvp(args[0], args);
        
        perror("[ERROR] Falló execvp en Hijo 1");
        exit(EXIT_FAILURE);
    }

    // --- SEGUNDO HIJO ---
    pid2 = fork();
    if (pid2 < 0) {
        perror("[ERROR] Falló la creación del Hijo 2");
        return;
    } 
    else if (pid2 == 0) {
        cout << "[HIJO 2] Iniciado | PID: " << getpid() << " | PPID: " << getppid() << "\n";
        cout << "[HIJO 2] Pausando 10 segundos para verificación con pstree en Linux...\n";
        
        // Pausa para verificación con pstree en otra terminal
        sleep(10);

        cout << "[HIJO 2] Ejecutando comando externo 'ls -l'...\n";
        char *args[] = {(char*)"ls", (char*)"-l", NULL};
        execvp(args[0], args);

        perror("[ERROR] Falló execvp en Hijo 2");
        exit(EXIT_FAILURE);
    }

    // --- CÓDIGO DEL PADRE ---
    cout << "\n[PADRE] Hijos creados -> Hijo 1 (PID: " << pid1 << ") | Hijo 2 (PID: " << pid2 << ")\n";
    cout << "[PADRE] Esperando finalización de los procesos hijos con waitpid()...\n\n";

    int estado1, estado2;
    // Esperar a ambos hijos
    waitpid(pid1, &estado1, 0);
    waitpid(pid2, &estado2, 0);

    // Mostrar estados de finalización
    cout << "\n-----------------------------------------\n";
    if (WIFEXITED(estado1)) {
        cout << "[PADRE] Hijo 1 finalizó correctamente (Código: " << WEXITSTATUS(estado1) << ")\n";
    }
    if (WIFEXITED(estado2)) {
        cout << "[PADRE] Hijo 2 finalizó correctamente (Código: " << WEXITSTATUS(estado2) << ")\n";
    }
    cout << "=========================================\n\n";
}
