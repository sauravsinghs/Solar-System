// Fragment shader for translucent overlays (atmospheres, rings)
// Uses texture alpha * overlayAlpha and a color tint, with basic lambert lighting
#version 330 core

in vec2 textureCoordinate;
in vec3 normal;
in vec3 fragmentPosition;
in vec3 worldPosition;

out vec4 FragColor;

uniform vec3 ambientColor;
uniform vec3 sunlightColor;
uniform vec3 lightPosition;
uniform sampler2D textureSampler;
uniform float overlayAlpha;
uniform vec3 colorTint; // multiplier for tinting overlays (e.g., darker Uranus rings)
// No per-pixel wobble; rings will be animated (if desired) via model rotation, not UV warping

void main()
{
    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float diffuseStrength = max(dot(normalizedNormal, lightDirection), 0.0);

    // Use mesh-provided radial mapping: x = radial (inner->outer), y is ignored (use midline)
    vec2 uv = vec2(textureCoordinate.x, 0.5);
    vec4 tex = texture(textureSampler, uv);
    // Use texture alpha multiplied by overlayAlpha for blending (atmosphere / rings)
    vec3 lit = (ambientColor + diffuseStrength * sunlightColor) * (tex.rgb * colorTint);
    float alpha = tex.a * overlayAlpha;
    FragColor = vec4(lit, alpha);
}


