#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 objectColor = vec3(0.5, 0.5, 0.9);
uniform vec3 lightPos;

uniform vec3 ambient_mod = vec3(1.0f);
uniform vec3 diffuse_mod = vec3(1.0f);
uniform vec3 specular_mod = vec3(1.0f);


uniform sampler2D ambient_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

uniform int has_ambient_map = 0;
uniform int has_diffuse_map = 0;
uniform int has_specular_map = 0;

void main()
{
	vec3 specular = vec3(1.0f);
	vec3 lightColor = vec3(0.7, 0.7, 0.7);

	float ambientStrength = 0.4f;
	vec3 ambient = ambient_mod * ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = diffuse_mod * diff * lightColor;
	
	vec4 result = vec4(ambient + diffuse, 1.0f);
        if (has_diffuse_map == 1)
        {
            result = texture(diffuse_map, TexCoord) * result;
        }
	FragColor = result;
}
