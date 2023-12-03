#version 330

in vec3 var_vtx_rgb;

out vec4 FragColor;

void main()
{
    FragColor = vec4(var_vtx_rgb, 1.0);
}
