#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 objectColor = vec3(0.5, 0.5, 0.9);
uniform vec3 lightPos;

uniform sampler2D ambient_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

void main()
{
	vec3 specular = vec3(1.0f);
	vec3 lightColor = vec3(0.7, 0.7, 0.7);

	float ambientStrength = 0.4f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = diff * lightColor;
	
	vec3 result = (ambient + diffuse);
	FragColor = texture(diffuse_map, TexCoord) * vec4(result, 1.0f);
}
