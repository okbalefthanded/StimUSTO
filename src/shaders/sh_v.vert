#version 300 es
<<<<<<< HEAD
//#version 300 es
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
=======
// #version 460
// #version 300 es
// #version 330
// #version 300 es
layout(location = 0) in highp vec3 position;
layout(location = 1) in highp vec3 color;
>>>>>>> f48039ea1d566beb7dea6e01e3dbafd82c5eeeb1

out highp vec3 vColor;

void main()
{
    gl_PointSize = 4.0f;
    gl_Position = vec4(position, 1.0);
    vColor = color;
}
