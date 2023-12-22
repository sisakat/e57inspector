#version 330

layout(location = 0) in vec3 in_vtx_xyz;
layout(location = 1) in vec2 in_vtx_tex;

out vec3 var_vtx_rgb;
out vec2 var_vtx_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_vtx_xyz, 1.0);
    var_vtx_rgb = vec3(1.0, 1.0, 1.0);
    var_vtx_tex = in_vtx_tex;
}
