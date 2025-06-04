# Building instuctions

1) get inside the Tesselatior directory `cd Tesselatior`
2) pull vcpkg with `git submodule update --init`
3) install dependencies based on your system if you need them
   1) **glfw** system deps
      - If you are on windows you can use Visual Studio, Clang in Visual Studio or MINGW directly
      - If you are on MACOS you can use XCode directly
      - more information on <https://www.glfw.org/docs/latest/compile_guide.html#compile_deps_x11>
      - MACOS: `brew install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config`
      - Debian: `sudo apt install xorg-dev`
        - Wayland: `sudo apt install libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules`
      - Red Hat: `sudo dnf install libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel`
        - Wayland: `sudo dnf install wayland-devel libxkbcommon-devel wayland-protocols-devel extra-cmake-modules`
      - FreeBSD: `pkg install xorgproto`
        - Wayland: `pkg install wayland libxkbcommon wayland-protocols kf5-extra-cmake-modules`

   2) **glew** system deps
      - More information on <https://www.glfw.org/docs/latest/compile_guide.html#compile_deps>
      - Debian: `sudo apt-get install libxmu-dev libxi-dev libgl-dev`
  
4) run `cmake --list-presets` to check your available presets
5) execute your desired preset with `cmake --preset "your-preset-name"`
6) build the project with `cmake --build "./build/your-preset-name"`
   1) the build command should also copy every asset automatically. If not you can simply run the custom targets before.
7) the executable `Tesselatior.exe` is in the build directory

 Cold Compilation time: ~8/10 min

 After initial CMake Cache build the compile time is a few seconds at max
