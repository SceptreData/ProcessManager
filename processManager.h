#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
/*
 *   ProcessManager v.01
 *   David Bergeron
 *
 *   A system I implemented to handle various subsystems and calls inside a
 *   game engine. Processes are updated everytime we call UpdateProcessManager().
 *
 *   Every process may have a child process, which it adds to our process list when
 *   the parent process is complete. This allows us to chain various complex interactions,
 *   for instance:
 *       Process A -> Character paths towards destination.
 *       Process B -> Character Interacts with object at destination.
 *
 *   If Process B is a child-process of Process A, it will only be added to the 
 *   chain once Process A has succesfully completed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct process_t {
    enum {
        UNINITIALIZED = 0,
        RUNNING,
        PAUSED,
        COMPLETE,
        FAILED,
        ABORT
    } state;

    void (*onInit)();
    void (*onUpdate)();
    void (*onComplete)();
    void (*onFail)();
    void (*onAbort)();

    void *data;

    struct process_t *child;
    struct process_t *next;
} process_t;

typedef struct processManager_t {
    process_t *processList;
    unsigned count;
} ProcessManager;

ProcessManager *NewProcessManager(void);

/* Here we register a processes Callback functions, and can pass data to udata */
process_t *NewProcess (void (*onInit)(),
                       void (*onUpdate),
                       void (*onComplete),
                       void (*onFail)(),
                       void (*onAbort),
                       void *udata
                      );

/* Attach a process to our ProcessManager's process chain. */
void AttachProcess(ProcessManager *manager, process_t *newProcess);

/* Traverse our process chain, executing the appropriate callbacks. */
void UpdateProcessManager(ProcessManager *manager, unsigned dt);

bool ProcessIsDead(process_t *p);
bool ProcessIsAlive(process_t *p);

#endif // PROCESSMANAGER_H
