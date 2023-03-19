#pragma once
#include <inttypes.h>

namespace physika {

struct VertexData;

namespace primitive_gen {

void CreateEquilateralTriangle(float const side, VertexData* vertices, uint32_t* vertexCount,
                               uint32_t* indices, uint32_t* indexCount);
void CreateCube(float const side, VertexData* vertices, uint32_t* vertexCount, uint32_t* indices,
                uint32_t* indexCount);

}  // namespace primitive_gen
}  // namespace physika