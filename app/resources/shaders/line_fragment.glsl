#version 330

in vec3 var_vtx_rgb;
in vec2 var_vtx_tex;

out vec4 FragColor;

uniform sampler2D imageTexture;
uniform sampler2D imageMaskTexture;
uniform int useTexture;
uniform int useImageMaskTexture;

void main()
{
    if (useTexture == 1)
    {
        if (useImageMaskTexture == 1)
        {
            vec4 maskColor = texture(imageMaskTexture, var_vtx_tex);
            if (maskColor.r > 0.0 || maskColor.g > 0.0 || maskColor.b > 0.0) {
                FragColor = texture(imageTexture, var_vtx_tex);
            } else {
                FragColor = vec4(0.0, 0.0, 0.0, 0.25);
            }
        } else {
            FragColor = texture(imageTexture, var_vtx_tex);
        }
    }
    else
    {
        FragColor = vec4(var_vtx_rgb, 1.0);
    }
}
