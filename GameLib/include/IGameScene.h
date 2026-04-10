#pragma once
#include "Core/Scene.h"
#include <filesystem>

#include "Audio/SoundComponent.h"
#include "Core/SceneManager.h"
#include "Core/Scene.h"
#include "Tools/Random.h"
#include "Core/Window.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"

#include "Core/Transform2dComponent.h"
#include "Core/UiComponent.h"


#include "Core/InputManager.h"
#include "Core/CameraComponent.h"
#include "Core/TrasformComponent.h"
#include "Core/Mesh.h"
#include "Core/Texture.h"
#include "Math/Collision2d.h"

#include "EventBus.h"
#include "Core/gravityComponent.h"
#include "Math/CollisionComponent.h"
#include "Math/OBB.h"
#include "Math/SAT.h"
#include <iostream>

#include "Score.h"
#include "Generation.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

glm::vec3 camCurrentPos = { 0,8,15 };
glm::vec3 camCurrentLook = { 0,-10, 0 };
float smoothCamSpeed = 2.0f;
bool isGravity = false;

glm::vec3 PlayerCurrentPos = { 0,0,2 };
float smoothPlayerSpeed = 6.0f;

struct controlComponentPlayer
{
	glm::vec3 PlayerTargetPos = { 0.0f, 0.0f, 2.0f };
};

enum class DirectionState
{
	HAUT
	, BAS
	, GAUCHE
	, DROITE

};

enum class PositionState
{
	CENTRE,
	GAUCHE,
	DROITE
};


struct Pos
{
	std::pair<DirectionState, PositionState> pairPosition;
	glm::vec2 position;
};

static constexpr float centerOffset = 4.0f;
static constexpr glm::vec3 worldCenter = { 0,0,0 };
static constexpr float offsetMove = 1.0f;
static constexpr float genNeeded = -50;
static constexpr float zValue = worldCenter.z + centerOffset;
static constexpr float attenuationFactor = 3.0f;
static constexpr int maxFile = 25;

const std::array<Pos, 12> Positions =
{ {
	{{DirectionState::BAS, PositionState::GAUCHE},  { worldCenter.x - offsetMove, worldCenter.y }},
	{{DirectionState::BAS, PositionState::CENTRE},  {worldCenter }},
	{{DirectionState::BAS, PositionState::DROITE},  {  worldCenter.x + offsetMove, worldCenter.y }},

	{{DirectionState::HAUT, PositionState::GAUCHE},  {  worldCenter.x + offsetMove , worldCenter.y + centerOffset}},
	{{DirectionState::HAUT, PositionState::CENTRE},  { worldCenter.x, worldCenter.y + centerOffset }},
	{{DirectionState::HAUT, PositionState::DROITE},  {worldCenter.x - offsetMove, worldCenter.y + centerOffset  }},

	{{DirectionState::GAUCHE, PositionState::GAUCHE},  { -4, worldCenter.y + offsetMove}},
	{{DirectionState::GAUCHE, PositionState::CENTRE},  { -4, worldCenter.y }},
	{{DirectionState::GAUCHE, PositionState::DROITE},  { -4, worldCenter.y - offsetMove }},

	{{DirectionState::DROITE, PositionState::GAUCHE},  { 4,  worldCenter.y - offsetMove  }},
	{{DirectionState::DROITE, PositionState::CENTRE},  { 4,  worldCenter.y  }},
	{{DirectionState::DROITE, PositionState::DROITE},  { 4, worldCenter.y + offsetMove  }},

} };

glm::vec2 getPosition(DirectionState dir, PositionState pos)
{
	for (const auto& posArr : Positions)
	{
		if (posArr.pairPosition == std::make_pair(dir, pos))
		{
			return posArr.position;
		}
	}
	return { 0.0f, 0.0f };
}

struct playerComponent
{
	DirectionState StartDir = DirectionState::BAS;
	PositionState StartPos = PositionState::CENTRE;
	bool isMoove = false;
	float distMoove = 0;
	glm::vec3 actualPos;
	glm::vec3 targetPos;


	bool isInvincible = false;
};




struct Plateform
{

};


struct Obstacle
{
	float velocityObstacle = 6.0f;
};

struct ChangeSceneEvent
{
	std::string targetScene;
};


struct CloseEvent
{
	
};
struct GameSceneManager : public SceneManager
{
	bool shouldClose = false;
	GameSceneManager(const std::filesystem::path& path)
	{
		KGR::RenderWindow::Init();
		m_window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,800 }, "My_Super_Mega_Duper_Projet_De_La_Mort_Qui_Tue_!!", path);
		KGR::Audio::WavComponent::Init();
		KGR::Audio::WavStreamComponent::Init();
		KGR::EventBus<ChangeSceneEvent>::AddListener(this);
		KGR::EventBus<ChangeSceneEvent>::AddCallBack<GameSceneManager>(&GameSceneManager::ChangeScene);

		KGR::EventBus<CloseEvent>::AddListener(this);
		KGR::EventBus<CloseEvent>::AddCallBack<GameSceneManager>(&GameSceneManager::Close);
	}
	~GameSceneManager() override
	{
		KGR::EventBus<ChangeSceneEvent>::RemoveListener(this);

		KGR::EventBus<CloseEvent>::RemoveListener(this);

	}
	KGR::RenderWindow* GetWindow() const
	{
		return m_window.get();
	}
	bool LoopCondition() const override
	{
		if (shouldClose)
			return false;
		return !m_window->ShouldClose();
	}
	void Destroy() override
	{
		m_window->Destroy();
		SceneManager::Destroy();
		KGR::RenderWindow::End();
	}
	void ChangeScene(const ChangeSceneEvent& event)
	{
		SetCurrentScene(event.targetScene);
	}
	void Close(const CloseEvent& event)
	{
		shouldClose = true;
	}
private:
	std::unique_ptr<KGR::RenderWindow> m_window;
};

