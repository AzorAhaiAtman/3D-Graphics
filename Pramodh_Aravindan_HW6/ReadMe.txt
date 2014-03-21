1. The homework 6 files are in the folder hw6.
2. I have programmed it using visual studio 2012 and the project works by running the CS580HW5.sln file.
3. I have added the disp.h,disp.cpp,rend.h and rend.cpp
4. I used my files from the previous homework and  have made a few minor modifications in application.cpp, application5.cpp, application5.h, cs580hwview.cpp to support multisampling anti-aliasing.

Extra Credit Submission
----------------------------
1. In line 24 of the Application5.cpp file 
     --select TOON_SHADING as 1 to get toon shaded version of the teapot with anti-aliasing. 
     --select TOON_SHADING as 0 to get the rasterized teapot with phong or gourad shading, texturing and anti-aliasing

For this purpose a ToonShader.h file was added to the project

2. In rend.cpp,
-- Line 9: Toggle Grainy Noise Effect between 0 and 1 to get a grainy teapot.

For this purpose perlin noise function was implemented using the algorithm by Ken Perlin.

float raw_noise(const float x, const float y, const float z); 
float scaled_octave_noise(const float octaves, const float persistence, const float scale, const float loBound, const float hiBound, const float x, const float y, const float z);

3. Added a Render -> RotateAnimation button that rotates the teapot along the y-axis. The resulting animation is stored in sequentially generated ppm files.
-- For this purpose modifications to the following files were made

---- CS580HW.rc
---- CS580HWView.h
---- CS580HWView.cpp
---- Application5.cpp

Can Rotate the anti-aliased teapot as well as the toon shaded teapot with or without the noise effect.