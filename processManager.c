#include "processManager.h"

process_t *NewProcess (void (*onInit)(),
                       void (*onUpdate),
                       void (*onComplete),
                       void (*onFail)(),
                       void (*onAbort),
                       void *udata
                      )
{
    process_t *p = malloc(sizeof(*p));
    if (!p) {
        printf("Mem Error: Unable to create new process.\n");
        exit(1);
    }
    
    p->state = UNINITIALIZED;

    p->onInit = onInit;
    p->onUpdate = onUpdate;
    p->onComplete = onComplete;
    p->onFail = onFail;
    p->onAbort = onAbort;

    p->data = udata;

    p->child = NULL;
    p->next = NULL;

    return p;
}

bool ProcessIsDead(process_t *p)
{
    return (p->state == COMPLETE || p->state == FAILED || p->state == ABORT);
}

bool ProcessIsAlive(process_t *p)
{
    return (p->state == RUNNING || p->state == PAUSED);
}

ProcessManager *NewProcessManager(void)
{
    ProcessManager *pm = malloc(sizeof(*pm));
    pm->processList = NULL;
    pm->count = 0;
}

void AttachProcess(ProcessManager *manager, process_t *newProcess)
{
    process_t *cur = manager->processList;
    if (cur == NULL) {
        manager->processList = newProcess;
    } else {
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = newProcess;
    }
    manager->count++;
    printf("Total Processes: %d\n", manager->count);
}

void RemoveProcess(process_t *cur, process_t *prev)
{
    process_t *kill = cur;
    /* Node is head */
    if (prev == NULL) {
        cur = cur->next;
    /* Node is in middle of list somewhere */
    } else {
        cur = cur->next;
        prev->next = cur;
    }

    free(kill);
}

// Element is Head, list is empty
// Element is After Head
// Element is Head, rest of list exists.
void UpdateProcessManager(ProcessManager *manager, unsigned dt)
{
    process_t *cur = NULL, *prev = NULL;
    cur = manager->processList;

    while (cur) {
        if (cur->state == UNINITIALIZED)
            cur->onInit(cur);

        if (cur->state == RUNNING)
            cur->onUpdate(cur, dt);

        if (ProcessIsDead(cur)) {

            switch (cur->state) {

                case COMPLETE:
                    if(cur->onComplete)
                        cur->onComplete(cur);
                    if (cur->child != NULL){
                        AttachProcess(manager, cur->child);
                    }
                    break;

                case FAILED:
                    if (cur->onFail)
                        cur->onFail(cur);
                    break;

                case ABORT:
                    if (cur->onAbort)
                        cur->onAbort(cur);
                    break;
            }

            /* Delete our process from the List */
            process_t *tmp = cur;
            if (prev == NULL)
                cur = cur->next;
            else{
                prev->next = cur->next;
                cur = cur->next;
            }
            free(tmp);

            manager->count--;
        } else {
            prev = cur;
            cur = cur->next;
        }

    }
}
