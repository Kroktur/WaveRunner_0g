#include "Scene/Scene.h"

// ACTUALLY IN WORK -- NOT PART OF THE PROJECT

SceneEntity Scene::CreateEntity(const std::string& name, SceneEntity parent)
{
	SceneEntity e = m_registry.CreateEntity();

	m_registry.AddComponent<NameComponent>(e, NameComponent{ name });
	m_registry.AddComponent<TransformComponent>(e);
	m_registry.AddComponent<HierarchyComponent>(e, HierarchyComponent{ parent, {} });

	if (parent != NullEntity && m_registry.HasComponent<HierarchyComponent>(parent))
	{
		auto& parentHierarchy = m_registry.GetComponent<HierarchyComponent>(parent);
		parentHierarchy.m_children.push_back(e);
	}

	return e;
}

void Scene::DestroyEntity(SceneEntity e)
{
	if (m_registry.HasComponent<HierarchyComponent>(e))
	{
		auto& hierarchy = m_registry.GetComponent<HierarchyComponent>(e);

		if (hierarchy.m_parent != NullEntity && m_registry.HasComponent<HierarchyComponent>(hierarchy.m_parent))
		{
			auto& childrens = m_registry.GetComponent<HierarchyComponent>(hierarchy.m_parent).m_children;
			childrens.erase(std::remove(childrens.begin(), childrens.end(), e), childrens.end());
		}
	}

	DestroySubtree(e);
}

SceneEntity Scene::FindByName(const std::string& name)
{
	for (SceneEntity e : m_registry.GetAllEntities())
	{
		if (!m_registry.HasComponent<NameComponent>(e))
			continue;

		if (m_registry.GetComponent<NameComponent>(e).name == name)
			return e;
	}
	return NullEntity;
}

SceneRegistry& Scene::GetRegistry()
{
	return m_registry;
}

const SceneRegistry& Scene::GetRegistry() const
{
	return m_registry;
}

void Scene::DestroySubtree(SceneEntity e)
{
	if (m_registry.HasComponent<HierarchyComponent>(e))
	{
		auto children = m_registry.GetComponent<HierarchyComponent>(e).m_children;
		for (SceneEntity child : children)
			DestroySubtree(child);
	}

	m_registry.DestroyEntity(e);
}