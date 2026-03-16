#pragma once
#include <memory>
#include <string>
#include "ECS/Registry.h"
#include "Core/TrasformComponent.h"
#include "Scene/SceneComponents.h"

// ACTUALLY IN WORK -- NOT PART OF THE PROJECT

using SceneRegistry = KGR::ECS::Registry<SceneEntity, 100>;

class Scene
{
public:
	SceneEntity CreateEntity(const std::string& name, SceneEntity parent = NullEntity);
	void DestroyEntity(SceneEntity e);
	SceneEntity FindByName(const std::string& name);

	SceneRegistry& GetRegistry();
	const SceneRegistry& GetRegistry() const;

	template<typename T, typename... Args>
	void AddComponent(SceneEntity e, Args&&... args)
	{
		m_registry.AddComponent<T>(e, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent(SceneEntity e)
	{
		return m_registry.GetComponent<T>(e);
	}

	template<typename T>
	T& HasComponent(SceneEntity e) const
	{
		return m_registry.HasComponent<T>(e);
	}

private:
	void DestroySubtree(SceneEntity e);

	SceneRegistry m_registry;
};