struct IGameScene : public Scene
{
	IGameScene(const KGR::Tools::Chrono<float>::Time& time) :Scene(time), m_window(nullptr) {}
	void Init(SceneManager* manager) override
	{
		auto m = static_cast<GameSceneManager*>(manager);
		m_window = m->GetWindow();
	}
	void Update(float dt) override
	{
		KGR::RenderWindow::PollEvent();
		m_window->Update();
	}
	void Render() override
	{
		{
			auto es = m_ecs.GetAllComponentsView<CameraComponent, TransformComponent>();
			if (es.size() != 1)
				throw std::runtime_error("need one and one cam");
			for (auto& e : es)
			{
				m_ecs.GetComponent<CameraComponent>(e).UpdateCamera(m_ecs.GetComponent<TransformComponent>(e).GetFullTransform());
				m_ecs.GetComponent<CameraComponent>(e).SetAspect(m_window->GetSize().x, m_window->GetSize().y);
				m_window->RegisterCam(m_ecs.GetComponent<CameraComponent>(e), m_ecs.GetComponent<TransformComponent>(e));
			}
		}


		{
			auto es = m_ecs.GetAllComponentsView<MeshComponent, TransformComponent, MaterialComponent>();
			for (auto& e : es)
			{
				m_window->RegisterRender(
					m_ecs.GetComponent<MeshComponent>(e),
					m_ecs.GetComponent<TransformComponent>(e),
					m_ecs.GetComponent<MaterialComponent>(e));
			}

		}
		{
			auto es = m_ecs.GetAllComponentsView<LightComponent<LightData::Type::Point>, TransformComponent>();
			for (auto& e : es)
				m_window->RegisterLight(m_ecs.GetComponent<LightComponent<LightData::Type::Point>>(e), m_ecs.GetComponent<TransformComponent>(e));
		}
		{
			auto es = m_ecs.GetAllComponentsView<LightComponent<LightData::Type::Spot>, TransformComponent>();
			for (auto& e : es)
				m_window->RegisterLight(m_ecs.GetComponent<LightComponent<LightData::Type::Spot>>(e), m_ecs.GetComponent<TransformComponent>(e));
		}
		{
			auto es = m_ecs.GetAllComponentsView<LightComponent<LightData::Type::Directional>, TransformComponent>();
			for (auto& e : es)
				m_window->RegisterLight(m_ecs.GetComponent<LightComponent<LightData::Type::Directional>>(e), m_ecs.GetComponent<TransformComponent>(e));
		}
		{
			auto es = m_ecs.GetAllComponentsView < TextureComponent, TransformComponent2d, UiComponent >();
			for (auto& e : es)
			{
				auto transform = m_ecs.GetComponent<TransformComponent2d>(e);
				auto ui = m_ecs.GetComponent<UiComponent>(e);
				auto texture = m_ecs.GetComponent<TextureComponent>(e);
				m_window->RegisterUi(ui, transform, texture);
			}
		}
		{
			auto es = m_ecs.GetAllComponentsView < TextComp, TransformComponent2d, UiComponent >();
			for (auto& e : es)
			{
				auto& transform = m_ecs.GetComponent<TransformComponent2d>(e);
				auto& ui = m_ecs.GetComponent<UiComponent>(e);
				auto& text = m_ecs.GetComponent<TextComp>(e);
				m_window->RegisterText(ui, transform, text);
			}
		}
		m_window->Render({ 0.0f, 0.0f, 0.0f , 1});
	}
protected:
	ecsType m_ecs;
	KGR::RenderWindow* m_window;
};






struct ScoreText
{
	
};

struct LiveText
{
	
};
struct ScoreAdded
{
	std::string name = "unknow";
	int number = 1;
};


struct GameScene : public IGameScene
{
	float spawnTimer = 0.0f;
	float spawnInterval = 2.0f;
	float obstacleCount = 0;
	float lastZ = zValue - 10.0f;
	ScoreAlign allign = ScoreAlign{ 8};
	int score = 0;

