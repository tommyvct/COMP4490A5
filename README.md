# COMP4490 Assignment 5
Tested on openSUSE Tumbleweed with clang14. 

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
### Partially Completed
- refraction and total internal reflection (very buggy)
### Not Implemented
- 2 extra scene


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
# change to glsl directory 
cd ../
# run the executable from here
../build/Assignment5
```

### OpenMP Acceleration
OpenMP acceleration can be turned off by passing `-DOPENMP=OFF` to CMake.
It may cause render artifacts.

### Windows Specific
- The dependencies have been upgraded to 64-bit
- Make sure the startup project is assigned to a project other `ALL_BUILD` and `ZERO_CHECK`.

### macOS Specific
CMake cannot set the working directory for Xcode debugging. To manually set the working directory for debugging, go to `Product` -> `Scheme` -> `Edit Scheme`, under `Options` tab, tick `Use custom working directory` and set it to where the GLSL shaders are.

## Linux/Unix Specific
OpenGL related libraries are to be installed via package manager first.

For Ubuntu, the following packages need to be installed:
```
libglu1-mesa-dev freeglut3-dev mesa-common-dev libglew-dev
```

<!DOCTYPE html>
<html><head>
</head>
<body><!--  style="color: #494c4e;"> -->
<h1 id="comp-4490-assignment-5" style="background-color: #a0a0f0; border-bottom: thin solid; color: #323273; font-family: Helvetica, Arial, sans-serif; font-size: x-large; font-weight: bold; margin: 0; padding: 12px; text-align: center; width: 100%;" bgcolor="#A0A0F0" align="center" width="100%">COMP 4490 Assignment 5</h1>
<h2 id="material-covered" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Material covered:</h2>
<ul>
<li>shading</li>
<li>ray tracing</li>
<li>global illumination</li>
</ul>
<h2 id="notes" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Notes:</h2>
<ul>
<li>For all programs, use C/C++ with freeglut/glew/glm.</li>
<li>Include a text <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">README</code> file that describes the platform you used to test your programs (operating system and version, graphics hardware), and notes to the marker for each question (e.g. things that don't work as expected or things that are better than required).</li>
<li>Name your source files, shaders, and window title appropriately.</li>
<li>Electronically submit a copy of your solutions in a single <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">.zip</code> file.</li>
<li>Your assignment submission <strong>must</strong> include all dependencies, and build and run correctly as downloaded using either a Makefile or a Visual Studio solution. You can submit just sources, or a complete solution, but ensure that you have submitted everything you need to run either by itself, or when merged into the sample project.</li>
<li>You must fill in an <strong>honesty declaration</strong> for each assignment before you can access the assignment dropbox.</li>
<li>You must accept and follow all of the academic integrity requirements for the course, including the Expectations for Individual Work in Computer Science</li>
<li><strong>NOTE: For this assignment, you will also submit screenshots of your raytraced scenes. See the assignment description for details.</strong></li>
<li>Note that this assignment is entirely individual work.</li>
</ul>
<h2 id="question-1-ray-tracer" style="border-bottom-color: #408; border-bottom-style: solid; border-bottom-width: 2px; font-family: Helvetica, Arial, sans-serif; font-size: large; font-weight: bold;">Question 1: Ray Tracer</h2>
<p>Write an OpenGL program that will draw ray traced scenes. It should implement the following features:</p>
<ul>
<li>Intersection tests for spheres, planes, and triangles (meshes).</li>
<li>Lighting calculated using Phong reflection (Phong specular + Lambertian diffuse).</li>
<li>Ambient, point, directional, and spot lights.</li>
<li>Shadow testing.</li>
<li>Mirror reflection and transmission (simple transparency and refraction).</li>
</ul>
<p>Scenes will be rendered based on a fixed camera positioned at the origin. The view plane is parallel to the <em>xy</em> plane and at a distance of 1 along the negative z axis. You will be given a set of scenes to render in a JSON format, along with a JSON library and demonstration code on how to process the scenes in that input format. If you prefer, you can preprocess the given scenes and "hard-code" them into your implementation's data structures.</p>
<p>Your ray tracer must:</p>
<ul>
<li>Be launched from the command line with a single lower-case letter command line argument indicating the name of the scene to render; e.g. <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">../build/q1 a</code> will render scene <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a</code> (matching the input file <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a.json</code>). If you are using Visual Studio, you can <a href="http://cs-people.bu.edu/deht/CS585/VSTutorial/#CommandLineArgs">add command line arguments to the debug configuration</a>. Running once will render one scene.
<ul>
<li>If there is no command line argument, the given code will render scene <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">c.json</code>, though you can change that.</li>
</ul>
</li>
<li>Render using the camera, lights, and scene corresponding to that input file (either by reading the file, or by using a "hard-coded" technique). If you are reading files, they should be located in a <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">scenes</code> subdirectory relative to the source code.</li>
<li>Allow you to click on points in the window to cast a single ray into the scene, and print out relevant debugging information (like intersections). The exact output format doesn't matter.</li>
</ul>
<p>If you use the posted <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a5_skeleton.zip</code> skeleton, the most important source code file is <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">raytracer.cpp</code>. It contains the following functions that you need to implement:</p>
<ul>
<li><code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">void choose_scene(char *fn)</code> - choose to render the scene corresponding to the input file <em><code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">fn</code></em><code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">+".json"</code>.</li>
<li><code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">bool trace(const point3 &amp;e, const point3 &amp;s, colour3 &amp;colour, bool pick)</code> - trace a single ray from the eye position <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">e</code> (for this assignment, this will always be the origin) through the viewplane at point <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">s</code> (which, for this assignment, always has a <em>z</em> value of -1). Store the resulting output colour for the fragment in <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">colour</code>. Return <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">true</code> if it hit an object (if it is <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">false</code> the output colour should be the background colour). If <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">pick</code> is true, then the user is interactively picking, so print additional debug information.</li>
</ul>
<p>Or you can use any part of the skeleton code, as much as you need. You can add as many additional source files to your project as you like.</p>
<p>Your submission must also include:</p>
<ul>
<li>a <strong>complete <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">README</code></strong> file that describes all the features completed, partially completed, missing, or anything that your ray tracer does beyond the requirements;</li>
<li><strong>two scenes</strong> of your own creation, named <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a</code> and <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">b</code> (or <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a.json</code> and <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">b.json</code>). Each of these scenes should show off the features of your implementation, and should include a variety of geometry and lighting; and</li>
<li><strong>screenshots of all the scenes</strong> <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a</code> through <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">i</code>, rendered using the default settings/window size. The filenames of the screenshots should match the scene names; e.g. <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a.jpg</code> or <code style="background-color: #ffffe8; font-family: Consolas, Courier, 'Courier New', monospace;" bgcolor="#FFFFE8">a.png</code> and so on.</li>
</ul>
</body></html>