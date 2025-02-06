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

2. **Run CMake configuration**

You can run CMake configuration by executing
```Bash
cd flight_planning
cmake -B build
# or cmake -B build -G <your_generator> if you wish to provide an alternative generator
```

3. **Build the targets**

You can build the targets by running `cmake --build build --parallel`