	ScoreAlign allignLive = ScoreAlign{ 1 };
	int life = 5;
	GenBindRegistry reg;
	GameScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time) {}
	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);
		// camera 
		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(90.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 200.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,0,0 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 50.0f, 50.0f});
			// here the scale
			ui.SetScale({ 486.0f,177.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/score.png", m_window->App());
			// same as always 
			auto e = m_ecs.CreateEntity();
			//TextComp text;
			////text.text.font = &FontLoader::Load("Fonts/arial.ttf", m_window->App());
			//text.text.SetText("je pense donc je suis !\nje mange des arbres ");
			//text.text.textTexture = &TextureLoader::Load("Textures/viking_room.png", m_window->App());
			//text.text.SetAlign(Text::Align::Center);



			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture)/*, std::move(text)*/);

		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 92.0f, 79.0f });
			// here the scale
			ui.SetScale({ 394.0f,101.0f });
			// create a texture but be aware that only the first texture in the component will be use 

			// same as always 
			auto e = m_ecs.CreateEntity();
			TextComp text;
			text.text.font = &FontLoader::Load("Fonts/Lazer84.ttf", m_window->App(),6);
			text.text.SetText(" ");
			text.text.SetAlign(Text::Align::Center);
			text.text.textTexture = &TextureLoader::Load("Textures/PBC.png", m_window->App());


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(text), ScoreText{});

		}



		// mesh
		{
			// a mesh need a meshComponent a transform and a texture 

			// create a mesh and load it with the cash loader
			MeshComponent platform_down;
			platform_down.mesh = &MeshLoader::Load("Models/Enviro/floor.obj", m_window->App());
	
			// create a texture 
			MaterialComponent text_down;
			// allocate the size of the texture must be the same as the number of submeshes 
			text_down.materials.resize(platform_down.mesh->GetSubMeshesCount());
			// then fill the texture ( this system need to be refact but for now you need to do it like that
			for (int i = 0; i < platform_down.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_down;
				mat_down.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", m_window->App());
				mat_down.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", m_window->App());
				mat_down.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", m_window->App());
				mat_down.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", m_window->App());

				text_down.materials[i] = mat_down;
			}

			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformDown", platform_down.mesh);

			// create the transform and set all the data
			TransformComponent transform_down;
			transform_down.SetPosition(worldCenter + glm::vec3{0,-1,0} * centerOffset);
			transform_down.SetScale({ 1.0f,1.0f,4.0f });
			// same create an entity / id
			auto e_down = m_ecs.CreateEntity();
			// fill the component
			m_ecs.AddComponents(e_down, std::move(platform_down), std::move(text_down), std::move(transform_down), std::move(collider), Plateform{});
		}

		{
			MeshComponent player;
			player.mesh = &MeshLoader::Load("Models/Perso/mesh.obj", m_window->App());

			MaterialComponent text_player;

			text_player.materials.resize(2);

			
				Material mat_player;
				mat_player.baseColor = &TextureLoader::Load("Textures/Perso/BaseColor.png", m_window->App());
				mat_player.emissive = &TextureLoader::Load("Textures/Perso/Emissive.png", m_window->App());
				mat_player.normalMap = &TextureLoader::Load("Textures/Perso/Normal.png", m_window->App());
				mat_player.pbrMap = &TextureLoader::Load("Textures/Perso/ORM.png", m_window->App());


				text_player.materials[0] = mat_player;
				mat_player.baseColor = &TextureLoader::Load("Textures/Trotti/BaseColor.png", m_window->App());
				mat_player.emissive = &TextureLoader::Load("Textures/Trotti/Emissive.png", m_window->App());
				mat_player.normalMap = &TextureLoader::Load("Textures/Trotti/Normal.png", m_window->App());
				mat_player.pbrMap = &TextureLoader::Load("Textures/Trotti/ORM.png", m_window->App());

				text_player.materials[1] = mat_player;
			
			TransformComponent transform_player;
			transform_player.SetPosition(worldCenter);
			transform_player.SetScale({ 1.0f,1.0f,1.0f });

			controlComponentPlayer player_input;
			playerComponent player_comp;

			PhysicComponent gravity;
			CollisionComp collider;
			collider.collider = &ColliderManager::Load("playerCollider", player.mesh);

			auto e_player = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_player, std::move(player), std::move(text_player), std::move(transform_player), std::move(player_input), std::move(player_comp), std::move(gravity), std::move(collider));
		}

		{
			MeshComponent platform_up;
			platform_up.mesh = &MeshLoader::Load("Models/Enviro/floor.obj", m_window->App());

			MaterialComponent text_up;

			text_up.materials.resize(platform_up.mesh->GetSubMeshesCount());

			for (int i = 0; i < platform_up.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_up;
				mat_up.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", m_window->App());
				mat_up.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", m_window->App());
				mat_up.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", m_window->App());
				mat_up.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", m_window->App());

				text_up.materials[i] = mat_up;
			}


			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformUp", platform_up.mesh);

			TransformComponent transform_up;
			transform_up.SetPosition(worldCenter + glm::vec3{ 0,1,0 } * centerOffset);
			transform_up.SetScale({ 1.0f,1.0f,4.0f });
			transform_up.RotateEuler<RotData::Orientation::Roll>(glm::radians(180.0f));

			auto e_up = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_up, std::move(platform_up), std::move(text_up), std::move(transform_up), std::move(collider), Plateform{});
		}

		{
			MeshComponent platform_left;
			platform_left.mesh = &MeshLoader::Load("Models/Enviro/floor.obj", m_window->App());;

			MaterialComponent text_left;

			text_left.materials.resize(platform_left.mesh->GetSubMeshesCount());

			for (int i = 0; i < platform_left.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_left;
				mat_left.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", m_window->App());
				mat_left.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", m_window->App());
				mat_left.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", m_window->App());
				mat_left.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", m_window->App());

				text_left.materials[i] = mat_left;
			}


			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformLeft", platform_left.mesh);

			TransformComponent transform_left;
			transform_left.SetPosition(worldCenter + glm::vec3{ -1,0,0 } * centerOffset);
			transform_left.SetScale({ 1.0f,1.0f,4.0f });
			transform_left.RotateEuler<RotData::Orientation::Roll>(glm::radians(90.0f));

			auto e_left = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_left, std::move(platform_left), std::move(text_left), std::move(transform_left), std::move(collider), Plateform{});
		}

		{
			MeshComponent platform_right;
			platform_right.mesh = &MeshLoader::Load("Models/Enviro/floor.obj", m_window->App());;

			MaterialComponent text_right;

			text_right.materials.resize(platform_right.mesh->GetSubMeshesCount());

			for (int i = 0; i < platform_right.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_right;
				mat_right.baseColor = &TextureLoader::Load("Textures/Obstacles/BaseColor.png", m_window->App());
				mat_right.emissive = &TextureLoader::Load("Textures/Obstacles/Emissive.png", m_window->App());
				mat_right.normalMap = &TextureLoader::Load("Textures/Obstacles/Normal.png", m_window->App());
				mat_right.pbrMap = &TextureLoader::Load("Textures/Obstacles/ORM.png", m_window->App());

				text_right.materials[i] = mat_right;
			}


			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformRight", platform_right.mesh);

			TransformComponent transform_right;
			transform_right.SetPosition(worldCenter + glm::vec3{ 1,0,0 } * centerOffset);
			transform_right.SetScale({ 1.0f,1.0f,4.0f });
			transform_right.RotateEuler<RotData::Orientation::Roll>(glm::radians(-90.0f));

			auto e_right = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_right, std::move(platform_right), std::move(text_right), std::move(transform_right), std::move(collider), Plateform{});
		}
		{
			MeshComponent backGround;
			backGround.mesh = &MeshLoader::Load("Models/BackGround/mesh.obj", m_window->App());;

			MaterialComponent text_right;

			text_right.materials.resize(backGround.mesh->GetSubMeshesCount());

			for (int i = 0; i < backGround.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_right;
				mat_right.baseColor = &TextureLoader::Load("Textures/BackGround/BaseColor.png", m_window->App());
				mat_right.emissive = &TextureLoader::Load("Textures/BackGround/Emissive.png", m_window->App());
				mat_right.normalMap = &TextureLoader::Load("Textures/BackGround/Normal.png", m_window->App());
				mat_right.pbrMap = &TextureLoader::Load("Textures/BackGround/ORM.png", m_window->App());

				text_right.materials[i] = mat_right;
			}


			

			TransformComponent transform_right;
			transform_right.SetPosition(worldCenter - glm::vec3{0.0f, 0.0f, zValue});
			transform_right.SetScale({ 1.0f,1.0f,1.0f });

			auto e_right = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_right, std::move(backGround), std::move(text_right), std::move(transform_right), Plateform{});
		}
		
		{
			TransformComponent2d transform;
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			ui.SetPos({ 1830.0f, 90.0f });
			ui.SetScale({ 60,60 });
			auto e = m_ecs.CreateEntity();
			TextComp text;
			text.text.font = &FontLoader::Load("Fonts/Lazer84.ttf", m_window->App(), 6);
			text.text.SetText(" ");
			text.text.SetAlign(Text::Align::Center);
			text.text.textTexture = &TextureLoader::Load("Textures/PBC.png", m_window->App());


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(text), LiveText{});

		}

		{
			TransformComponent2d transform;
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			ui.SetPos({ 1750.0f, 25.0f });
			ui.SetScale({ 200,200 });
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/record.png", m_window->App());
			auto e = m_ecs.CreateEntity();

			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture));

		}

		// light
		{
			// the light need transform component and light component
			// all lights type have their own system to create them go in the file to understand
			LightComponent<LightData::Type::Point> lc = LightComponent<LightData::Type::Point>::Create({ 1,1,1 }, { 1,1,1 },100.0f, 100.0f);
			// set the transform but certain light need dir some position or both so just use what necessary 
			TransformComponent transform;
			transform.SetPosition(worldCenter);
			// same 
			auto e = m_ecs.CreateEntity();
			// same
			m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		}

		//// light
		//{
		//	// the light need transform component and light component
		//	// all lights type have their own system to create them go in the file to understand
		//	LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
		//	// set the transform but certain light need dir some position or both so just use what necessary 
		//	TransformComponent transform;
		//	transform.SetPosition(worldCenter + glm::vec3{ 0,-1,0 } * centerOffset);
		//	transform.LookAtDir({ 0,1,0 });
		//	// same 
		//	auto e = m_ecs.CreateEntity();
		//	// same
		//	m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		//}
		//// light
		//{
		//	// the light need transform component and light component
		//	// all lights type have their own system to create them go in the file to understand
		//	LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
		//	// set the transform but certain light need dir some position or both so just use what necessary 
		//	TransformComponent transform;
		//	transform.SetPosition(worldCenter + glm::vec3{ -1,0,0 } * centerOffset);
		//	transform.LookAtDir({ 1,0,0 });
		//	// same 
		//	auto e = m_ecs.CreateEntity();
		//	// same
		//	m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		//}
		//// light
		//{
		//	// the light need transform component and light component
		//	// all lights type have their own system to create them go in the file to understand
		//	LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
		//	// set the transform but certain light need dir some position or both so just use what necessary 
		//	TransformComponent transform;
		//	transform.SetPosition(worldCenter + glm::vec3{ -1,0,0 } * centerOffset);
		//	transform.LookAtDir({ -1,0,0 });
		//	// same 
		//	auto e = m_ecs.CreateEntity();
		//	// same
		//	m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		//}



		reg.Register('a', "Models/Obstacles/bloc_1x1.obj");
		reg.Register('z', "Models/Obstacles/bloc_1x2.obj");
		reg.Register('e', "Models/Obstacles/bloc_1x3.obj");
		reg.Register('q', "Models/Obstacles/bloc_2x1.obj");
		reg.Register('s', "Models/Obstacles/bloc_2x2.obj");
		reg.Register('d', "Models/Obstacles/bloc_2x3.obj");

		for (int i = 1 ; i <= maxFile; ++i)
		{
			GenLoader::Load("Level_" + std::to_string(i) + ".txt");
		}
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		score++;
		{
			auto es = m_ecs.GetAllComponentsView<TextComp, ScoreText>();
			for (auto e : es)
			{
				m_ecs.GetComponent<TextComp>(e).text.SetText(allign.Compute(score));
			}
			auto eLive = m_ecs.GetAllComponentsView<TextComp, LiveText>();
			for (auto e : eLive)
			{
				auto str = allignLive.Compute(life);
				std::string realStr = " " + str + " ";
				m_ecs.GetComponent<TextComp>(e).text.SetText(realStr);
			}
		}

		if (lastZ > genNeeded)
		{
			KGR::Tools::Random rd;
			//right
			{
				PosMapper mapper;
				mapper.midPos = worldCenter + centerOffset * glm::vec3{ 1,0,0 } + glm::vec3{ 0,0.0f,lastZ };
				mapper.radAngle = glm::radians(-90.0f);
				mapper.offset = offsetMove;
				mapper.rightVec = glm::vec3{ 0,1,0 };
				auto random = rd.getRandomNumber(1, maxFile);
				Generate(reg, m_ecs, mapper, GenLoader::Load("Level_" + std::to_string(random) + ".txt"), *m_window).z;
			}
			// left
			{
				PosMapper mapper;
				mapper.midPos = worldCenter - centerOffset * glm::vec3{ 1,0,0 } + glm::vec3{ 0,0.0f,lastZ };
				mapper.radAngle = glm::radians(90.0f);
				mapper.offset = offsetMove;
				mapper.rightVec = glm::vec3{ 0,1,0 };
				auto random = rd.getRandomNumber(1, maxFile);
				Generate(reg, m_ecs, mapper, GenLoader::Load("Level_" + std::to_string(random) + ".txt"), *m_window).z;
			}
			// up
			{
				PosMapper mapper;
				mapper.midPos = worldCenter + centerOffset * glm::vec3{0,1,0 } + glm::vec3{ 0,0.0f,lastZ };
				mapper.radAngle = glm::radians(-180.0f);
				mapper.offset = offsetMove;
				mapper.rightVec = glm::vec3{1 ,0,0 };
				auto random = rd.getRandomNumber(1, maxFile);
				Generate(reg, m_ecs, mapper, GenLoader::Load("Level_" + std::to_string(random) + ".txt"), *m_window).z;
			}
			//Dawn
			{
				PosMapper mapper;
				mapper.midPos = worldCenter - centerOffset * glm::vec3{ 0,1,0 } + glm::vec3{ 0,0.0f,lastZ };
				mapper.radAngle = 0.0f;
				mapper.offset = offsetMove;
				mapper.rightVec = glm::vec3{ 1 ,0,0 };
				auto random = rd.getRandomNumber(1, maxFile);
				lastZ += Generate(reg, m_ecs, mapper, GenLoader::Load("Level_" + std::to_string(random) + ".txt"), *m_window).z;
			}

			






		}


		{
			auto es = m_ecs.GetAllComponentsView<TransformComponent, controlComponentPlayer>();
			for (auto& e : es)
			{
				glm::vec3& PlayerTargetPos = m_ecs.GetComponent<controlComponentPlayer>(e).PlayerTargetPos;

				auto input = m_window->GetInputManager();
				auto camView = m_ecs.GetAllComponentsView<CameraComponent, TransformComponent>();
				auto& transform = m_ecs.GetComponent<TransformComponent>(e);
				auto& playerComp = m_ecs.GetComponent<playerComponent>(e);
				auto& gravity = m_ecs.GetComponent<PhysicComponent>(e);
				auto& colision = m_ecs.GetComponent<CollisionComp>(e);

				// changement de plateformes
				if (input->IsKeyPressed(KGR::Key::Up_arrow) && (playerComp.StartDir == DirectionState::BAS
					|| playerComp.StartDir == DirectionState::GAUCHE || playerComp.StartDir == DirectionState::DROITE))
				{
					playerComp.StartDir = DirectionState::HAUT;
					//playerComp.StartPos = PositionState::CENTRE;
				};
				if (input->IsKeyPressed(KGR::Key::Down_arrow) && (playerComp.StartDir == DirectionState::HAUT
					|| playerComp.StartDir == DirectionState::GAUCHE || playerComp.StartDir == DirectionState::DROITE))
				{
					playerComp.StartDir = DirectionState::BAS;
					//playerComp.StartPos = PositionState::CENTRE;
				};
				if (input->IsKeyPressed(KGR::Key::Left_arrow) && (playerComp.StartDir == DirectionState::BAS
					|| playerComp.StartDir == DirectionState::HAUT || playerComp.StartDir == DirectionState::DROITE))
				{
					playerComp.StartDir = DirectionState::GAUCHE;
					//playerComp.StartPos = PositionState::CENTRE;
				};
				if (input->IsKeyPressed(KGR::Key::Right_arrow) && (playerComp.StartDir == DirectionState::BAS
					|| playerComp.StartDir == DirectionState::HAUT || playerComp.StartDir == DirectionState::GAUCHE))
				{
					playerComp.StartDir = DirectionState::DROITE;
					//playerComp.StartPos = PositionState::CENTRE;
				};

				if (input->IsKeyPressed(KGR::Key::Q) || input->IsKeyPressed(KGR::Key::A))
				{
					if (playerComp.StartPos == PositionState::DROITE)
					{
						playerComp.StartPos = PositionState::CENTRE;
					}
					else if (playerComp.StartPos == PositionState::CENTRE)
					{
						playerComp.StartPos = PositionState::GAUCHE;
					}
				}
				if (input->IsKeyPressed(KGR::Key::D))
				{
					if (playerComp.StartPos == PositionState::GAUCHE)
					{
						playerComp.StartPos = PositionState::CENTRE;
					}
					else if (playerComp.StartPos == PositionState::CENTRE)
					{
						playerComp.StartPos = PositionState::DROITE;
					}
				}

				glm::vec2 position = getPosition(playerComp.StartDir, playerComp.StartPos);
				PlayerTargetPos.x = position.x;
				PlayerTargetPos.y = position.y;

				glm::vec3 currentPos = transform.GetPosition();

				switch (playerComp.StartDir)
				{
				case DirectionState::BAS:
				case DirectionState::HAUT:
					PlayerCurrentPos.x = glm::mix(PlayerCurrentPos.x, PlayerTargetPos.x, smoothPlayerSpeed * dt);
					currentPos.x = PlayerCurrentPos.x;
					break;

				case DirectionState::GAUCHE:
				case DirectionState::DROITE:
					PlayerCurrentPos.y = glm::mix(PlayerCurrentPos.y, PlayerTargetPos.y, smoothPlayerSpeed * dt);
					currentPos.y = PlayerCurrentPos.y;
					break;
				}

				transform.SetPosition(currentPos);



				glm::vec3 camTargetPos = { 0,0,0 };
				glm::vec3 camTargetLook = { 0,0,0 };

				for (const auto& camEntity : camView)
				{
					auto& camTransform = m_ecs.GetComponent<TransformComponent>(camEntity);
				
					switch (playerComp.StartDir)
					{
					case DirectionState::BAS:
						transform.SetRotation({ 0.0f,0.0f,0.0f });
						camTargetPos = worldCenter + glm::vec3{ 0,centerOffset / attenuationFactor,zValue };
						camTargetLook = worldCenter + glm::vec3{ 0,-centerOffset / attenuationFactor,-zValue };
						break;

					case DirectionState::HAUT:
						transform.SetRotation({ 0.0f,0.0f,glm::radians(180.0f) });
						camTargetPos = worldCenter + glm::vec3{ 0,-centerOffset / attenuationFactor,zValue };
						camTargetLook = worldCenter + glm::vec3{ 0,centerOffset / attenuationFactor,-zValue };
						break;

					case DirectionState::GAUCHE:
						transform.SetRotation({ 0.0f,0.0f,glm::radians(-90.0f) });

						camTargetPos = worldCenter + glm::vec3{ centerOffset / attenuationFactor,0,zValue };
						camTargetLook = worldCenter + glm::vec3{ -centerOffset / attenuationFactor,0,-zValue };
						break;

					case DirectionState::DROITE:
						transform.SetRotation({ 0.0f,0.0f,glm::radians(90.0f) });

						camTargetPos = worldCenter + glm::vec3{ -centerOffset / attenuationFactor,0,zValue };
						camTargetLook = worldCenter + glm::vec3{ centerOffset / attenuationFactor,0,-zValue };
						break;
					}

					camCurrentPos = glm::mix(camCurrentPos, camTargetPos, smoothCamSpeed * dt);
					camCurrentLook = glm::mix(camCurrentLook, camTargetLook, smoothCamSpeed * dt);

					camTransform.SetPosition(camCurrentPos);
					camTransform.LookAt(camCurrentLook);

					///////////////////////////////////////////////////////////////////////////////
				}

				glm::vec3 dir(0.0f);
				glm::vec3 normal(0.0f);
				switch (playerComp.StartDir)
				{
				case DirectionState::BAS: dir = { 0,-1,0 };break;
				case DirectionState::HAUT: dir = { 0,1,0 }; break;
				case DirectionState::GAUCHE: dir = { -1,0,0 }; break;
				case DirectionState::DROITE: dir = { 1,0,0 }; break;
				}
				switch (playerComp.StartDir)
				{
				case DirectionState::BAS: normal = { 0,1,0 }; break;
				case DirectionState::HAUT: normal = { 0,-1,0 }; break;
				case DirectionState::GAUCHE: normal = { 1,0,0 }; break;
				case DirectionState::DROITE: normal = { -1,0,0 }; break;
				}


				if (input->IsKeyDown(KGR::Key::Z) && gravity.getIsGround())
				{
					gravity.jump();
				}				

				gravity.uptadePhysique(dt);
				glm::vec3 move = dir * gravity.getVelocity() * dt;

				//if (glm::length(move) < 0.0001f)
				//	continue;

				transform.Translate(move);


				auto boxOBB = colision.collider->ComputeGlobalOBB(
					transform.GetScale(), transform.GetPosition(), transform.GetOrientation());

				//gravity.setIsGround(false);

				bool grounded = false;

				auto plateforms = m_ecs.GetAllComponentsView<Plateform, TransformComponent, CollisionComp>();
				for (auto p : plateforms)
				{
					auto& tP = m_ecs.GetComponent<TransformComponent>(p);
					auto& colP = m_ecs.GetComponent<CollisionComp>(p);

					auto pOBB = colP.collider->ComputeGlobalOBB(
						tP.GetScale(), tP.GetPosition(), tP.GetOrientation());

					auto collision = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(boxOBB, pOBB);
					if (collision.IsColliding() && gravity.getVelocity() >= 0.0f)
					{
						glm::vec3 playerPosDist = transform.GetPosition() - tP.GetPosition();
						bool isGroundSide = glm::dot(playerPosDist, dir) < 0;
						bool isFalling = gravity.getVelocity() >= 0.0f;

						if (isGroundSide && isFalling)
						{
							transform.Translate(-collision.GetPenetration() * collision.GetCollisionNormal());
							boxOBB = colision.collider->ComputeGlobalOBB(
								transform.GetScale(), transform.GetPosition(), transform.GetOrientation());
							grounded = true;
							gravity.resetVelocity();
						}
						else if (!isGroundSide)
						{
							transform.Translate(-collision.GetPenetration() * collision.GetCollisionNormal());
							boxOBB = colision.collider->ComputeGlobalOBB(
								transform.GetScale(), transform.GetPosition(), transform.GetOrientation());
							gravity.resetVelocity();
						}

					}
					
				}
				static float speed = 10.0f;
				{
					auto plateforms = m_ecs.GetAllComponentsView<PlatFormMove, TransformComponent, CollisionComp,LifeTimeComp>();
					for (auto p : plateforms)
					{
						
						auto& tP = m_ecs.GetComponent<TransformComponent>(p);
						tP.Translate(dt * speed
							* glm::vec3{ 0,0,1 });
						if (tP.GetPosition().z > zValue  * 4)
							m_ecs.GetComponent<LifeTimeComp>(p).isDead = true;
					}
					lastZ += dt * 1.0f * speed;
					
					bool isColiding = false;
					
					for (auto p : plateforms)
					{
						auto& tP = m_ecs.GetComponent<TransformComponent>(p);
						auto& colP = m_ecs.GetComponent<CollisionComp>(p);

						auto pOBB = colP.collider->ComputeGlobalOBB(
							tP.GetScale(), tP.GetPosition(), tP.GetOrientation());

						auto collision = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(boxOBB, pOBB);
						if (collision.IsColliding() && gravity.getVelocity() >= 0.0f)
						{
							glm::vec3 playerPosDist = transform.GetPosition() - tP.GetPosition();
							bool isGroundSide = glm::dot(playerPosDist, normal) > 0.7;
							bool isFalling = gravity.getVelocity() >= 0.0f;
							
							if (isGroundSide && isFalling)
							{
							
								transform.Translate(-collision.GetPenetration() * collision.GetCollisionNormal());
								boxOBB = colision.collider->ComputeGlobalOBB(
									transform.GetScale(), transform.GetPosition(), transform.GetOrientation());
								
								gravity.resetVelocity();
								grounded = true;
								break;
							}
							else if (!isGroundSide)
							{
								transform.Translate(-collision.GetPenetration() * collision.GetCollisionNormal());
								boxOBB = colision.collider->ComputeGlobalOBB(
									transform.GetScale(), transform.GetPosition(), transform.GetOrientation());
								gravity.resetVelocity();
								isColiding = true;
							
							}
							

						}

					}
					if (isColiding && !playerComp.isInvincible && !grounded)
					{
						--life;
						std::cout << "player life : " << life << std::endl;
						playerComp.isInvincible = true;
					}
					else if (!isColiding && playerComp.isInvincible)
					{
						playerComp.isInvincible = false;
					}
					if (life <= 0)
					{
						KGR::EventBus<ScoreAdded>::Notify({ .number = score });
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ "GameOver" });

					}
					transform.SetPosition(glm::vec3(transform.GetPosition().x, transform.GetPosition().y, worldCenter.z));
				}

				gravity.setIsGround(grounded);

			}


		}

		

		{
			auto input = m_window->GetInputManager();
			if (input->IsKeyDown(KGR::Key::P))
				KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ "Pause" });
		}
		{

			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
					u.SetColor({ 1,0,0,1 });
				else
					u.SetColor({ 0,1,0,1 });
			}
		}


		{
			auto es = m_ecs.GetAllComponentsView<LifeTimeComp>();
			for (auto e : es)
			{
				if (m_ecs.GetComponent<LifeTimeComp>(e).isDead)
					m_ecs.DestroyEntity(e);
			}
		}
		

	}
	void Render() override
	{
		IGameScene::Render();
	}
};

