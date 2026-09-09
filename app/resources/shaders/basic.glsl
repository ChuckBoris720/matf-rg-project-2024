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

uniform vec3 objectColor;
uniform vec3 lightDirection;
uniform vec3 lightColor;

void main() {
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);

    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 ambient = 0.2 * objectColor;
    vec3 lighting = ambient + diffuse * lightColor * objectColor;

    FragColor = vec4(lighting, 1.0);
}