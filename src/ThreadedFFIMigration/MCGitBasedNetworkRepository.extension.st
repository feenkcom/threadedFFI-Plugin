Extension { #name : #MCGitBasedNetworkRepository }

{ #category : #'*ThreadedFFIMigration' }
MCGitBasedNetworkRepository >> createIcebergRepositoryWithFallbackFor: remote url: remoteUrl [
	| urlToUse credentialsRetry |
	credentialsRetry := true.
	urlToUse := remoteUrl.
	[ [ ^ self createIcebergRepositoryFor: urlToUse ]
		on: IceAuthenticationError
		do: [ :e |
			self
				crTrace:
					('I got an error while cloning: {1}. I will try to clone the HTTPS variant.'
						format: {e messageText}).
			urlToUse := remote httpsUrl.
			e retry ] ]
		on: IceGenericError
		do: [ :e | | znUrl host credentials |
			e messageText = 'Invalid username and/or password provided.' ifFalse: 
				[ e pass ].
			znUrl := urlToUse asZnUrl.
			znUrl scheme = #https ifFalse: [ e pass ].
			host := znUrl host.
			credentials := IceCredentialStore current plaintextCredentialForHostname: host.
			credentials username ifNotEmpty: [ 
				credentialsRetry ifTrue: 
					[ credentialsRetry := false.
					e retry ]
				ifFalse:
					[ e pass ] ].
			(IceCredentialsProvider defaultForRemote: (IceGitRemote url: urlToUse))
					obtainPlaintextCredentials: credentials.
			self crTrace: 'Retry with credentials'.
			e retry ]
]
