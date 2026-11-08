#version 460 core
#extension GL_ARB_bindless_texture : require

layout(std430, binding = 0) readonly buffer Textures {
    sampler2D blockTextures[];
};

in vec2 texCoord;
flat in uint vTextureIndex;
flat in float faceBrightness;
flat in uint vBlockLight;
flat in uint vSkyLight;

out vec4 FragColor;

void main() {
    float blockLight = float(vBlockLight) / 15.0;
    float skyLight = float(vSkyLight) / 15.0;
    float light = max(max(blockLight, skyLight), 0.1);

    vec4 color = texture(blockTextures[vTextureIndex], texCoord);

    FragColor = vec4(
            color.rgb * faceBrightness * light,
            color.a
        );
}
