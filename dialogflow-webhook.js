{
  'responseId': '6905e097-6741-4d78-9759-d841a11e1a6d-b4ef8d5f',
  'queryResult': {
    'queryText': 'Turn on bedroom 1 light',
    'action': 'on',
    'parameters': {
      'light': 'bedroom 1 light'
    },
    'allRequiredParamsPresent': True,
    'fulfillmentText': 'Cannot connect to Home Assistant or it is taking too long',
    'fulfillmentMessages': [
      {
        'text': {
	  'text': [
	    'Cannot connect to Home Assistant or it is taking too long'
	  ]
	}
      }
    ],
    'outputContexts': [
      {
	'name': 'projects/homeassistant-ylirdt/agent/sessions/ff51077b-2739-da3a-a344-50025b74b85b/contexts/system_counters',
	'parameters': {
          'no-input': 0.0,
	  'no-match': 0.0,
	  'light': 'bedroom 1 light',
	  'light.original': 'bedroom 1 light'
	}
      }
    ],
    'intent': {
      'name': 'projects/homeassistant-ylirdt/agent/intents/82e7b493-2397-4ee9-a8f6-41a12590cf39',
      'displayName': 'Switch on'
    },
    'intentDetectionConfidence': 1.0,
    'languageCode': 'en'
  },
  'originalDetectIntentRequest': {
    'payload': {}
  },
  'session': 'projects/homeassistant-ylirdt/agent/sessions/ff51077b-2739-da3a-a344-50025b74b85b'
}
