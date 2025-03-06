import http from "http";
import os from 'node:os';
import fs from 'node:fs';

import {asr as asr_cli} from "./asr-cli.js";
import {asr as asr_server} from "./asr-server.js";
import {llm} from "./llm.js"
import { exit } from "node:process";

const TMP_DIR = os.tmpdir() + "/org.muse/aiserver"
const ASR_TMP_FILE = TMP_DIR + "/asrfile.wav"

if (!fs.existsSync(TMP_DIR)){
    fs.mkdirSync(TMP_DIR, {recursive: true});
}
 
const server =  http.createServer(function(req, res) {

    console.log(req.url)

    if (req.url === "/asr") {
        var body = '';
        req.on('data', function (data) {
            body += data;
        });
    
        req.on('end', function () {
            let pack = JSON.parse(body);
            onAsrRequest(pack, res)
        });
    }
});

server.listen(2212);

async function onAsrRequest(pack, res)
{
    console.log("reqID: ", pack.reqID);

    // save wav file
    {
        let wavFile = Buffer.from(pack.wavFile, 'base64')
        console.log("wavFile: " + wavFile.length)
        fs.writeFileSync(ASR_TMP_FILE, wavFile)
    }

    // asr
    // let text1 = await asr_cli.transcribe(ASR_TMP_FILE)
    // console.log("asr-cli:", text1);
    let text2 = await asr_server.transcribe(ASR_TMP_FILE)
    //console.log("asr-server:", text2);

    // llm
    let cmd = await llm.interpret(text2)
    console.log(cmd);

    // responce
    res.end(JSON.stringify(cmd))
}