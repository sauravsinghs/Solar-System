// Fragment shader for unlit rendering. Applies UV flow for sun animation
#version 330 core

in vec2 textureCoordinate;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform float timeSeconds;

void main()
{
    // Slow horizontal flow to emulate gas motion / rotation
    vec2 uv = vec2(textureCoordinate.x + 0.01 * timeSeconds, 1.0 - textureCoordinate.y);
    vec4 col = texture(textureSampler, uv);
    // Slight emissive boost for the sun
    FragColor = vec4(min(col.rgb * 1.15, 1.0), col.a);
}