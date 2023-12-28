#version 330

layout(location = 0) in vec3  in_vtx_xyz;
layout(location = 1) in vec3  in_vtx_normal;
layout(location = 2) in float in_vtx_intensity;
layout(location = 3) in vec4  in_vtx_rgba;

out vec3 var_vtx_rgb;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int  pointSize;
uniform int  viewType;
uniform vec3 singleColor;

void main()
{
    gl_Position = projection * view * model * vec4(in_vtx_xyz, 1.0);
    gl_PointSize = pointSize;

    if (viewType == 0)
    {
        var_vtx_rgb = in_vtx_rgba.xyz;
    }
    else if (viewType == 1)
    {
        var_vtx_rgb = vec3(in_vtx_intensity, in_vtx_intensity, in_vtx_intensity);
    }
    else
    {
        var_vtx_rgb = singleColor;
    }
}
