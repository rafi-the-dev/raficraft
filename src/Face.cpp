#include "Face.h"
#include <algorithm>

uint32_t packFace(uint32_t direction, uint32_t texture)
{
    return (direction & 7u)
         | ((texture & 2047u) << 3);
}

std::vector<FaceBatch> buildFaceBatches(std::vector<Face>& faces)
{
    const auto textureIndex = [](const Face& face) { return (face.data >> 3u) & 2047u; };
    std::sort(faces.begin(), faces.end(), [&](const Face& left, const Face& right) {
        return textureIndex(left) < textureIndex(right);
    });

    std::vector<FaceBatch> batches;
    for(std::size_t first = 0; first < faces.size();)
    {
        uint32_t texture = textureIndex(faces[first]);
        std::size_t end = first + 1;
        while(end < faces.size() && textureIndex(faces[end]) == texture)
        {
            ++end;
        }
        batches.push_back({texture, first, end - first});
        first = end;
    }
    return batches;
}
