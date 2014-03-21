1. The homework 4 files are in the folder hw4.
2. I have programmed it using visual studio 2012 and the project works by running the CS580HW4.sln file.
3. I have added the disp.h,disp.cpp,rend.h and rend.cpp
4. Phong and Gourad shading works as intended by toggling the interpolation style value between 0 and 1 in Application4.cpp 

Extra Credit Submission
----------------------------
1. In line 26 of the Application4.cpp file 
     --select TOON_SHADING as 1 to get toon shaded version of the teapot. 
     --select TOON_SHADING as 0 to get the rasterized teapot with phong or gourad shading.

For this purpose a ToonShader.h file was added to the project

2. Added a Render -> RotateAnimation button that rotates the teapot along the y-axis. The resulting animation is stored in sequentially generated ppm files.
-- For this purpose modifications to the following files were made

---- CS580HW.rc
---- CS580HWView.h
---- CS580HWView.cpp
---- Application4.cpp

Can Rotate the rendered teapot as well as the toon shaded teapot.