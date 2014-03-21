1. The homework 5 files are in the folder hw5.
2. I have programmed it using visual studio 2012 and the project works by running the CS580HW5.sln file.
3. I have added the disp.h,disp.cpp,rend.h and rend.cpp
4. Texture mapping (function tex_fun.cpp) on Phong and Gourad shading works as intended by toggling the interpolation style value and mapping type between tex_fun and ptex_fun
5. The procedural texture function implements a random julia set to compute the colors.

Extra Credit Submission
----------------------------
1. In line 26 of the Application5.cpp file 
     --select TOON_SHADING as 1 to get toon shaded version of the teapot. 
     --select TOON_SHADING as 0 to get the rasterized teapot with phong or gourad shading.

For this purpose a ToonShader.h file was added to the project

2. In rend.cpp,
-- Line 9: Toggle Grainy Noise Effect between 0 and 1 to get a grainy teapot.

For this purpose perlin noise function was implemented using the algorithm by Ken Perlin.
The following functions were added to rend.cpp

float raw_noise(const float x, const float y, const float z); 
float scaled_octave_noise(const float octaves, const float persistence, const float scale, const float loBound, const float hiBound, const float x, const float y, const float z);

3. Added a Render -> RotateAnimation button that rotates the teapot along the y-axis. The resulting animation is stored in sequentially generated ppm files.
-- For this purpose modifications to the following files were made.

---- CS580HW.rc
---- CS580HWView.h
---- CS580HWView.cpp
---- Application5.cpp

Can Rotate the rendered teapot as well as the toon shaded teapot with or without the noise effect.