struct CSComp
{
	std::string targetScene;
};

struct ExitComp
{

};

struct OldScene
{
	std::string targetScene;
};

struct StartScene : public IGameScene
{

	StartScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time) {}

	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);
		{
			KGR::Audio::WavStreamComponent comp;
			comp.SetWav(KGR::Audio::WavStreamManager::Load("Musics/game_song.mp3"));
			comp.SetLoop(true);
			comp.Play();
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(comp));
		}

		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(45.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,3,5 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({0.0f,0.0f});
			// here the scale
			ui.SetScale({ 1920.0f , 1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_BG.png", m_window->App());
			
			// same as always 
			auto e = m_ecs.CreateEntity();


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture));

		}



		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f , 0.0f} );
			// here the scale
			ui.SetScale({ 1920.0f ,1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_window.png", m_window->App());
			// same as always 
			auto e = m_ecs.CreateEntity();



			
			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}));

		}

		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 301.1f, 439.0f });
			// here the scale
			ui.SetScale({ 589.1f,152.5f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Play button.png", m_window->App());
			CSComp comp;
			comp.targetScene = "Game";
			// same as always 
			auto e = m_ecs.CreateEntity();



			
			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp));

		}


		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 301.1f, 642 });
			// here the scale
			ui.SetScale({ 589.1f,152.5f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Quit button.png", m_window->App());
			
			// same as always 
			auto e = m_ecs.CreateEntity();




			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}),ExitComp {});

		}


		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 1207.0f, 693.0f });
			// here the scale
			ui.SetScale({ 370.0f, 240.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/record.png", m_window->App());
			CSComp comp;
			comp.targetScene = "Score";
			// same as always 
			auto e = m_ecs.CreateEntity();

			TextComp text;
			text.text.font = &FontLoader::Load("Fonts/Lazer84.ttf", m_window->App(), 6);
			text.text.SetText(" \n      Scores    \n ");
			text.text.SetAlign(Text::Align::Center);
			text.text.textTexture = &TextureLoader::Load("Textures/PBC.png", m_window->App());


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp),std::move(text));

		}
		// TODO create backGround
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		// TODO click on buttonaé 
		if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Right))
			KGR::EventBus<ScoreAdded>::Notify(ScoreAdded{ .number = 999 });
		{

			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, CSComp>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{
					
					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
					{
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ m_ecs.GetComponent<CSComp>(e).targetScene });
						if (m_ecs.GetComponent<CSComp>(e).targetScene == "Score")
							KGR::EventBus<OldScene>::Notify(OldScene{ "Start" });

					}
				}
			}
			{
				auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, ExitComp>();
				for (auto e : es)
				{
					auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
					auto& u = m_ecs.GetComponent<UiComponent>(e);
					t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

					if (t.aabb.IsColliding(mouseinAR))
					{
						if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
						KGR::EventBus <CloseEvent>::Notify({});
					}
				}
			}
		}
	}


};


