import http from "http";
import os from 'node:os';
import fs from 'node:fs';

import {asr} from "./asr.js"
import {llm} from "./llm.js"

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
    let text = await asr.transcribe(ASR_TMP_FILE)

    // llm
    let cmd = await llm.interpret(text)
    console.log(cmd);

    // responce
    res.end(JSON.stringify(cmd))
}