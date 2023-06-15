# Parallel Computing

## How to run

### CPU programs

```commandline
mkdir build
cd build
cmake ..
cmake --build .
```

### GPU program

You may want to use `nvidia-run.ps1` script before running `nvcc` compiler. Be sure to set up PATH variables in the
script before running it.

```commandline
mkdir build
cd build
mkdir cuda
cd cuda
nvcc ../../src/cuda_program.cu -o cuda_program
```

### Generate CSV

```commandline
python3 generate.py
```

### Run

```commandline
.\run.ps1
```

### Run Jupyter Notebook
```commandline
python -m pip install requirements.txt
```