struct PauseScene : public IGameScene
{

	PauseScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time) {}

	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);

		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(45.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,3,5 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f,0.0f });
			// here the scale
			ui.SetScale({ 1920.0f , 1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_BG.png", m_window->App());

			// same as always 
			auto e = m_ecs.CreateEntity();


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture));

		}



		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f , 0.0f });
			// here the scale
			ui.SetScale({ 1920.0f ,1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_window.png", m_window->App());
			// same as always 
			auto e = m_ecs.CreateEntity();




			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}));

		}

		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 301.1f, 439.0f });
			// here the scale
			ui.SetScale({ 589.1f,152.5f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Play button.png", m_window->App());
			CSComp comp;
			comp.targetScene = "Game";
			// same as always 
			auto e = m_ecs.CreateEntity();




			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp));

		}


		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 301.1f, 642 });
			// here the scale
			ui.SetScale({ 589.1f,152.5f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Quit button.png", m_window->App());

			// same as always 
			auto e = m_ecs.CreateEntity();

			


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), ExitComp{});

		}
		// TODO create backGround
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		// TODO click on buttonaé 

		
		{
			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, CSComp>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{

					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ m_ecs.GetComponent<CSComp>(e).targetScene });
				}
			}
			{
				auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, ExitComp>();
				for (auto e : es)
				{
					auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
					auto& u = m_ecs.GetComponent<UiComponent>(e);
					t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

					if (t.aabb.IsColliding(mouseinAR))
					{
						if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
							KGR::EventBus <CloseEvent>::Notify({});
					}
				}
			}
		}
	}


};

