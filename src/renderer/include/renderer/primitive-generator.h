#pragma once
#include "renderer/types.h"

namespace physika::renderer {

MeshData CreateEquilateralTriangle(float const side);

MeshData CreateCube(float const side);

MeshData CreateUniformGrid(int side, int cellSize);

}  // namespace physika::renderer