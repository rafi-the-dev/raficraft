#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : require

layout(std430, binding = 0) readonly buffer Textures {
    sampler2D blockTextures[];
};

in vec2 texCoord;
flat in uint textureID;
flat in float faceBrightness;

out vec4 FragColor;

void main() {
    vec4 color = texture(blockTextures[textureID], texCoord);

    FragColor = vec4(
        color.rgb * faceBrightness,
        color.a
    );
}
