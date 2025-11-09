// Fragment shader for general lit surfaces
// Supports subtle flowing UVs for gaseous giants via flowAmount
// Uniforms:
//  - ambientColor, sunlightColor, lightPosition
//  - textureSampler (albedo)
//  - viewPosition (for rim lighting)
//  - timeSeconds (for animation), flowAmount (0..1)
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
uniform float timeSeconds;
uniform float flowAmount; // 0 for rocky, 1 for gaseous

void main()
{
    vec3 n = normalize(normal);
    // Add subtle flow to gaseous giants by modulating texture lookup
    vec2 flow = flowAmount * vec2(0.02 * sin(timeSeconds*0.1), 0.01 * cos(timeSeconds*0.08));
    vec4 tex = texture(textureSampler, textureCoordinate + flow);
    vec3 l = normalize(lightPosition - fragmentPosition);
    float ndotl = max(dot(n, l), 0.0);
    vec3 diffuse = ndotl * sunlightColor;
    // Simple rim to soften terminator a bit for small bodies
    vec3 v = normalize(viewPosition - fragmentPosition);
    float rim = pow(1.0 - max(dot(n, v), 0.0), 2.0);
    vec3 color = ambientColor + diffuse + 0.1 * rim * sunlightColor;
    FragColor = vec4(color, 1.0) * tex;
}

