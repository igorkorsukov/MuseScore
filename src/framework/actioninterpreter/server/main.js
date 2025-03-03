import http from "http";
import os from 'node:os';
import fs from 'node:fs';

import {transcribe} from "./asr.js"

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
    console.log("wavStr: " + pack.wavFile.length)
    fs.writeFileSync(ASR_TMP_FILE+".txt", pack.wavFile)
    let wavFile = Buffer.from(pack.wavFile, 'base64')
    console.log("wavFile: " + wavFile.length)
    fs.writeFileSync(ASR_TMP_FILE, wavFile)

    console.time("asr");

    let text = await transcribe(ASR_TMP_FILE)
    console.log(text);

    let ret = {
        transcribe: text
    }

    res.end(JSON.stringify(ret))

    console.timeEnd("asr");

}