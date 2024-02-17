# meta docs for c++ version of depth of field

I need to include some libraries

https://github.com/opencv/opencv.git

https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

https://github.com/marketplace/actions/setup-opencv

https://github.com/Dovyski/setup-opencv-action/blob/master/index.js

wait actually. you can just ask cmake to find your system install of opencv-which is i think simpler?

C:\tools\opencv\build\x64\vc16\lib
C:\tools\opencv\build\x64\vc16\bin

cmake -S opencv -B opencv/build

make -j10 -C opencv/build

make -C opencv/build install --prefix ./fob/include

https://github.com/IntelRealSense/librealsense.git


https://github.com/zivsha/librealsense/blob/getting_started_example/examples/getting-started/CMakeLists.txt


C:\Program Files (x86)\Intel RealSense SDK 2.0\lib
C:\Program Files (x86)\Intel RealSense SDK 2.0\include



Maybe need to get openGL tests but its possible itll build without it

https://amiralizadeh9480.medium.com/how-to-run-opengl-based-tests-on-github-actions-60f270b1ea2c


C:\tools\glew-2.1.0\include\GL
C:\tools\glew-2.1.0\lib\Release\x64
C:\tools\glew-2.1.0\bin\Release\x64


https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html


https://learnopengl.com/Getting-started/Hello-Triangle
https://learnopengl.com/Getting-started/Shaders

https://github.com/nothings/stb/blob/master/stb_image.h



https://learnopengl.com/In-Practice/2D-Game/Postprocessing


Where you left off: 

we are rendering to a texture and applying a shader to it. Now you need to make that texture something that isnt plain white


