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
#include "Generation.h"


int main(int argc, char** argv)
{
	// this part is due to the archi of the code to retrieve the folder resources
	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	GameSceneManager manager(projectRoot / "Ressources");

	GenLoader::SetGlobalFIlePath(projectRoot / "Ressources" / "Gen");
	manager.AddScene(std::make_unique<StartScene>(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f / 60.0f)), "Start", true);

	manager.AddScene(std::make_unique<GameScene>(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f/1000.0f)),"Game",false);
	manager.Run(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f / 60.0f));
}