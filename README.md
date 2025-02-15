# Depth of Field

![](doc/depth_demo.gif)

This project emulates the lens effect "depth of field" with an Intel Realsense depth sensor and OpenGL shaders.

## Run

Download the [latest release](https://github.com/starmaid/depth-of-field-cpp/releases/latest). Connect your supported Intel Realsense device and run `dof.exe`. 

Camera must have depth and RGB. All my testing was done on a used D435 I got for like $160 in 2023. I also reccommend using the Realsense viewer to [calibrate the camera](https://starmaid.github.io/projects/intel-depth-camera/), if you haven't already done so.

| Keypress | Action |
| --- | --- |
| `q` | Exit application |
| `w` | Increase focus distance |
| `s` | Decrease focus distance |
| `e` | Increase focal width |
| `d` | Decrease focal width |
| `r` | Increase blur intensity |
| `f` | Decrease blur intensity |

Note that the depthbuffer is dynamically scaled between the maximum and minimum distance sensed (kind of like how a thermal camera does). This behavior should probably be changed but I haven't done it yet.

The width also depends on these values so will be dynamic based on the camera's perception.

![](doc/pen.gif)

![](doc/bed_static.gif)

## Build

Its a cmake project. You need to have the realsense api installed, and also you need to install an openGL handler. See the cmakelists.txt file for details. TODO: document this more. Also make it multi platform.

## History

Read the full blog post if you want to know more.

[Impossible f0.3 lens](https://www.youtube.com/watch?v=-4uzyhbDFas)

[Acerola's video on video game depth of field](https://www.youtube.com/watch?v=v9x_50czf-4)

[Acerola's code repo](https://github.com/GarrettGunnell/AcerolaFX/blob/main/Shaders/AcerolaFX_BokehBlur.fx)


## Notes for Development

### OpenCV

https://github.com/opencv/opencv.git

https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

https://github.com/marketplace/actions/setup-opencv

https://github.com/Dovyski/setup-opencv-action/blob/master/index.js

wait actually. you can just ask cmake to find your system install of opencv-which is i think simpler?

```
C:\tools\opencv\build\x64\vc16\lib
C:\tools\opencv\build\x64\vc16\bin
```

```
cmake -S opencv -B opencv/build

make -j10 -C opencv/build

make -C opencv/build install --prefix ./fob/include
```

### Realsense

https://github.com/IntelRealSense/librealsense.git

https://github.com/zivsha/librealsense/blob/getting_started_example/examples/getting-started/CMakeLists.txt

```
C:\Program Files (x86)\Intel RealSense SDK 2.0\lib
C:\Program Files (x86)\Intel RealSense SDK 2.0\include
```

### OpenGL

Maybe need to get openGL tests but its possible itll build without it

https://amiralizadeh9480.medium.com/how-to-run-opengl-based-tests-on-github-actions-60f270b1ea2c

```
C:\tools\glew-2.1.0\include\GL
C:\tools\glew-2.1.0\lib\Release\x64
C:\tools\glew-2.1.0\bin\Release\x64
```

https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html

https://learnopengl.com/Getting-started/Hello-Triangle

https://learnopengl.com/Getting-started/Shaders

https://github.com/nothings/stb/blob/master/stb_image.h

https://learnopengl.com/In-Practice/2D-Game/Postprocessing

### HLSL Cross compile

Look into it.