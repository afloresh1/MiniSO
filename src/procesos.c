#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // getpid(), getppid(), fork(), execvp(), sleep()
#include <sys/wait.h>   // waitpid(), WIFEXITED(), WEXITSTATUS()

void ejecutar_modulo_procesos() {
    printf("\n=========================================\n");
    printf("     MÓDULO I: GESTIÓN DE PROCESOS      \n");
    printf("=========================================\n");
    
    // 1. Mostrar PID y PPID del proceso Padre
    printf("[PADRE] PID del proceso principal: %d\n", getpid());
    printf("[PADRE] PPID (PID de la terminal/shell): %d\n\n", getppid());

    // 2 y 3. Crear 2 procesos hijos concurrentes
    pid_t pid1, pid2;

    // --- PRIMER HIJO ---
    pid1 = fork();
    if (pid1 < 0) {
        perror("[ERROR] Falló la creación del Hijo 1");
        return;
    } 
    else if (pid1 == 0) {
        printf("[HIJO 1] Iniciado | PID: %d | PPID: %d\n", getpid(), getppid());
        printf("[HIJO 1] Ejecutando comando externo 'date'...\n");
        
        char *args[] = {"date", NULL};
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
        printf("[HIJO 2] Iniciado | PID: %d | PPID: %d\n", getpid(), getppid());
        printf("[HIJO 2] Pausando 10 segundos para verificación con pstree en Linux...\n");
        
        // Pausa necesaria para que te dé tiempo de ejecutar 'pstree' en otra terminal
        sleep(10);

        printf("[HIJO 2] Ejecutando comando externo 'ls -l'...\n");
        char *args[] = {"ls", "-l", NULL};
        execvp(args[0], args);

        perror("[ERROR] Falló execvp en Hijo 2");
        exit(EXIT_FAILURE);
    }

    // --- CÓDIGO DEL PADRE ---
    printf("\n[PADRE] Hijos creados -> Hijo 1 (PID: %d) | Hijo 2 (PID: %d)\n", pid1, pid2);
    printf("[PADRE] Esperando finalización de los procesos hijos con waitpid()...\n\n");

    int estado1, estado2;
    // 4. Esperar de forma explícita a ambos hijos
    waitpid(pid1, &estado1, 0);
    waitpid(pid2, &estado2, 0);

    // 5. Mostrar estados de finalización
    printf("\n-----------------------------------------\n");
    if (WIFEXITED(estado1)) {
        printf("[PADRE] Hijo 1 finalizó correctamente (Código: %d)\n", WEXITSTATUS(estado1));
    }
    if (WIFEXITED(estado2)) {
        printf("[PADRE] Hijo 2 finalizó correctamente (Código: %d)\n", WEXITSTATUS(estado2));
    }
    printf("=========================================\n\n");
}

int main() {
    ejecutar_modulo_procesos();
    return 0;
}
