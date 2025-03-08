import http from "http";
import FormData from 'form-data';
import fs from 'node:fs';
import { spawn } from 'child_process';

const WHISPER_SERVER= "bin/whisper-server"
const MODEL = "bin/ggml-large-v3-turbo.bin"

const END_POINT="http://127.0.0.1:22121/inference"

var server = null
var serverStarted = false

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

export var asr = {

    run_server: async function() {

      console.log("asr server starting...")
    
      server = spawn(WHISPER_SERVER, ['--no-timestamps', '-l', 'auto', '-m', MODEL, '--port', '22121'], {
        detached: false
      });
    
      console.log("asr server started!")
    },

    transcribe: async function(wavFile) {

        return new Promise(function(resolve, reject) {
            console.time("asr-server");

            var form = new FormData();  
            form.append("temperature", "0.0");
            form.append("response_format", "text");
            form.append("file", fs.createReadStream(wavFile));

            form.submit(END_POINT, function(err, res) {
              // res – response object (http.IncomingMessage)  //

              if (err) {
                reject(err);
                return;
              }
       
              let body = '';
              res.on('data', (chunk) => {
                  body += chunk;
              });
              res.on('end', () => {
                  console.timeEnd("asr-server");
                  resolve(body);
              });
      
              res.resume();
            });
        });
    }
}

