#version 460 core

layout(location = 0) in ivec3 aPosition;
layout(location = 1) in uint aData;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec2 texCoord;
flat out uint textureID;
flat out float faceBrightness;

const vec2 corners[6] = vec2[6](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

void main() {
    uint faceDirection = aData & 7u;
    textureID = (aData >> 3u) & 2047u;

    vec2 uv = corners[gl_VertexID];
    texCoord = uv;

    vec3 localPos;

    if(faceDirection == 0u) {
        localPos = vec3(uv.x, uv.y, 1.0);
    }
    else if(faceDirection == 1u) {
        localPos = vec3(1.0 - uv.x, uv.y, 0.0);
    }
    else if(faceDirection == 2u) {
        localPos = vec3(0.0, uv.y, uv.x);
    }
    else if(faceDirection == 3u) {
        localPos = vec3(1.0, uv.y, 1.0 - uv.x);
    }
    else if(faceDirection == 4u) {
        localPos = vec3(uv.x, 1.0, 1.0 - uv.y);
    }
    else {
        localPos = vec3(uv.x, 0.0, uv.y);
    }

    float brightness = 0.8;

    if(faceDirection == 4u) {
        brightness = 1.0;
    }
    else if(faceDirection == 5u) {
        brightness = 0.5;
    }
    else if(faceDirection == 2u || faceDirection == 3u) {
        brightness = 0.65;
    }

    faceBrightness = brightness;

    vec3 worldPos = vec3(aPosition) + localPos;

    gl_Position = u_projection * u_view * u_model * vec4(worldPos, 1.0);
}
