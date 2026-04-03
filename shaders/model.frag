#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 ViewPos;

uniform vec3 ambient_mod = vec3(1.0f);
uniform vec3 diffuse_mod = vec3(1.0f);
uniform vec3 specular_mod = vec3(1.0f);


uniform sampler2D ambient_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

uniform int has_ambient_map = 0;
uniform int has_diffuse_map = 0;
uniform int has_specular_map = 0;

uniform float specular_pow = 32.0f;

void main()
{
    vec3 lightColor = vec3(0.7, 0.7, 0.7);

    float ambientStrength = 0.4f;
    vec4 ambient = vec4(ambient_mod * ambientStrength * lightColor, 1.0);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, norm), 0.0f);
    vec4 diffuse = vec4(diffuse_mod * diff * lightColor, 1.0);


    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec4 specular = vec4(specular_mod * spec * lightColor, 1.0);
    if (has_diffuse_map == 1)
    {
        ambient = texture(diffuse_map, TexCoord) * ambient;
        diffuse = texture(diffuse_map, TexCoord) * diffuse; 
    }
    if (has_specular_map == 1)
    {
        specular = texture(specular_map, TexCoord) * specular;
    }

    vec4 result = ambient + diffuse + specular;
    FragColor = result;
}
