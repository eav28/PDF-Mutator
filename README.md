# PDF Mutator

A model-based custom mutator for AFL++ to handle PDF files.

Requirements:
- AFL++
- QPDF

Install:

```
git clone --recurse-submodules https://github.com/eav28/PDF-fuzzer.git
```

How to build:

```
mkdir build
cd build
cmake ..
make
```

How to run:

```
export AFL_CUSTOM_MUTATOR_LIBRARY="/PATH/TO/libdata.so;/PATH/TO/libstructure.so"
export AFL_CUSTOM_MUTATOR_ONLY=1
```

Use the PDF-Preprocessing tool:

```
./pdf-preprocess [input] [output] -- /path/to/target_app [ ... ]
```

Then run AFL++ as normal.

## ⚠️ Take note:
The mutator is not in a very user friendly state at the moment as the CMake file has been hard-coded to have the AFL++ library and QPDF library in the parent directory. So the codebase is in need of a cleanup.

This might be updated in the future.
