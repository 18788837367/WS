cd ..
mkdir build
cd build

CURRENT_DIR=$(pwd)
SRC_DIR="${CURRENT_DIR}/.."
BIN_DIR="${CURRENT_DIR}/bin"

cmake -G"MinGW Makefiles" -S ${SRC_DIR} -B ${BIN_DIR}
make-mingw32
