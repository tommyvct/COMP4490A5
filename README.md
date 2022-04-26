# COMP4490 Assignment 5
Tested on openSUSE Tumbleweed with clang14 with LLVM backend, and Windows 10 21H2 with Visual Studio 2022.

## Structure
- `glsl/`: GLSL shader codes
- `src/`: C++ source codes
- `img/`: screenshots

The rests are dependencies.


## Features
### Completed
- Intersection tests for spheres, planes, and triangles (meshes). (May have bug)
- Lighting calculated using Phong reflection (Phong specular + Lambertian diffuse).
- Ambient, point, directional, and spot lights.
- Shadow testing. (Have bug)
- Mirror reflection and transparency
- 2 extra scene
### Partially Completed
- refraction and total internal reflection (very buggy)



## Build instructions
For all platforms:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

To run the compiled executables:
```bash
# change to parent directory (where CMakeLists.txt is) 
cd ../
# run the executable from here
../build/Assignment5
```

### OpenMP Acceleration
Optional OpenMP acceleration can be turned off by passing `-DOPENMP=OFF` to CMake.
It may cause render artifacts.

### Windows Specific
- The dependencies have been upgraded to 64-bit
- Make sure the startup project is assigned to a project other `ALL_BUILD` and `ZERO_CHECK`.
- Release build configuration can significantly speed up render speed.

### macOS Specific
CMake cannot set the working directory for Xcode debugging. To manually set the working directory for debugging, go to `Product` -> `Scheme` -> `Edit Scheme`, under `Options` tab, tick `Use custom working directory` and set it to where the GLSL shaders are.

### Linux/Unix Specific
OpenGL related libraries are to be installed via package manager first.

For Ubuntu, the following packages need to be installed:
```
libglu1-mesa-dev freeglut3-dev mesa-common-dev libglew-dev
```

`libomp` only works with LLVM Clang toolchain instead of commonly used gcc toolchain.
`libomp` support OpenMP version 5 while gcc implementation `libgomp` supports up to OpenMP version 4.5.