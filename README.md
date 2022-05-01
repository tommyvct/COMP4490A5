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












<!DOCTYPE html>
<html>
<head>
</head>
<body>
<h1 id="comp-4490-graphics-2-ray-tracing-project" style="background-color: #a0a0f0; border-bottom: thin solid; color: #323273; font-family: Helvetica, Arial, sans-serif; font-size: x-large; font-weight: bold; margin: 0; padding: 12px; text-align: center; width: 100%;"  width="100%">COMP 4490 Ray Tracing Project</h1>
<p>To have your ray tracer count as a project, add features to your ray tracing assignment from each of the categories as described below. Submit it as your project, <em>do not</em> include the additional work in your regular assignment submission.</p>
<p>In order to demonstrate some of the techniques, you will need to use new scene (JSON) input files. You can modify the files you were given in the assignment, or the files you have created, or new files you create. If you would like to generate more scenes from existing objects, please look at the posted Python script <code style="font-family: Consolas, Courier, 'Courier New', monospace;">obj2json.py</code> for converting <a href="https://en.wikipedia.org/wiki/Wavefront_.obj_file">OBJ files</a> to our JSON meshes.</p>
<p>If you have any other ideas for a technique to implement outside of the given list, please contact your instructor. A single "complex enough" feature may be considered a project, though it will likely require acceleration too.</p>
<p>Your submission must meet the following requirements:</p>
<ul>
<li>Use C/C++ with freeglut/glew/glm or another OpenGL environment with the consent of the instructor.</li>
<li>Include a <strong>written report</strong> that describes the features you implemented, how well they do (or do not) work, and citations of any external sources you consulted for information about those features. Remember that you can use information from those sources, but all code must be your own work.</li>
<li>Electronically submit a copy of your project (code and report) in a single <code style="font-family: Consolas, Courier, 'Courier New', monospace;">.zip</code> file.</li>
<li>Your submission <strong>must</strong> include all dependencies, and build and run correctly as downloaded using either a Makefile or a Visual Studio solution.</li>
<li>Include enough <strong>scenes</strong> (JSON files) along with output (<strong>screenshots</strong> or <strong>videos</strong> of the renderings from those files) to demonstrate all of the features of your ray tracer. In your report, list the <strong>rendering time</strong> (minutes and seconds) of each of the scenes on your system.</li>
<li>Please consult with the instructor if you would like to make this a group project (at most two students per group). You will need to implement more features.</li>
</ul>
<h2 id="feature-1-acceleration" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Feature #1: Acceleration</h2>
<p>Include <strong>one</strong> of the following acceleration features:</p>
<ul>
<li>Scanline assistance for rendering (use scanline rendering for a fast object hit test).</li>
<li>Hybrid scanline-raytrace rendering (render the scene using scanline rendering, and then raytrace only objects that require it).</li>
<li>Implement one acceleration data structure; either:
<ul>
<li>two-level nested grids; <em>or</em></li>
<li>octrees; <em>or</em></li>
<li>BSP trees; <em>or</em></li>
<li>BVHs.</li>
</ul>
</li>
</ul>
<p>Note that the first two options will require a lot of work on the shaders. The third option can be implemented entirely in application (C/C++) code. This feature is also a prerequisite for many of the other features; without acceleration your ray tracer will be unable to completely render substantially more rays and/or objects.</p>
<h2 id="feature-2-transformations" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Feature #2: Transformations</h2>
<p>Implement geometry transformations in your ray tracer. This could be <strong>either</strong>:</p>
<ul>
<li>allow the camera position and direction to change (add lookat to the camera); <em>or</em></li>
<li>add a transformation to meshes that will be applied before rendering (including scale, rotation, and translation). Rotation can be done with Euler angles or quaternions. Note that the GLM library implements quaternions.</li>
</ul>
<p>Demonstrate this feature in your output by either rendering the same scene using different camera/object transformations (include screenshots for each, describing the transformation used), or by using a moving camera/object to generate scenes from an animation (make a movie).</p>
<h2 id="feature-3-improved-quality" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Feature #3: Improved Quality</h2>
<p>Implement <strong>two</strong> of the following features to improve output quality:</p>
<ul>
<li>Add texture mapping support for meshes, using a texture read from an image. The image reading code can be very simple (use <code style="font-family: Consolas, Courier, 'Courier New', monospace;">BMP</code> files) or you can include third-party code or a library (include the library and/or <strong>cite any code used</strong>). You will need to add <em>(u,v)</em> coordinates to the mesh data in the JSON file.</li>
<li>Add <a href="https://en.wikipedia.org/wiki/Bump_mapping">bump mapping</a> or <a href="https://en.wikipedia.org/wiki/Displacement_mapping">displacement mapping</a>. See the note above regarding image reading code.</li>
<li>Add <a href="https://en.wikipedia.org/wiki/Procedural_texture">procedural texture mapping support</a>.</li>
<li>Implement <a href="https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law">Beer's Law</a> for shadow attenuation.</li>
<li>Implement an <a href="https://en.wikipedia.org/wiki/Oren%E2%80%93Nayar_reflectance_model">Oren-Nayar diffuse reflectance model</a>.</li>
<li>Implement a <a href="https://en.wikipedia.org/wiki/Specular_highlight#Ward_anisotropic_distribution">Ward anisotropic distribution or Cook-Torrance specular reflectance model</a>.</li>
<li>Implement a <em>non-photorealistic</em> shading model such as cel shading, which can be enabled for specific primitives in your scene.</li>
<li>Implement Schlick's Approximation for refraction.</li>
<li>Add refraction for one level of nested objects (e.g. you could make a "bubble" out of a sphere within a sphere).</li>
</ul>
<h2 id="feature-4-more-geometry" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Feature #4: More Geometry</h2>
<p>Implement <strong>one</strong> of the following additional geometry options:</p>
<ul>
<li>generic surfaces of revolution (like cylinders, cones, <em>and</em> toroids); <em>or</em></li>
<li>CSG objects using union, intersection, and difference of other objects; <em>or</em></li>
<li>a primitive based on a particle system (at a fixed state); <em>or</em></li>
<li>Bezier patches or some similar parametric curved surfaces.</li>
</ul>
<p>None of these need to render with transmission.</p>
<h2 id="feature-5-techniques" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Feature #5: Techniques</h2>
<p>Implement <strong>one</strong> of the following ray-tracing techniques:</p>
<ul>
<li>distribution ray tracing in order to achieve <strong>any two</strong> of these effects:
<ul>
<li>antialiasing; <em>or</em></li>
<li>depth of field; <em>or</em></li>
<li>area and/or mesh lighting; <em>or</em></li>
<li>motion blur; <em>or</em></li>
<li>glossy reflection; <em>or</em></li>
</ul>
</li>
<li>path tracing; <em>or</em></li>
<li>a Monte Carlo simulation to render a BSSRDF, <a href="http://graphics.stanford.edu/~henrik/papers/bssrdf/bssrdf.pdf">like this</a>.</li>
</ul>
</body>
</html>