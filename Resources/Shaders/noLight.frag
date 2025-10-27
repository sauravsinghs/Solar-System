#version 330 core

in vec2 textureCoordinate;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform float timeSeconds;

void main()
{
    vec2 uv = vec2(textureCoordinate.x, 1.0 - textureCoordinate.y);
    vec4 col = texture(textureSampler, uv);
    // Slight emissive boost for the sun
    FragColor = vec4(min(col.rgb * 1.15, 1.0), col.a);
}