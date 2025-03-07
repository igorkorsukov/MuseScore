
import { ChatOllama } from "@langchain/ollama";
import { HumanMessage, SystemMessage } from "@langchain/core/messages";
import { z } from "zod";

const command = z.object({
    command: z.string().describe("The command without spaces"),
    args: z.string().describe("Arguments of command in the format: arg1=val1&arg2=val2")
});

// deepseek-r1:8b
// llama3.2:3b
// llama3.1:8b

const model = new ChatOllama({
    model: "llama3.1:8b",
    temperature: 0,
    numGpu: 16
    //keepAlive: 0
  });

const structuredLlm = model.withStructuredOutput(command);  

const sysMsg = new SystemMessage("First translate into English, if not English. \
    Context is musical notation. \
    Express the expression as a command with arguments. \
    All names are in lower case. \
    Output only the command. \
    Example: \
    Open the latest project -> command: open_project, args: name=last \
    Move cursor to right -> command: move_cursor, args: direction=right \
    Move cursor to left -> command: move_cursor, args: direction=left \
    First bar -> -> command: move_cursor, args: direction=first_bar \
    Next bar -> -> command: move_cursor, args: direction=next_bar \
    Previous bar -> -> command: move_cursor, args: direction=prev_bar \
    Next element -> command: move_cursor, args: direction=next_element \
    Previous element -> command: move_cursor, args: direction=prev_element \
    Add a note c -> command: add_note, args: note=c \
    Add a note a to the bar 2 -> command: add_note, args: note=a&bar=2 \
    Pitch up -> command: change_pitch, args: direction=up \
    Pitch down -> command: change_pitch, args: direction=down \
    Play -> command: play, args: '' \
    ")


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

function parse_args(args) {
    let ret = {}
    const pairs = args.split('&');
    pairs.forEach(pair => {
        const kv = pair.split('=')
        if (kv.length == 2) {
            ret[kv[0]] = kv[1]
        } else {
            console.log("bad arg:", pair)
        }
    });

    return ret;
}

function muse_action(cmd) {

    const command = cmd.command.toLowerCase()
    const args = cmd.args.toLowerCase()
    const aobj = parse_args(args)

    switch(cmd.command) {
        case "move_cursor": {
            let direction = aobj["direction"]
            direction = direction.replace("bar", "measure")
            return "ai://move_cursor?direction=" + direction
        }
    }

    // default
    let act = "ai://" + command
    if (args.length > 0) {
        act += "?" + args
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
