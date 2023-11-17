# pyoxquic

Python wrapper for oxen-libquic quic-based communication library

## Dependencies

- python3-pip
- python3-dev $\geq$ 3.7
- pybind11-dev $\geq$ 2.10.3
- scikit-build $\geq$ 0.13.0
- libquic-dev


## Building from Source

Check out the source code with:

```
git clone https://github.com/oxen-io/oxen-pyoxquic.git
cd oxen-pyoxquic
```

### CMake Build

A few CMake options have been exposed for granular configuration:
- `FORCE_PYBIND11_SUBMODULE` -- Force downloading pybind11 as a submodule over finding a local install; defaults to OFF
- `INSTALL_OXQUIC` -- Install as a python module upon successful compilation; defaults to OFF
- `OXQUIC_WARN_ERRORS` -- Will treat all warnings as errors for development use; defaults to ON for debug builds (or if `-DCMAKE_COMPILE_WARNING_AS_ERROR` is passed), OFF otherwise

To compile, execute the following commands from the project root:

```
mkdir -p build && cd build
cmake .. -GNinja \
    -DCMAKE_CXX_COMPILER="clang++" \
    -DCMAKE_BUILD_TYPE=Release
ninja -v
```

### Python Module Build

Two different methods can be executed to build the python module. For local development use, `pipx` can be used to test the install in a one-time, temporary environment. From the project root, run:
```
pipx run build
```

Otherwise, a system install can be performed as:
```
pip install .
```
