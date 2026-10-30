#include "Face.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

void require(bool condition, const char* message)
{
    if(!condition)
    {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void checkBatches(std::vector<Face> faces, std::size_t expectedBatchCount)
{
    std::vector<Face> original = faces;
    std::vector<FaceBatch> batches = buildFaceBatches(faces);
    require(batches.size() == expectedBatchCount, "Unexpected number of texture batches");

    std::size_t covered = 0;
    uint32_t previousTexture = 0;
    for(const FaceBatch& batch : batches)
    {
        require(batch.firstFace == covered && batch.faceCount > 0,
                "Batches must cover faces contiguously without empty draws");
        require(covered == 0 || batch.textureIndex > previousTexture,
                "Each texture must have exactly one batch");
        require(batch.firstFace + batch.faceCount <= faces.size(), "Batch exceeds face buffer");
        for(std::size_t i = batch.firstFace; i < batch.firstFace + batch.faceCount; ++i)
        {
            require(((faces[i].data >> 3u) & 2047u) == batch.textureIndex,
                    "A draw would select the wrong texture for a face");
        }
        covered += batch.faceCount;
        previousTexture = batch.textureIndex;
    }
    require(covered == original.size(), "Some faces were lost");

    const auto byPosition = [](const Face& left, const Face& right) { return left.x < right.x; };
    std::sort(original.begin(), original.end(), byPosition);
    std::sort(faces.begin(), faces.end(), byPosition);
    for(std::size_t i = 0; i < faces.size(); ++i)
    {
        require(faces[i].x == original[i].x && faces[i].y == original[i].y &&
                faces[i].z == original[i].z && faces[i].data == original[i].data,
                "Batching changed face geometry or packed direction/texture data");
    }
}

int main()
{
    checkBatches({}, 0);
    checkBatches({{0, 2, 3, packFace(TOP, 3)}}, 1);
    checkBatches({{0, 2, 3, packFace(TOP, 1)}, {1, 4, 5, packFace(BOTTOM, 1)}}, 1);

    std::vector<Face> mixed;
    const uint32_t textures[] = {3, 0, 2, 3, 1, 0, 2047, 2};
    for(int i = 0; i < 8; ++i)
    {
        mixed.push_back({i, i + 1, i + 2, packFace(i % 6, textures[i])});
    }
    checkBatches(mixed, 5);
    buildFaceBatches(mixed);
    checkBatches(mixed, 5);
    mixed.clear();
    checkBatches(mixed, 0);
    std::cout << "Face batching regression tests passed\n";
}
