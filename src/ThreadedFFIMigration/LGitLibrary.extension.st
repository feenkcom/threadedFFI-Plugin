Extension { #name : #LGitLibrary }

{ #category : #'*ThreadedFFIMigration' }
LGitLibrary >> calloutAPIClass [
	^ ThreadedFFIMigration threadedFFIAvailable
		ifTrue: [ TFCalloutAPI ]
		ifFalse: [ super calloutAPIClass ]
]

{ #category : #'*ThreadedFFIMigration' }
LGitLibrary >> runner [ 
	^ self class environment
		at: #ThreadedFFIMigration
		ifPresent: [ :aThreadedFFIMigration |
			aThreadedFFIMigration threadedFFIAvailable
				ifTrue: [ (self class environment at: #TFSameThreadRunner) uniqueInstance ]
				ifFalse: [ super runner ] ]
		ifAbsent: [ super runner ]
]

{ #category : #'*ThreadedFFIMigration' }
LGitLibrary >> safeCalloutAPIClass [
	^ ThreadedFFIMigration threadedFFIAvailable
		ifTrue: [ TFLGitSafeFFICalloutAPI ]
		ifFalse: [ LGitSafeFFICalloutAPI ]
]
