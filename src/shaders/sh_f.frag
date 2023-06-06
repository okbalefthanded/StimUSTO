#version 300 es
<<<<<<< HEAD
//#version 300 es
=======
// #version 460
// #version 300 es
>>>>>>> f48039ea1d566beb7dea6e01e3dbafd82c5eeeb1
// #version 330
// #version 300 es
// #version 330
// in highp vec4 vColor;

in highp vec3 vColor;
out highp vec4 fColor;

void main()
{
   fColor = vec4(vColor, 1.0);
}
