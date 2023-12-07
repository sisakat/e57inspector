#version 330

layout(location = 0) in vec3 in_vtx_xyz;
layout(location = 1) in vec3 in_vtx_rgb;

out vec3 var_vtx_rgb;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int  pointSize;

void main()
{
    gl_Position = projection * view * model * vec4(in_vtx_xyz, 1.0);
    gl_PointSize = pointSize;
    var_vtx_rgb = in_vtx_rgb;
}
