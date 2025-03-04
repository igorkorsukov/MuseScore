


HERE="$(dirname ${BASH_SOURCE[0]})"

TMP_DIR=$HERE/temp
BIN_DIR=$HERE/bin

WHISPER_SRC_DIR=$TMP_DIR/whisper.cpp
WHISPER_MODEL=$BIN_DIR/ggml-medium.bin

LLAMA_SRC_DIR=$TMP_DIR/llama.cpp

mkdir -p $TMP_DIR
mkdir -p $BIN_DIR

if [ ! -f "$BIN_DIR/whisper-cli" ]; then

    echo "=== Get Whisper ==="

    if [ ! -d "$WHISPER_SRC_DIR" ]; then
        echo "=== Clone Whisper ==="
        git clone https://github.com/ggerganov/whisper.cpp.git $WHISPER_SRC_DIR
    fi

    if [ ! -f "$WHISPER_SRC_DIR/build/bin/whisper-cli" ]; then
        echo "=== Build Whisper ==="
        cmake -S $WHISPER_SRC_DIR -B $WHISPER_SRC_DIR/build
        cmake --build $WHISPER_SRC_DIR/build --config Release
    fi

    cp $WHISPER_SRC_DIR/build/bin/whisper-cli $BIN_DIR/whisper-cli

fi

if [ ! -f "$WHISPER_MODEL" ]; then

    if [ ! -d "$WHISPER_SRC_DIR" ]; then
        echo "=== Clone Whisper ==="
        git clone https://github.com/ggerganov/whisper.cpp.git $WHISPER_SRC_DIR
    fi

    bash $WHISPER_SRC_DIR/models/download-ggml-model.sh medium $BIN_DIR
fi

# if [ ! -f "$BIN_DIR/llama-cli" ]; then

#     echo "=== Get Llama ==="

#     if [ ! -d "$LLAMA_SRC_DIR" ]; then
#         echo "=== Clone Lamma ==="
#         git clone https://github.com/ggerganov/llama.cpp.git $LLAMA_SRC_DIR
#     fi

#     if [ ! -f "$LLAMA_SRC_DIR/build/bin/llama-cli" ]; then
#         echo "=== Build Lamma ==="
#         cmake -S $LLAMA_SRC_DIR -B $LLAMA_SRC_DIR/build
#         cmake --build $LLAMA_SRC_DIR/build --config Release
#     fi

#     cp $LLAMA_SRC_DIR/build/bin/llama-cli $BIN_DIR/llama-cli

# fi



echo "=== Install js dependencies ==="
npm install