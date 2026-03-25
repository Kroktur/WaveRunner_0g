#include <iostream>

#include "IGameScene.h"
#include "Core/Transform2dComponent.h"
#include "Core/UiComponent.h"


#include "Core/InputManager.h"
#include "Core/CameraComponent.h"
#include "Core/TrasformComponent.h"
#include "Core/Mesh.h"
#include "Core/Texture.h"
#include "Core/Window.h"
#include "ECS/Component.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"
#include "Tools/Chrono.h"
#include "Audio/SoundComponent.h"
#include "Math/Collision2d.h"
#include "Core/SceneManager.h"
#include "Core/Scene.h"
#include "IGameScene.h"

// make you ecs type with entity 8 / 16 / 32 / 64 and the size of allocation between 1 and infinity
using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

int main(int argc, char** argv)
{
	
	// this part is due to the archi of the code to retrieve the folder resources
	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	GameSceneManager manager(projectRoot / "Ressources");
	manager.AddScene(std::make_unique<MenuScene>(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f / 60.0f)), "Menu", true);
	manager.AddScene(std::make_unique<GameScene>(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f/60.0f)),"Game",false);
	manager.Run(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f / 60.0f));
}