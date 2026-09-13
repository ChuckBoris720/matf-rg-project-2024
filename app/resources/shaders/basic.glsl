//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}


//#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightDirection;
uniform vec3 lightColor;

uniform vec3 pointLightPosition;
uniform vec3 pointLightColor;

void main() {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // direkciono svetlo
    vec3 directionalLightDir = normalize(-lightDirection);

    float directionalDiffuse = max(dot(normal, directionalLightDir), 0.0);

    vec3 directionalReflectDir = reflect(-directionalLightDir, normal);

    float directionalSpecular = pow(max(dot(viewDir, directionalReflectDir), 0.0), 32.0);

    vec3 directionalLighting = (directionalDiffuse * objectColor + 0.5 * directionalSpecular) * lightColor;

    // tackasto svetlo
    vec3 pointLightDir = normalize(pointLightPosition - FragPos);

    float distance = length(pointLightPosition - FragPos);

    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float pointDiffuse = max(dot(normal, pointLightDir), 0.0);

    vec3 pointReflectDir = reflect(-pointLightDir, normal);

    float pointSpecular = pow(max(dot(viewDir, pointReflectDir), 0.0), 32.0);

    vec3 pointLighting = attenuation * pointLightColor * (pointDiffuse * objectColor + 0.5 * pointSpecular);

    // ambijentalno svetlo
    vec3 ambient = 0.3 * objectColor;

    // celokupno svetlo
    vec3 lighting = ambient + directionalLighting + pointLighting;

    FragColor = vec4(lighting, 1.0);

}