#pragma once
#include "common/mesh-data.h"

namespace physika::common {

MeshData CreateEquilateralTriangle(float const side);

MeshData CreateCube(float const side);

MeshData CreateUniformGrid(int side, int cellSize);

}  // namespace physika::common