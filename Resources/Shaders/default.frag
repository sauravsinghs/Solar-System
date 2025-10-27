#version 330 core

in vec2 textureCoordinate;
in vec3 normal;
in vec3 fragmentPosition;

out vec4 FragColor;

uniform vec3 ambientColor;
uniform vec3 sunlightColor;
uniform vec3 lightPosition;
uniform sampler2D textureSampler;
uniform vec3 viewPosition;

void main()
{
    vec3 n = normalize(normal);
    vec4 tex = texture(textureSampler, textureCoordinate);
    vec3 l = normalize(lightPosition - fragmentPosition);
    float ndotl = max(dot(n, l), 0.0);
    vec3 diffuse = ndotl * sunlightColor;
    // Simple rim to soften terminator a bit for small bodies
    vec3 v = normalize(viewPosition - fragmentPosition);
    float rim = pow(1.0 - max(dot(n, v), 0.0), 2.0);
    vec3 color = ambientColor + diffuse + 0.1 * rim * sunlightColor;
    FragColor = vec4(color, 1.0) * tex;
}