struct OldComp
{
};
struct ScoreTextComp
{
	
};
struct ScoreScene : public IGameScene
{

	bool ScoreDirty = true;
	std::string oldScene = "";
	std::string scoreFile = "test.txt";
	ScoreScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time)
	{
		KGR::EventBus<OldScene>::AddListener(this);
		KGR::EventBus<OldScene>::AddCallBack<ScoreScene>(&ScoreScene::SetOld);

		KGR::EventBus<ScoreAdded>::AddListener(this);
		KGR::EventBus<ScoreAdded>::AddCallBack<ScoreScene>(&ScoreScene::AddScore);
		
	}
~ScoreScene() override
	{
		ScoreLoader::Load(scoreFile).Save();
		KGR::EventBus<OldScene>::RemoveListener(this);
		KGR::EventBus<ScoreAdded>::RemoveListener(this);
	}
	void AddScore(const ScoreAdded& score)
	{
		ScoreLoader::Load(scoreFile).AddScore({ score.name,score.number });
		ScoreDirty = true;
	}
	void SetOld(const OldScene& old)
	{
		oldScene = old.targetScene;
	}
	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);
		ScoreLoader::Load(scoreFile);
		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(45.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,3,5 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}

		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f,0.0f });
			// here the scale
			ui.SetScale({ 1920.0f , 1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_BG_without_logo.png", m_window->App());

			// same as always 
			auto e = m_ecs.CreateEntity();


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture));

		}

		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f, 0.0f });
			// here the scale
			ui.SetScale({ 150.0f, 150.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/record.png", m_window->App());
			OldComp comp;
			
			// same as always 
			auto e = m_ecs.CreateEntity();

			TextComp text;
			text.text.font = &FontLoader::Load("Fonts/Lazer84.ttf", m_window->App(), 6);
			text.text.SetText(" \n      Return    \n ");
			text.text.SetAlign(Text::Align::Center);
			text.text.textTexture = &TextureLoader::Load("Textures/PBC.png", m_window->App());


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp), std::move(text));

		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 300.0f, 100.0f });
			// here the scale
			ui.SetScale({ 1500.0f, 800 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/record.png", m_window->App());
			OldComp comp;

			// same as always 
			auto e = m_ecs.CreateEntity();

			TextComp text;
			text.text.font = &FontLoader::Load("Fonts/Lazer84.ttf", m_window->App(), 6);
			text.text.SetText("  ");
			text.text.SetAlign(Text::Align::Left);
			text.text.textTexture = &TextureLoader::Load("Textures/PBC.png", m_window->App());


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp), std::move(text), ScoreTextComp{});

		}
		// TODO create backGround
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		// TODO click on buttonaé 

		{

			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, CSComp>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{

					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ m_ecs.GetComponent<CSComp>(e).targetScene });
				}
			}
			{
				auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, OldComp>();
				for (auto e : es)
				{
					auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
					auto& u = m_ecs.GetComponent<UiComponent>(e);
					t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

					if (t.aabb.IsColliding(mouseinAR))
					{
						if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
							KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ oldScene });
					}
				}
			}
			{
				auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, ExitComp>();
				for (auto e : es)
				{
					auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
					auto& u = m_ecs.GetComponent<UiComponent>(e);
					t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

					if (t.aabb.IsColliding(mouseinAR))
					{
						if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
							KGR::EventBus <CloseEvent>::Notify({});
					}
				}
			}

			{
				if (ScoreDirty)
				{
					auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, TextComp, ScoreTextComp>();
					for (auto e : es)
					{
							m_ecs.GetComponent<TextComp>(e).text.SetText(ToStr());
					}
					ScoreDirty = false;
				}
			}
		}
	}

	std::string ToStr()
	{
		auto score = ScoreLoader::Load(scoreFile).Get();
		std::string scoreTot =" \n \n ";
		for (int i = 0 ; i < score.size() ; ++i)
		{
			std::string addingScore = std::string("                         ") + score[i].first + std::string(" : ") + std::to_string(score[i].second);
			std::string addingSize = "";
			
			if (addingScore.size() < 100)
			{
				addingSize.resize(100 - addingScore.size(), ' ');
			}
			scoreTot += addingScore + addingSize + "\n";
		}
		scoreTot += "\n ";
		return scoreTot;

	}
};



