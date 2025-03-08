import {llm} from "./llm.js"

let res

res = await llm.interpret("Add a note b")
console.log(res)

res = await llm.interpret("Add a note d to bar 2")
console.log(res)

res = await llm.interpret("Play")
console.log(res)

res = await llm.interpret("Move cursor to top")
console.log(res)

res = await llm.interpret("Move cursor to left")
console.log(res)