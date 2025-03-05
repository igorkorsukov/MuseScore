import { exec } from 'child_process';

const WHISPER_CLI = "bin/whisper-cli"
const MODEL = "bin/ggml-medium.bin"
//const MODEL = "bin/ggml-base.bin"

export var asr = {

    transcribe: async function(wavFile) {
    
        let cmd = WHISPER_CLI + " -l auto --no-prints --no-timestamps -m " + MODEL + " -f " + wavFile
        console.log(cmd)

        // bin/whisper-cli -l en --no-prints --no-timestamps -m bin/ggml-medium.bin -f /tmp/org.muse/aiserver/asrfile.wav

        return new Promise(function(resolve, reject) {
            console.time("asr");
            exec(cmd, (error, stdout, stderr) => {
                console.timeEnd("asr");
                console.log(stderr.trim())
                if (error) {
                    reject(error);
                    return;
                }

                resolve(stdout.trim());
            });
        });
    }
}