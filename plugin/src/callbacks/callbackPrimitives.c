#include "callbacks.h"

/* primitiveReadNextWorkerCallback
 *  answers next pending callback
 */
PrimitiveWithDepth(primitiveReadNextCallback, 1){
    CallbackInvocation *address;
    sqInt externalAddress;

    address = queue_next_pending_callback();

    if(address) {
        externalAddress = interpreterProxy->instantiateClassindexableSize(interpreterProxy->classExternalAddress(), sizeof(void*));
        checkFailed();

        writeAddress(externalAddress, address);
        checkFailed();
    } else {
        externalAddress = interpreterProxy->nilObject();
    }

    primitiveEndReturn(externalAddress);
}

/*
 * Initialize the callback queue with the semaphore index
 *
 * Arguments:
 *
 *  - 0 semaphoreIndex <SmallInteger>
 */
Primitive(primitiveInitilizeCallbacks){

	int semaphoreIndex = interpreterProxy->stackIntegerValue(0);
	checkFailed();

	initilizeCallbacks(semaphoreIndex);

	primitiveEnd();
}

/* primitiveUnregisterWorkerCallback
 *  unregisters callback (taking a handle as parameter)
 *  arguments:
 *  - callbackHandle    <ExternalAddress>
 */
PrimitiveWithDepth(primitiveUnregisterCallback, 1){
    Callback *callback;

    callback = (Callback *)readAddress(interpreterProxy->stackValue(0));
    checkFailed();

    if(callback != NULL)
    	callback_release(callback);

    primitiveEnd();
}

/*
 * This primitive register a callback in libFFI.
 * This primitive generates the pointer to the function to be passed as the callback.
 * To do so, it generates all the structures expected by lib ffi.
 *
 * It only uses the receiver.
 * The receiver is a TFCallback.
 *
 * It should at least have the following instance variables
 *
 * 0: handler: The pointer to the C callback function. This is the pointer passed to the C libraries using the callback.
 * 1: callbackData: A pointer to the plugin internal data structure.
 * 2: parameterHandlers
 * 3: returnTypeHandler
 * 4: runner
 */

PrimitiveWithDepth(primitiveRegisterCallback, 3){
    sqInt callbackHandle;
    Callback *callback;
    sqInt count;
    void *handler;
    sqInt idx;
    sqInt paramArray;
    sqInt runnerInstance;
    ffi_type **parameters;
    sqInt receiver;
    ffi_type *returnType;

    receiver = getReceiver();
    checkFailed();

    callbackHandle = getAttributeOf(receiver, 1);
    checkFailed();

    paramArray = getAttributeOf(receiver, 2);
    checkFailed();

    returnType = getHandler(getAttributeOf(receiver, 3));
    checkFailed();

    runnerInstance = getAttributeOf(receiver, 4);
    checkFailed();

    Runner *runner = (Runner *)getHandler(runnerInstance);
    checkFailed();

    if(runner == NULL){
    	interpreterProxy->primitiveFail();
    	return;
	}

    count = interpreterProxy->stSizeOf(paramArray);
    checkFailed();


    //This array is freed when the callback is released.
    //If there is an error during the creation of the callback.
    //callback_new() frees it.
    parameters = malloc(count*sizeof(void*));
    for (idx = 0; idx < count; idx += 1) {
        parameters[idx] = (getHandler(interpreterProxy->stObjectat(paramArray, idx + 1)));
    }
    checkFailed();

    callback = callback_new(runner, parameters, count, returnType);
    checkFailed();

    setHandler(receiver, callback->functionAddress);
    checkFailed();

    writeAddress(callbackHandle, callback);
    checkFailed();

    printf("primitiveRegisterCallback:\n");
    printf("- handle: 0x%lx\n", callbackHandle);
    printf("- paramArray: 0x%lx\n", paramArray);
    printf("- runnerInstance: 0x%lx\n", runnerInstance);
    printf("- callback: %p\n", callback);
    fflush(stdout);

    primitiveEnd();
}

/* primitiveWorkerCallbackReturn
 *   returns from a callback
 *   receiver <TFCallbackInvocation>
 */
PrimitiveWithDepth(primitiveCallbackReturn, 2) {
    CallbackInvocation *callbackInvocation;
    Callback *callback;
    sqInt receiver, callbackInstance, runnerInstance;
    Runner *runner;

    // A callback invocation
    receiver = getReceiver();
    checkFailed();
    
    callbackInstance = getAttributeOf(receiver, 1);
    checkFailed();
    
    runnerInstance = getAttributeOf(callbackInstance, 4);
    checkFailed();
    
    runner = (Runner *)getHandler(runnerInstance);
    checkFailed();
    
    if(!runner){
        interpreterProxy->primitiveFail();
        return;
    }
    
    callbackInvocation = (CallbackInvocation*)getHandler(receiver);
    checkFailed();
    
    if(!callbackInvocation){
        interpreterProxy->primitiveFail();
        return;
    }

    printf("primitiveCallbackReturn:\n");
    callback = callbackInvocation->callback;
    printf("- callback: %p\n", callback);
    fflush(stdout);

    // If the returning callback is not the last callback that entered, we cannot return
    // Otherwise this would produce a stack corruption (returning to an older callback erasing/overriding the stack of newer ones)
    if (callbackInvocation != runner->callbackStack){
        interpreterProxy->primitiveFail();
        return;
    }
    
    // If the callback was the last one, we need to pop it from the callback stack
    runner->callbackStack = runner->callbackStack->previous;
    runner->callbackExitFunction(runner, callbackInvocation);

    primitiveEnd();
}
