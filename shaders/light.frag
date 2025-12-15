#version 330 core

out vec4 FragColor;

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    FragColor = vec4(lightColor, 1.0f);
}
