# flight_planning
A 3D-space route planning software for unmanned aerial vehicles.

### Building
#### Requirements
- Git
- CMake v3.20
- C++17 toolchain
#### Steps
1. **Clone the repository**

You can clone the repository by executing
`git clone https://github.com/leoovs/flight_planning` 

2. **Install system dependencies (Linux-only)**

* Install [SDL3 build depenencies](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies).
* Install one of the required file dialog utilities:
  - Xdialog
  - Zenity
  - YAD

If not available or installed, basic console input will be used as a source for 
file name.

3. **Run CMake configuration**

You can run CMake configuration by executing
```Bash
cd flight_planning
cmake -B build
# or cmake -B build -G <your_generator> if you wish to provide an alternative generator
```

3. **Build the editor**

You can build the editor by running `cmake --build build --parallel --target editor`.

