#include "Core/Scene.h"
#include "Core/Window.h"
#include "Core/SceneManager.h"
Scene::Scene(const KGR::Tools::Chrono<float>::Time& time) : m_time(time)
{

}

void Scene::Init(SceneManager* manager)
{
}

KGR::Tools::Chrono<float>::Time Scene::GetTime() const
{
	return m_time;
}
