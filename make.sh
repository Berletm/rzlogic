#!/bin/bash
set -e

echo "Building librzlogic"
mkdir -p build
cmake -B build -S librzlogic
cmake --build build --target rzlogic-pybind -j $(nproc)

if [ ! -d .rz.env ]; then
    echo "Creating virtual environment (.rz.env)"
    python3 -m venv .rz.env
    . .rz.env/bin/activate
    pip install --upgrade pip
    pip install setuptools
    pip install dotenv openai
    pip install .
else
    echo "Using existing virtual environment (.rz.env)"
    . .rz.env/bin/activate
fi

echo "Done! Use 'source .rz.env/bin/activate' to enter virtual environment"