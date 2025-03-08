

curl http://localhost:11434/api/chat -d '{
  "model": "deepseek-r1",
  "messages": [
    {
        "role": "system",
        "content": "Express it as a command with format: {command}?arg1=value1&arg2=value2&argn=valuen. Output only the result"
    },
    {
      "role": "user",
      "content": "Add a note B"
    }
  ]
}'