struct GameOver : public IGameScene
{

	GameOver(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time) {}

	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);

		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(45.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,3,5 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f,0.0f });
			// here the scale
			ui.SetScale({ 1920.0f , 1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_BG.png", m_window->App());

			// same as always 
			auto e = m_ecs.CreateEntity();


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture));

		}



		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0.0f , 0.0f });
			// here the scale
			ui.SetScale({ 1920.0f ,1080.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Menu_window.png", m_window->App());
			// same as always 
			auto e = m_ecs.CreateEntity();




			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}));

		}



		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 301.1f, 642 });
			// here the scale
			ui.SetScale({ 589.1f,152.5f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/Quit button.png", m_window->App());

			// same as always 
			auto e = m_ecs.CreateEntity();




			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), ExitComp{});

		}


		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 1207.0f, 693.0f });
			// here the scale
			ui.SetScale({ 370.0f, 240.0f });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu/record.png", m_window->App());
			CSComp comp;
			comp.targetScene = "Score";
			// same as always 
			auto e = m_ecs.CreateEntity();

			TextComp text;
			text.text.font = &FontLoader::Load("Fonts/Lazer84.ttf", m_window->App(), 6);
			text.text.SetText(" \n      Scores    \n ");
			text.text.SetAlign(Text::Align::Center);
			text.text.textTexture = &TextureLoader::Load("Textures/PBC.png", m_window->App());


			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp), std::move(text));

		}
		// TODO create backGround
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		// TODO click on buttonaé 
		if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Right))
			KGR::EventBus<ScoreAdded>::Notify(ScoreAdded{ .number = 999 });
		{

			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, CSComp>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{

					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
					{
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{ m_ecs.GetComponent<CSComp>(e).targetScene });
						if (m_ecs.GetComponent<CSComp>(e).targetScene == "Score")
							KGR::EventBus<OldScene>::Notify(OldScene{ "GameOver" });

					}
				}
			}
			{
				auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, ExitComp>();
				for (auto e : es)
				{
					auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
					auto& u = m_ecs.GetComponent<UiComponent>(e);
					t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

					if (t.aabb.IsColliding(mouseinAR))
					{
						if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
							KGR::EventBus <CloseEvent>::Notify({});
					}
				}
			}
		}
	}


};
