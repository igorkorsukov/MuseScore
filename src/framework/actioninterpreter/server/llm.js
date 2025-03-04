
import { ChatMistralAI } from "@langchain/mistralai";
import { ChatOllama } from "@langchain/ollama";
import { HumanMessage, SystemMessage } from "@langchain/core/messages";
import { z } from "zod";

const command = z.object({
    command: z.string().describe("The command without spaces"),
    args: z.string().describe("Arguments of command in the format: arg1=val1&arg2=val2")
});

// const model = new ChatMistralAI({
//   model: "mistral-large-latest",
//   temperature: 0.1
// });

// deepseek-r1:8b
// llama3.2:3b
// llama3.1:8b

const model = new ChatOllama({
    model: "llama3.1:8b",
    temperature: 0.1
  });

const structuredLlm = model.withStructuredOutput(command);  

const sysMsg = new SystemMessage("First translate into English, if not English. \
    Express the expression as a command with arguments. \
    All names are in lower case. \
    Example: \
    Add a note c -> command: add_note, args: note=c \
    Add a note a to the bar 2 -> command: add_note, args: note=a&bar=2 \
    Play -> command: play, args: '' \
    Move cursor to right -> command: move_cursor, args: direction=right \
    Output only the command")


async function llm_invoke(text) {

    console.time("llm");

    const messages = [
        sysMsg,
        new HumanMessage(text)
    ]

    const ret = await structuredLlm.invoke(messages)

    console.timeEnd("llm")

    return ret
}   

function muse_action(cmd) {

    switch(cmd.command) {
        case "next_element": return "ai://notation/next_element"
    }

    // default
    let act = "ai://" + cmd.command
    if (cmd.args.length > 0) {
        act += "?" + cmd.args
    }
 
    return act;
}

const responce_template = {
    text: "",
    interpret: {
        command: "",
        args: []
    },
    action: ""
}

export var llm = {

    interpret: async function (text) {

        let res = responce_template
        res.text = text
        res.interpret = await llm_invoke(text)
        res.action = muse_action(res.interpret) 
        return res
    }
}    
