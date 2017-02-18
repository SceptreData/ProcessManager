#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
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

process_t *NewProcess (void (*onInit)(),
                       void (*onUpdate),
                       void (*onComplete),
                       void (*onFail)(),
                       void (*onAbort),
                       void *udata
                      );

void AttachProcess(ProcessManager *manager, process_t *newProcess);

void UpdateProcessManager(ProcessManager *manager, unsigned dt);

bool ProcessIsDead(process_t *p);

bool ProcessIsAlive(process_t *p);

#endif // PROCESSMANAGER_H
