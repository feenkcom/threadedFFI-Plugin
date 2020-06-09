Class {
	#name : #TFRunner,
	#superclass : #FFIExternalReference,
	#instVars : [
		'exceptionHandler',
		'semaphorePool',
		'callbackReturnSemaphore'
	],
	#category : #'ThreadedFFI-Worker'
}

{ #category : #accessing }
TFRunner >> callbackStackSize [

	^self primitiveCallbackStackSize
]

{ #category : #errors }
TFRunner >> cannotReturnCallbackFromOldSession: aCallbackInvocation [

	^ TFInvalidSessionCallbackReturn new
		callbackInvocation: aCallbackInvocation;
		signal
]

{ #category : #errors }
TFRunner >> cannotReturnCallbackInWrongOrder: aCallbackInvocation [

	^ TFIncorrectOrderCallbackReturn new
		callbackInvocation: aCallbackInvocation;
		previousCallbacks: #();
		signal
]

{ #category : #executing }
TFRunner >> doInitialize [

	semaphorePool := self newSemaphorePool
]

{ #category : #executing }
TFRunner >> ensureInitialized [
	"Only initialize if the image has been restarted and thus I have no handle"
	self isNull ifTrue: [ self doInitialize ]
]

{ #category : #errors }
TFRunner >> exceptionHandler [

	^ exceptionHandler ifNil: [ 
		exceptionHandler := TFForkCallbackExceptionHandler new ]
]

{ #category : #errors }
TFRunner >> exceptionHandler: anExceptionHandler [ 
	
	exceptionHandler := anExceptionHandler
]

{ #category : #executing }
TFRunner >> executeCallback: aCallbackInvocation [
	"Entry point to execute a callback invocation.
	Runs the callback in a separate green thread and stack it.
	The stack gives information about the order in which callbacks should return.
	If a callback does not return in the right order, an exception is thrown.
	
	To guarantee the stack is not modified while this method runs, this method should be called from a high priority process.
	  => no callbacks should finish while this method runs."

	stackProtect critical: [ callbackInvocationStack push: aCallbackInvocation ].
	aCallbackInvocation callback runStrategy executeCallback: aCallbackInvocation on: self.

]

{ #category : #executing }
TFRunner >> executeFunction: aTFExternalFunction withArguments: argumentHolder withReturnHolder: aReturnHolder usingSemaphore: anInteger [

	self ensureInitialized.

	self
		primitivePerformWorkerCall: aTFExternalFunction
		withArguments: argumentHolder
		withReturnHolder: aReturnHolder
		usingSemaphore: anInteger
]

{ #category : #errors }
TFRunner >> handleExceptionDuring: aBlock [

	self exceptionHandler handleExceptionDuring: aBlock
]

{ #category : #executing }
TFRunner >> invokeFunction: aTFExternalFunction [ 
	
	^ self invokeFunction: aTFExternalFunction withArguments: #()
]

{ #category : #executing }
TFRunner >> invokeFunction: aTFExternalFunction withArguments: aCollection [ 

	^ aTFExternalFunction newCall
		parameters: aCollection;
		executeOn: self
]

{ #category : #private }
TFRunner >> newSemaphorePool [

	^ TFPool
		newProvider: [ TFExternalSemaphore new ] 
		size: self semaphorePoolSize
]

{ #category : #private }
TFRunner >> primitiveCallbackStackSize [
	"Answer the callbackStack size"

	<primitive: 'primitiveCallbackStackSize' module: 'PThreadedPlugin' error: ec>
	self primitiveFailed: ec
]

{ #category : #private }
TFRunner >> primitivePerformWorkerCall: aTFExternalFunction
		withArguments: argumentHolder
		withReturnHolder: aReturnHolder
		usingSemaphore: anInteger [
		
	^ self subclassResponsibility
]

{ #category : #executing }
TFRunner >> release [ 
	
	handle beNull.

]

{ #category : #executing }
TFRunner >> returnCallback: aCallbackInvocation [
	"Entry point to return a callback invocation.
	Check the callbackInvocation stack to see if the given invocation is the last one.
	If so, it can return safely.
	Otherwise, throw an exception as returning means a bug in your application.
	The user must guarantee callbacks return in the correct order"

	callbackReturnSemaphore ifNil: [ 
		callbackReturnSemaphore := Semaphore new ].

	aCallbackInvocation isNull 
		ifTrue: [ ^ self ].

	callbackInvocationStack isEmptyOrNil ifTrue: [ 
		^ self cannotReturnCallbackFromOldSession: aCallbackInvocation].

	stackProtect critical: [
		(callbackInvocationStack top == aCallbackInvocation) 
			ifTrue: [
				aCallbackInvocation returnExecution
					ifTrue: [
						"If we are starting a session and an execution was executed, it 
						 may happen that the stack is empty."
						callbackInvocationStack ifEmpty: [ ^ self ].
						callbackInvocationStack pop.
						[ callbackReturnSemaphore signalAll ] valueUnpreemptively.
						^ self ]] ].
	"failed"
	callbackReturnSemaphore wait.
	self returnCallback: aCallbackInvocation
]

{ #category : #accessing }
TFRunner >> semaphorePool [
	
	self ensureInitialized.
	^ semaphorePool
]

{ #category : #accessing }
TFRunner >> semaphorePoolSize [

	^ 5
]
