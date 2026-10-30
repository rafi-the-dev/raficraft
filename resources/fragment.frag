#version 460 core
#extension GL_ARB_bindless_texture : require

layout(std430, binding = 0) readonly buffer Textures {
    sampler2D blockTextures[];
};

in vec2 texCoord;
uniform uint textureIndex;
flat in float faceBrightness;

out vec4 FragColor;

void main() {
    vec4 color = texture(blockTextures[textureIndex], texCoord);

    FragColor = vec4(
        color.rgb * faceBrightness,
        color.a
    );
}
