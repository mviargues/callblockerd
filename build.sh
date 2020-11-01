# Compile the callblocker deamon

set -e

echo "Compiling..."

OUTPUT_DIR='out'
if [[ ! -d ${OUTPUT_DIR} ]]; then
    mkdir ${OUTPUT_DIR}
fi

pushd src
gcc *.cpp -lstdc++ -o ../${OUTPUT_DIR}/callblockerd
popd

# DEBUG: for testing
./${OUTPUT_DIR}/callblockerd