#pragma once

constexpr float cubeVertices[] = {
    // Front (+Z)
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // Back (-Z)
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

    // Top (+Y)
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Bottom (-Y)
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // Left (-X)
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,

    // Right (+X)
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f
};

constexpr unsigned int cubeIndices[] = {
     0,  1,  2,   0,  2,  3,  // Front
     4,  5,  6,   4,  6,  7,  // Back
     8,  9, 10,   8, 10, 11,  // Top
    12, 13, 14,  12, 14, 15,  // Bottom
    16, 17, 18,  16, 18, 19,  // Left
    20, 21, 22,  20, 22, 23   // Right
};

struct Block {
    enum {
        air = 0,
        dirt,
        stone,
        grass,
    };
    int type = 0;
};
