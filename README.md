# PaintBucket V0.1

## Build
You can either download the [binary](release/runtime/Paint.exe) or build using CMake. To build, just run `cmake --preset release` from the project root folder and run `cmake --build ./release`. 

### Current progress
- Load an image
- Drag and zoom into image
- Buttons for fitting to screen or centering

### TODO
- Color picker
- Floodfill
- Floodfill threshold slider
- Real-time floodfilling update when changing thresholds
- GPU implmentation (OpenCL?) for faster performance
