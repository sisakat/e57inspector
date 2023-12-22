#version 330

in vec3 var_vtx_rgb;
in vec2 var_vtx_tex;

out vec4 FragColor;

uniform sampler2D imageTexture;
uniform int useTexture;

void main()
{
    if (useTexture == 1)
    {
        FragColor = texture(imageTexture, var_vtx_tex);
    }
    else
    {
        FragColor = vec4(var_vtx_rgb, 1.0);
    }
}
