
import { ChatOllama } from "@langchain/ollama";
import { HumanMessage, SystemMessage } from "@langchain/core/messages";
import process from 'node:process';

// deepseek-r1:8b
// llama3.2:3b
// llama3.1:8b

const model = new ChatOllama({
    model: "llama3.1:8b",
    temperature: 0,
    numGpu: 16
    //keepAlive: 0
  });

const sysMsg = new SystemMessage("Translate into english.")


async function llm_invoke(text) {


    console.time("llm");

    const messages = [
        sysMsg,
        new HumanMessage(text)
    ]

    const ret = await model.invoke(messages)

    console.timeEnd("llm")

    console.log(ret.content)
}   

await llm_invoke(process.argv[2])
