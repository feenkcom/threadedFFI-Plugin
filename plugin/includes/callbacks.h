#ifndef __CALLBACKS__
#define __CALLBACKS__

#include <ffi.h>

#include "PThreadedPlugin.h"
#include "threadSafeQueue.h"
#include "pharoSemaphore.h"
#include "pSemaphore.h"

typedef struct _Callback Callback;
typedef struct _Runner Runner;
typedef struct _CallbackInvocation CallbackInvocation;

typedef void (*CALLBACK_FUNCTION)(Runner* runner, CallbackInvocation* callback);

struct _Runner {
	CALLBACK_FUNCTION callbackEnterFunction;
	CALLBACK_FUNCTION callbackExitFunction;
	CALLBACK_FUNCTION callbackPrepareInvocation;
    CallbackInvocation *callbackStack;
};

struct _Callback {
    Runner* runner;
    ffi_closure *closure;
    ffi_cif cif;
    void *functionAddress;
    ffi_type **parameterTypes;
};

struct _CallbackInvocation {
    Callback *callback;
    void *returnHolder;
    void **arguments;
    //Optional payload used by the runners
    // In the same Thread strategy here we store the state of the interpreter to perform the sig long jump
    // In the worker it puts a semaphore to signal if the call is from another thread that is not the worker.
    // If it is the same it is NULL
    void *payload;
    void *previous;
};

Callback *callback_new(Runner * runner, ffi_type** parameters, sqInt count, ffi_type* returnType);
void callback_release(Callback* callbackData);

CallbackInvocation *queue_next_pending_callback();

void initilizeCallbacks(int pharo_semaphore_index);

#endif
