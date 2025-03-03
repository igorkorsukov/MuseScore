import { exec } from 'child_process';

const WHISPER_CLI = "bin/whisper-cli"
//const MODEL = "bin/ggml-medium.bin"
const MODEL = "bin/ggml-base.bin"

export function transcribe(wavFile) {
    
    let cmd = WHISPER_CLI + " -l en --no-prints --no-timestamps --no-gpu -m " + MODEL + " -f " + wavFile
    console.log(cmd)

    // bin/whisper-cli -l en --no-prints --no-timestamps --no-gpu -m bin/ggml-medium.bin -f /tmp/org.muse/aiserver/asrfile.wav

    return new Promise(function(resolve, reject) {
        exec(cmd, (error, stdout, stderr) => {
            if (error) {
                reject(error);
                return;
            }

            resolve(stdout.trim());
        });
    });
}