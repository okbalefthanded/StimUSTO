#version 300 es
//in highp vec4 vColor;
in highp vec3 vColor;
out highp vec4 fColor;

void main()
{
   fColor = vec4(vColor, 1.0);
}