#include "callbacks.h"
#include <pthread.h>

int singleCallToCallback(SIMPLE_CALLBACK fun, int base){
	int result;

	printf("singleCallToCallback(%p, %d)\n", fun, base);
	result = fun(base + 1);
	printf(" => %d\n", result);
	return result;
}

int callbackInALoop(SIMPLE_CALLBACK fun){
	int i;
	int acc = 0;

	for(i=0;i<42;i++){
		acc = fun(acc);
	}
	
	return acc;
}

int reentringCallback(SIMPLE_CALLBACK fun, int base){
	printf("Value entered: %d\n", base);

	if(base == 0)
		return 1;

	return fun(base);
}

int doubleCallToCallbacks(SIMPLE_CALLBACK fun1, SIMPLE_CALLBACK fun2, int base) {
    int result;

    printf("Entered doubleCalltoCallbacks\n");
    fflush(stdout);

    result = fun1(base);
    result = fun2(result);
    return result;
}


static int value = 0;

void* otherThread(void* aFunction){
	SIMPLE_CALLBACK f = (SIMPLE_CALLBACK) aFunction;
	sleep(3);
	value = f(42);
}

int getValue(){
	return value;
}

void callbackFromAnotherThread(SIMPLE_CALLBACK fun){

	value = 0;
	pthread_t t;

	pthread_create(&t, NULL, otherThread, fun);
	pthread_detach(t);
}
