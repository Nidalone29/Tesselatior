# Tesselatior

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/Nidalone29/Tesselatior/windows-build.yml?style=flat-square&label=Windows%20Build) ![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/Nidalone29/Tesselatior/unixlike-build.yml?style=flat-square&label=Unixlike%20Build)

C++ 3D application that features mesh subdivision surface algorithms, phong tessellation, displacement maps, and much more. Implemented using a pointer-based Halfedge data structure.

![Screenshot_2025-06-07_02-02-06](https://github.com/user-attachments/assets/dd3d1c7b-c786-4d4e-9d66-6807932552a2)

Originally developed for the computer graphics course at the University of Milan - Bicocca, it is now serving as a playground to experiment with computer graphics, C++ design patterns implementations, and parallel algorithms.

## Usage
### Movement

- `W A S D` classic movement forward and laterally
- `SPACEBAR` move vertically up
- `LSHIFT` move vertically down
- **`O` toggle mouse** (The letter, not the digit)

## Features

- OpenGL renderer with the Phong reflection model
- Import of assets via Assimp and textures via stb
- UI controls with ImGui
- Pointer based Halfedge data structure to manage mesh data
- Phong Tessellation
- Subdivision surface algorithms (Loop, Catmull-Clark, sqrt3)
- Displacement maps

## Roadmap

- [ ] Finish refactoring the importer code
- [ ] Improve the performance of the subdivision surface algorithms to real-time (with Gregory patches? with parallelization?)
- [ ] Create a headless version to ease testing and profiling
- [ ] Support importing meshes at runtime
- [ ] Implement terrain rendering
- [ ] Implement progressive meshes

## Build

Build instructions in `BUILD.md`
