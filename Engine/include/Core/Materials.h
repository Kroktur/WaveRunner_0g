#pragma once
#include "Texture.h"

struct Material
{
	Texture* baseColor = nullptr;
	Texture* emissive = nullptr;
	Texture* normalMap = nullptr;
	Texture* pbrMap = nullptr;
};


/**
 * @brief Component that stores a collection of materials per submesh.
 *
 * This component manages a dynamic array of Material objects, typically
 * corresponding to the different submeshes of a mesh. Each submesh can
 * have its own material assigned.
 */
struct MaterialComponent
{
    std::vector<Material> materials;
};