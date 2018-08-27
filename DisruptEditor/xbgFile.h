#pragma once

#include <stdint.h>
#include "Vector.h"
#include <string>
#include "GLHelper.h"

/*

Wobble wrote:
I found the vertex format here:
vertexdeclaration.inc.fx

VERTEX_DECL_POSITIONCOMPRESSED
VERTEX_DECL_POSITIONFLOAT
VERTEX_DECL_UV0
VERTEX_DECL_UV1
...

They don't say what the actual bits are for each flag, but just looking at the order of things gives
a basic layout of the format:
position | uv0 | uv1 | blend_weights | blend_indices | normal | color | tangent | binormal | ...

Perhaps these flags are defined in an XBOX or PS3 SDK that is only available to Developers.


For stride 44 theres a blank unknown if i can recall but yeah thats the basic format if only I was home and the game was dx9 could easily get the bit values using Microsoft PIX
//POS
//UV
//UV2
//BoneWeights
//BoneIndices
//blankUnknown
//Vertex normal
//Vertex col
//Bi-Normal
//Tangents

*/

class xbgFile {
public:
	void open(const char* file);

	struct Mesh {
		uint16_t vertexStride, matID, vertexCount, totalVertexCount, faceCount, UVFlag, scaleFlag, boneMapID;
		std::string mat;
		Vector<uint16_t> buffer, index;
		VertexBuffer vbo, ibo;
	};
	Vector<Mesh> meshes;

	struct Material {
		uint32_t hash;
		std::string file;
	};
	Vector<Material> materials;

	void draw();
};

