#!/bin/bash

API_KEY="YOUR_API_KEY"

curl \
  -X POST 'https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key='${API_KEY} \
  -H 'Content-Type: application/json' \
  -d @<(echo '{
  "contents": [
    {
      "parts": [
        {
          "text": "Output:\nThe output must be formatted as JSON text. For example the following are valid outputs:\n- {\"name\" : \"request_payload\",\"quality\": \"GOOD\", \"explanation\" : \"\", \"alternative\" : \"\"}\n- {\"name\" : \"wc\", \"quality\": \"BAD\", \"explanation\" : \"The variable name is too short\", \"alternative\" : \"word_count\"}\n- {\"name\" : \"s\",\"quality\": \"BAD\", \"explanation\" : \"Very short names are reserved to indexes\", \"alternative\" : \"size\"}\n\nContext:\nQuality can only be one of the following:\n- GOOD\n- NEUTRAL\n- BAD\n\nConsider the following C++ code when deciding the quality of a variable name:\nint main()\n{\nfor(auto i = 0; i &lt; 10; ++i)\nstd::cout &lt;&lt; i ;\n}\n\nAdditionally consider those rules:\n- Variable names should be explicit\n- Variable names should be coherent with their usage\n- Single letter variables names are forbidden, except if they represent counters.\n\nInput:\nDoes the variable  ﻿i follow the rules?\nDoes it have a good name within the context of the provided code?\nIf the quality is BAD or NEUTRAL, always propose an alternative name."
        }
      ]
    }
  ],
  "generationConfig": {
    "temperature": 0.9,
    "topK": 1,
    "topP": 1,
    "maxOutputTokens": 2048,
    "stopSequences": []
  },
  "safetySettings": [
    {
      "category": "HARM_CATEGORY_HARASSMENT",
      "threshold": "BLOCK_MEDIUM_AND_ABOVE"
    },
    {
      "category": "HARM_CATEGORY_HATE_SPEECH",
      "threshold": "BLOCK_MEDIUM_AND_ABOVE"
    },
    {
      "category": "HARM_CATEGORY_SEXUALLY_EXPLICIT",
      "threshold": "BLOCK_MEDIUM_AND_ABOVE"
    },
    {
      "category": "HARM_CATEGORY_DANGEROUS_CONTENT",
      "threshold": "BLOCK_MEDIUM_AND_ABOVE"
    }
  ]
}')
