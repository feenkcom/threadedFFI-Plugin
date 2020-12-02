Extension { #name : #IceCredentialsProvider }

{ #category : #'*ThreadedFFIMigration' }
IceCredentialsProvider class >> threadedFFIPriority [
	"The higher number, the higher preference (importance).
	The higher number says which provider is suitable for Threaded FFI."
	<return: #Number>
	^ -1000
]

{ #category : #'*ThreadedFFIMigration' }
IceCredentialsProvider class >> threadedFFIProvider [
	| aCurrent |
	aCurrent := IceNonInteractiveCredentialsProvider.
	self allSubclassesDo: [ :aClass | 
		aClass isAbstract not and: [ 
			aClass threadedFFIPriority > aCurrent threadedFFIPriority
				ifTrue: [ aCurrent := aClass ] ] ].
	^ aCurrent
]
