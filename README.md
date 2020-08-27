# Welcome to MathG!
Ultra-light GPU-accelerated **extendable** mathematics library. Developed mostly for [NeuroG](https://github.com/Meta-chan/NeuroG).

### Platforms
Compiled and tested on Windows x86/x64, compiled on Linux x86. In perspective can be ported on every platform that provides OpenGL API.

### Usage
The installation process is integrated with [Ironic Library](https://github.com/Meta-chan/ironic_library), follow the link for more. But in a nutshell, you can do just this:
```c++
#define IR_IMPLEMENT
#include <mathg.h>
```

### Dependencies and Building options
At the moment MathG depends on [GLEW](http://glew.sourceforge.net), [SDL](https://www.libsdl.org) or [FreeGlut](http://freeglut.sourceforge.net) or [EGL](https://www.khronos.org/egl/), and [Ironic Library](https://github.com/Meta-chan/ironic_library).

#### Linux
 - For all cases:
   - Install libGL-dev, glew2-dev
   - Compile with -lGL -lGLEW -I ironic_library
 - With SDL:
   - Install SDL2-dev
   - Compile with -lSDL2
 - With EGL:
   - Install libEGL-dev, libX11-dev, expat2
   - Compile with -lEGL
 - With Freeglut:
   - Install freeglut-dev
   - Compile with -lglut

 (tested on Tiny Core Linux 11 & g++)

#### Windows
 - For all cases:
   - Download [Ironic Library](https://github.com/Meta-chan/ironic_library) and [GLEW](http://glew.sourceforge.net)
   - Install glew/bin/Release/(Win32/x64)/glew32.dll
   - Add Ironic Library and glew/include to include directories
   - Add glew/lib/Release/(Win32|x64) to libraries directories
   - Add (glew32.lib|glew32s.lib) to dependencies
 - With SDL:
   - Download [SDL](https://www.libsdl.org)
   - Install SDL2/lib/(x86/x64)/SDL2.dll
   - Add SDL2/include to include directories
   - Add SDL2/lib/(x86/x64) to libraries directories
   - Add SDL2.lib and SDL2main.lib to dependencies
 - With Freeglut:
   - Download [FreeGlut](http://freeglut.sourceforge.net)
   - Install Freeglut/bin/(Debug/freeglutd.dll|Release/freeglut.dll)
   - Add Freeglut/include to include directories
   - Add Freeglut/lib/(Debug|Release) to libraries directories
   - it may be needed to set subsystem to CONSOLE
 - With EGL:
   - No information (I do not know)

 (tested on Windows & Visual Studio 17)

### Documentation
The code is pretty self-documented. But more importantly, I provide [Doxygen](https://www.doxygen.nl/manual/starting.html) documentation! It does not look too pretty since I am not an expert, but it is still quite informative. And of course, feel free to contact me!

###### P.S. My code is not dirty, it is alternatively clean.