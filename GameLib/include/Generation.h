#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <array>
#include <glm/vec3.hpp>

#include "Core/Materials.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/Window.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"
#include "Math/CollisionComponent.h"

struct PlatFormMove
{};
struct LifeTimeComp
{
	bool isDead = false;
};

using my_ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct GenBindRegistry
{
	void Register(char c,const std::string& str)
	{
		if (m_map.contains(c))
			throw std::runtime_error("already store");
		m_map[c] = str;
	}
	std::string Get(char c)
	{
		if (!m_map.contains(c))
			throw std::runtime_error("not store");
		return m_map[c];
	}
private:
	std::unordered_map<char, std::string> m_map;
};


static std::array<char, 150> test;

struct SimpleTab
{
	SimpleTab()
	{
		array.fill(' ');
	}
	static constexpr int width = 3;
	static constexpr int height = 50;
	std::array<char, height* width> array;
};


struct PosMapper
{
	glm::vec3 midPos;
	glm::vec3 rightVec;
	float offset;
	float radAngle;
};

static glm::vec3 Generate(GenBindRegistry& reg, my_ecsType& ecs,const PosMapper& mapper,const SimpleTab& tab,KGR::RenderWindow& window)
{
	glm::vec3 leftPos = mapper.midPos - mapper.offset * mapper.rightVec;
	glm::vec3 midPos = mapper.midPos;
	glm::vec3 rightPos = mapper.midPos + mapper.offset * mapper.rightVec;
	float advance = 0;
	for (int i = 0 ; i < SimpleTab::height; ++i , ++advance)
	{
		// left
		{
			char leftC = tab.array[3 * i + 0];
			if (leftC == ' ')
				continue;
			std::string leftStr = reg.Get(leftC);
			{
				MeshComponent obstacle;
				obstacle.mesh = &MeshLoader::Load(leftStr, window.App());

				MaterialComponent tex;

				tex.materials.resize(obstacle.mesh->GetSubMeshesCount());

				for (int i = 0; i < obstacle.mesh->GetSubMeshesCount(); ++i)
				{
					Material mat;
					mat.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", window.App());
					mat.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", window.App());
					mat.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", window.App());
					mat.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", window.App());

					tex.materials[i] = mat;
				}


				CollisionComp collider;
				collider.collider = &ColliderManager::Load(leftStr, obstacle.mesh);

				TransformComponent transform;
				transform.SetPosition(leftPos + advance * glm::vec3{ 0,0,-1 });
				transform.RotateEuler<RotData::Orientation::Roll>(mapper.radAngle);

				auto e_left = ecs.CreateEntity();
				ecs.AddComponents(e_left, std::move(obstacle), std::move(tex), std::move(transform), std::move(collider), PlatFormMove{},LifeTimeComp{});
			}

			
		}


		// mid
		{
			char midC = tab.array[3 * i + 1];
			if (midC == ' ')
				continue;
			std::string midStr = reg.Get(midC);
			{
				MeshComponent obstacle;
				obstacle.mesh = &MeshLoader::Load(midStr, window.App());

				MaterialComponent tex;

				tex.materials.resize(obstacle.mesh->GetSubMeshesCount());

				for (int i = 0; i < obstacle.mesh->GetSubMeshesCount(); ++i)
				{
					Material mat;
					mat.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", window.App());
					mat.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", window.App());
					mat.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", window.App());
					mat.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", window.App());

					tex.materials[i] = mat;
				}


				CollisionComp collider;
				collider.collider = &ColliderManager::Load(midStr, obstacle.mesh);

				TransformComponent transform;
				transform.SetPosition(midPos + advance * glm::vec3{ 0,0,-1 });
				transform.RotateEuler<RotData::Orientation::Roll>(mapper.radAngle);

				auto e_left = ecs.CreateEntity();

				ecs.AddComponents(e_left, std::move(obstacle), std::move(tex), std::move(transform), std::move(collider), PlatFormMove{}, LifeTimeComp{});
			}


		}
		// right
		{
			char rightC = tab.array[3 * i + 2];
			if (rightC == ' ')
				continue;
			std::string rightStr = reg.Get(rightC);
			{
				MeshComponent obstacle;
				obstacle.mesh = &MeshLoader::Load(rightStr, window.App());

				MaterialComponent tex;

				tex.materials.resize(obstacle.mesh->GetSubMeshesCount());

				for (int i = 0; i < obstacle.mesh->GetSubMeshesCount(); ++i)
				{
					Material mat;
					mat.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", window.App());
					mat.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", window.App());
					mat.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", window.App());
					mat.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", window.App());

					tex.materials[i] = mat;
				}


				CollisionComp collider;
				collider.collider = &ColliderManager::Load(rightStr, obstacle.mesh);

				TransformComponent transform;
				transform.SetPosition(rightPos + advance * glm::vec3{ 0,0,-1 } );
				transform.RotateEuler<RotData::Orientation::Roll>(mapper.radAngle);

				auto e_left = ecs.CreateEntity();

				ecs.AddComponents(e_left, std::move(obstacle), std::move(tex), std::move(transform), std::move(collider), PlatFormMove{}, LifeTimeComp{});
			}


		}

	}
	return advance * glm::vec3{ 0,0,-1 };
	
}
	



