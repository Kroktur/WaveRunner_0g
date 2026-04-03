#pragma once
#include "Core/Scene.h"
#include <filesystem>

#include "Audio/SoundComponent.h"
#include "Core/SceneManager.h"
#include "Core/Scene.h"

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

struct playerComponent
{
	DirectionState StartDir = DirectionState::BAS;
	PositionState StartPos = PositionState::CENTRE;
	bool isMoove = false;
	float distMoove = 0;
	glm::vec3 actualPos;
	glm::vec3 targetPos;

	float life = 3;
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

struct GameSceneManager : public SceneManager
{
	GameSceneManager(const std::filesystem::path& path)
	{
		KGR::RenderWindow::Init();
		m_window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,800 }, "My_Super_Mega_Duper_Projet_De_La_Mort_Qui_Tue_!!", path);
		KGR::Audio::WavComponent::Init();
		KGR::Audio::WavStreamComponent::Init();
		KGR::EventBus<ChangeSceneEvent>::AddListener(this);
		KGR::EventBus<ChangeSceneEvent>::AddCallBack<GameSceneManager>(&GameSceneManager::ChangeScene);
	}
	~GameSceneManager() override
	{
		KGR::EventBus<ChangeSceneEvent>::RemoveListener(this);
	}
	KGR::RenderWindow* GetWindow() const 
	{
		return m_window.get();
	}
	bool LoopCondition() const override
	{
		return !m_window->ShouldClose();
	}
	void Destroy() override
	{
		m_window->Destroy();
		KGR::RenderWindow::End();
	}
	void ChangeScene(const ChangeSceneEvent& event)
	{
		SetCurrentScene(event.targetScene);
	}
private:
	std::unique_ptr<KGR::RenderWindow> m_window;
};

struct IGameScene : public Scene
{
	IGameScene(const KGR::Tools::Chrono<float>::Time& time):Scene(time),m_window(nullptr){}
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
			if (es.Size() != 1)
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
		m_window->Render({ 0.53f, 0.81f, 0.92f, 1.0f });
	}
protected:
	ecsType m_ecs;
	KGR::RenderWindow* m_window;
};


struct GameScene : public IGameScene
{
	float spawnTimer = 0.0f;
	float spawnInterval = 2.0f;
	float obstacleCount = 0;

	GameScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time){}
	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);
		// camera 
		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(90.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,5,7 });
			transform.LookAt({ 0,1,2 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}


		// mesh
		{
			// a mesh need a meshComponent a transform and a texture 

			// create a mesh and load it with the cash loader
			MeshComponent platform_down;
			platform_down.mesh = &MeshLoader::Load("Models/cube.obj", m_window->App());

			// create a texture 
			MaterialComponent text_down;
			// allocate the size of the texture must be the same as the number of submeshes 
			text_down.materials.resize(platform_down.mesh->GetSubMeshesCount());
			// then fill the texture ( this system need to be refact but for now you need to do it like that
			for (int i = 0; i < platform_down.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_down;
				mat_down.baseColor = &TextureLoader::Load("Textures/H1_H2_bloc_BaseColor_Emissive.png", m_window->App());

				text_down.materials[i] = mat_down;
			}

			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformDown", platform_down.mesh);

			// create the transform and set all the data
			TransformComponent transform_down;
			transform_down.SetPosition({ 0,-1,0 });
			transform_down.SetScale({ 6.0f,0.3f,20.0f });
			// same create an entity / id
			auto e_down = m_ecs.CreateEntity();
			// fill the component
			m_ecs.AddComponents(e_down, std::move(platform_down), std::move(text_down), std::move(transform_down), std::move(collider), Plateform{});
		}

		{
			MeshComponent platform_up;
			platform_up.mesh = &MeshLoader::Load("Models/cube.obj", m_window->App());

			MaterialComponent text_up;

			text_up.materials.resize(platform_up.mesh->GetSubMeshesCount());

			for (int i = 0; i < platform_up.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_up;
				mat_up.baseColor = &TextureLoader::Load("Textures/H1_H2_bloc_BaseColor_Emissive.png", m_window->App());

				text_up.materials[i] = mat_up;
			}


			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformUp", platform_up.mesh);

			TransformComponent transform_up;
			transform_up.SetPosition({ 0,6,0 });
			transform_up.SetScale({ 6.0f,0.3f,20.0f });

			auto e_up = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_up, std::move(platform_up), std::move(text_up), std::move(transform_up), std::move(collider), Plateform{});
		}

		{
			MeshComponent platform_left;
			platform_left.mesh = &MeshLoader::Load("Models/cube.obj", m_window->App());

			MaterialComponent text_left;

			text_left.materials.resize(platform_left.mesh->GetSubMeshesCount());

			for (int i = 0; i < platform_left.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_left;
				mat_left.baseColor = &TextureLoader::Load("Textures/H1_H2_bloc_BaseColor_Emissive.png", m_window->App());

				text_left.materials[i] = mat_left;
			}


			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformLeft", platform_left.mesh);

			TransformComponent transform_left;
			transform_left.SetPosition({ -5,2,0 });
			transform_left.SetScale({ 6.0f,0.3f,20.0f });
			transform_left.RotateEuler<RotData::Orientation::Roll>(glm::radians(90.0f));

			auto e_left = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_left, std::move(platform_left), std::move(text_left), std::move(transform_left), std::move(collider), Plateform{});
		}

		{
			MeshComponent platform_right;
			platform_right.mesh = &MeshLoader::Load("Models/cube.obj", m_window->App());

			MaterialComponent text_right;

			text_right.materials.resize(platform_right.mesh->GetSubMeshesCount());

			for (int i = 0; i < platform_right.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_right;
				mat_right.baseColor = &TextureLoader::Load("Textures/H1_H2_bloc_BaseColor_Emissive.png", m_window->App());

				text_right.materials[i] = mat_right;
			}


			CollisionComp collider;
			collider.collider = &ColliderManager::Load("plateformRight", platform_right.mesh);

			TransformComponent transform_right;
			transform_right.SetPosition({ 5,2,0 });
			transform_right.SetScale({ 6.0f,0.3f,20.0f });
			transform_right.RotateEuler<RotData::Orientation::Roll>(glm::radians(-90.0f));

			auto e_right = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_right, std::move(platform_right), std::move(text_right), std::move(transform_right), std::move(collider), Plateform{});
		}

		{
			MeshComponent player;
			player.mesh = &MeshLoader::Load("Models/cube.obj", m_window->App());

			MaterialComponent text_player;

			text_player.materials.resize(player.mesh->GetSubMeshesCount());

			for (int i = 0; i < player.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat_player;
				mat_player.baseColor = &TextureLoader::Load("Textures/H1_H2_bloc_BaseColor_Emissive.png", m_window->App());

				text_player.materials[i] = mat_player;
			}
			TransformComponent transform_player;
			transform_player.SetPosition({ 0,0,2 });
			transform_player.SetScale({ 1.0f,1.0f,1.0f });

			controlComponentPlayer player_input;
			playerComponent player_comp;

			PhysicComponent gravity;
			CollisionComp collider;
			collider.collider = &ColliderManager::Load("playerCollider", player.mesh);

			auto e_player = m_ecs.CreateEntity();

			m_ecs.AddComponents(e_player, std::move(player), std::move(text_player), std::move(transform_player),std::move(player_input),std::move(player_comp), std::move(gravity), std::move(collider));
		}

		// light
		{
			// the light need transform component and light component
			// all lights type have their own system to create them go in the file to understand
			LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
			// set the transform but certain light need dir some position or both so just use what necessary 
			TransformComponent transform;
			transform.SetPosition({ 0,5,0 });
			transform.LookAtDir({ 0,-1,0 });
			// same 
			auto e = m_ecs.CreateEntity();
			// same
			m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		}

		// light
		{
			// the light need transform component and light component
			// all lights type have their own system to create them go in the file to understand
			LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
			// set the transform but certain light need dir some position or both so just use what necessary 
			TransformComponent transform;
			transform.SetPosition({ 0,5,0 });
			transform.LookAtDir({ 0,1,0 });
			// same 
			auto e = m_ecs.CreateEntity();
			// same
			m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		}
		// light
		{
			// the light need transform component and light component
			// all lights type have their own system to create them go in the file to understand
			LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
			// set the transform but certain light need dir some position or both so just use what necessary 
			TransformComponent transform;
			transform.SetPosition({ 0,5,0 });
			transform.LookAtDir({ 1,0,0 });
			// same 
			auto e = m_ecs.CreateEntity();
			// same
			m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		}
		// light
		{
			// the light need transform component and light component
			// all lights type have their own system to create them go in the file to understand
			LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 100.0f);
			// set the transform but certain light need dir some position or both so just use what necessary 
			TransformComponent transform;
			transform.SetPosition({ 0,5,0 });
			transform.LookAtDir({ -1,0,0 });
			// same 
			auto e = m_ecs.CreateEntity();
			// same
			m_ecs.AddComponents(e, std::move(lc), std::move(transform));
		}

		// ui ( not fully operational)
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::LeftTop);
			// here set the position in the virtual resolution
			ui.SetPos({ 0, 0 });
			// here the scale
			ui.SetScale({ 200,200 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/texture.jpg", m_window->App());

			// same as always 
			auto e = m_ecs.CreateEntity();
			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}));

		}
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);

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

				if (!isGravity)
				{
					switch (playerComp.StartDir)
					{
					case DirectionState::BAS:
						if (input->IsKeyPressed(KGR::Key::Up_arrow)) {
							playerComp.StartDir = DirectionState::HAUT;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 0.0f,5.0f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Left_arrow)) {
							playerComp.StartDir = DirectionState::GAUCHE;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { -4.0f,2.5f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Right_arrow)) {
							playerComp.StartDir = DirectionState::DROITE;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 4.0f,2.5f,2.0f };
						}

						switch (playerComp.StartPos)
						{
						case PositionState::CENTRE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { -1.30f, 0.0f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { -1.30f, 0.0f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::DROITE;
								PlayerTargetPos = { 1.30f, 0.0f, 2.0 };
							}
							break;
						case PositionState::GAUCHE:
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 0.0f, 0.0f, 2.0f };
							}
							break;
						case PositionState::DROITE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 0.0f, 0.0f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 0.0f, 0.0f, 2.0f };
							}
							break;
						}
						break;

					case DirectionState::HAUT:
						if (input->IsKeyPressed(KGR::Key::Left_arrow)) {
							playerComp.StartDir = DirectionState::GAUCHE;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { -4.0f,2.5f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Down_arrow)) {
							playerComp.StartDir = DirectionState::BAS;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 0.0f,0.0f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Right_arrow)) {
							playerComp.StartDir = DirectionState::DROITE;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 4.0f,2.5f,2.0f };
						}

						switch (playerComp.StartPos)
						{
						case PositionState::CENTRE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { 1.30f, 5.0f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { 1.30f, 5.0f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::DROITE;
								PlayerTargetPos = { -1.30f, 5.0f, 2.0f };
							}
							break;
						case PositionState::GAUCHE:
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 0.0f, 5.0f, 2.0f };
							}
							break;
						case PositionState::DROITE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 0.0f, 5.0f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 0.0f, 5.0f, 2.0f };
							}
							break;
						}
						break;

					case DirectionState::GAUCHE:
						if (input->IsKeyPressed(KGR::Key::Up_arrow)) {
							playerComp.StartDir = DirectionState::HAUT;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 0.0f,5.0f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Down_arrow)) {
							playerComp.StartDir = DirectionState::BAS;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 0.0f,0.0f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Right_arrow)) {
							playerComp.StartDir = DirectionState::DROITE;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 4.0f,2.5f,2.0f };
						}

						switch (playerComp.StartPos)
						{
						case PositionState::CENTRE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { -4.0f, 3.80f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { -4.0f, 3.80f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::DROITE;
								PlayerTargetPos = { -4.0f, 1.20f, 2.0f };
							}
							break;
						case PositionState::GAUCHE:
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { -4.0f,2.5f,2.0f };
							}
							break;
						case PositionState::DROITE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { -4.0f,2.5f,2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { -4.0f,2.5f,2.0f };
							}
							break;
						}
						break;

					case DirectionState::DROITE:
						if (input->IsKeyPressed(KGR::Key::Up_arrow)) {
							playerComp.StartDir = DirectionState::HAUT;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 0.0f,5.0f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Left_arrow)) {
							playerComp.StartDir = DirectionState::GAUCHE;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { -4.0f,2.5f,2.0f };
						}
						if (input->IsKeyPressed(KGR::Key::Down_arrow)) {
							playerComp.StartDir = DirectionState::BAS;
							playerComp.StartPos = PositionState::CENTRE;
							PlayerTargetPos = { 0.0f,0.0f,2.0f };
						}

						switch (playerComp.StartPos)
						{
						case PositionState::CENTRE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { 4.0f, 1.20f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::GAUCHE;
								PlayerTargetPos = { 4.0f, 1.20f, 2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::DROITE;
								PlayerTargetPos = { 4.0f, 3.80f, 2.0f };
							}
							break;
						case PositionState::GAUCHE:
							if (input->IsKeyPressed(KGR::Key::D)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 4.0f,2.5f,2.0f };
							}
							break;
						case PositionState::DROITE:
							if (input->IsKeyPressed(KGR::Key::Q)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 4.0f,2.5f,2.0f };
							}
							if (input->IsKeyPressed(KGR::Key::A)) {
								playerComp.StartPos = PositionState::CENTRE;
								PlayerTargetPos = { 4.0f,2.5f,2.0f };
							}
							break;
						}
						break;
					}

					PlayerCurrentPos = glm::mix(PlayerCurrentPos, PlayerTargetPos, smoothPlayerSpeed * dt);
					m_ecs.GetComponent<TransformComponent>(e).SetPosition(PlayerCurrentPos);

				}

				
				glm::vec3 camTargetPos = { 0,0,0 };
				glm::vec3 camTargetLook = { 0,0,0 };

				for (const auto& camEntity : camView)
				{
					auto& camTransform = m_ecs.GetComponent<TransformComponent>(camEntity);

					switch (playerComp.StartDir)
					{
					case DirectionState::BAS:
						camTargetPos = { 0,3,7 };
						camTargetLook = { 0, -5, -8 };
						break;

					case DirectionState::HAUT:
						camTargetPos = { 0,3,7 };
						camTargetLook = { 0, 5, -8 };
						break;

					case DirectionState::GAUCHE:
						camTargetPos = { 0,3,7 };
						camTargetLook = { -10, 0, -8 };
						break;

					case DirectionState::DROITE:
						camTargetPos = { 0,3,7 };
						camTargetLook = { 10, 0, -8 };
						break;
					}

					camCurrentPos = glm::mix(camCurrentPos, camTargetPos, smoothCamSpeed * dt);
					camCurrentLook = glm::mix(camCurrentLook, camTargetLook, smoothCamSpeed * dt);

					camTransform.SetPosition(camCurrentPos);
					camTransform.LookAt(camCurrentLook);

					///////////////////////////////////////////////////////////////////////////////
					glm::vec3 dir(0.0f);
			
					switch (playerComp.StartDir)
					{
					case DirectionState::BAS: dir = { 0,-1,0 }; break;
					case DirectionState::HAUT: dir = { 0,1,0 }; break;
					case DirectionState::GAUCHE: dir = { -1,0,0 }; break;
					case DirectionState::DROITE: dir = { 1,0,0 }; break;
					}


					if (input->IsKeyDown(KGR::Key::Z))
					{
						isGravity = true;
						gravity.jump();
					}
					if (gravity.getVelocity() == 0.0f)
					{
						isGravity = false;
					}

					gravity.uptadePhysique(dt);
					glm::vec3 move = dir * gravity.getVelocity() * dt;
					transform.Translate(move);

					auto boxOBB = colision.collider->ComputeGlobalOBB(
						transform.GetScale(), transform.GetPosition(), transform.GetOrientation());

					gravity.setIsGround(false);

					auto plateforms = m_ecs.GetAllComponentsView<Plateform, TransformComponent, CollisionComp>();
					for (auto p : plateforms)
					{
						auto& tP = m_ecs.GetComponent<TransformComponent>(p);
						auto& colP = m_ecs.GetComponent<CollisionComp>(p);

						auto pOBB = colP.collider->ComputeGlobalOBB(
							tP.GetScale(), tP.GetPosition(), tP.GetOrientation());

						auto collision = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(boxOBB, pOBB);
						if (collision.IsColliding())
						{
							transform.Translate(-move);
							gravity.resetVelocity();
							gravity.setIsGround(true);
						}
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////

		spawnTimer += dt;
		if (spawnTimer >= spawnInterval)
		{
			spawnTimer = 0.0f;
			spawnObstacle();
		}

		glm::vec3 playerPos(0.0f);
		auto playerView = m_ecs.GetAllComponentsView<playerComponent, TransformComponent>();
		for (auto& player : playerView)
			playerPos = m_ecs.GetComponent<TransformComponent>(player).GetPosition();
		std::vector<decltype(m_ecs.CreateEntity())> toDestroy;

		auto obsView = m_ecs.GetAllComponentsView<Obstacle, TransformComponent>();

		for (auto obstacle : obsView)
		{
			auto& obsTransform = m_ecs.GetComponent<TransformComponent>(obstacle);
			auto& obs = m_ecs.GetComponent<Obstacle>(obstacle);
			auto& obsCol = m_ecs.GetComponent<CollisionComp>(obstacle);

			obsTransform.Translate({ 0.0f, 0.0f, obs.velocityObstacle * dt });

			if (obsTransform.GetPosition().z > 10.0f)
			{
				toDestroy.push_back(obstacle);
				continue;
			}
		}

		for (auto& obstacle : toDestroy)
			m_ecs.DestroyEntity(obstacle);


		{
			auto input = m_window->GetInputManager();
			if (input->IsKeyDown(KGR::Key::P))
				KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{"Menu"});
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

	}
	void Render() override
	{
		IGameScene::Render();
	}

	void spawn(glm::vec3 position, const std::string& meshPath, const std::string& texturePath)
	{
		MeshComponent mesh;
		mesh.mesh = &MeshLoader::Load(meshPath, m_window->App());

		MaterialComponent material;
		material.materials.resize(mesh.mesh->GetSubMeshesCount());


		for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
		{
			Material mat;
			mat.baseColor = &TextureLoader::Load(texturePath, m_window->App());

			material.materials[i] = mat;
		}

		TransformComponent transform;
		transform.SetPosition(position);
		transform.SetScale({ 0.8, 0.8, 0.8 });

		CollisionComp collider;
		std::string nameCollision = "obstacle_" + std::to_string(obstacleCount++);

		collider.collider = &ColliderManager::Load(nameCollision, mesh.mesh);

		auto e = m_ecs.CreateEntity();
		m_ecs.AddComponents(e, std::move(mesh), std::move(material), std::move(transform), std::move(collider), Obstacle{});
	};

	void spawnObstacle()
	{
		float lanesX[3] = { -1.3f, 0.0f, 1.3f };
		float lanesY[3] = { 1.2f, 2.5f, 3.8f };

		std::vector<std::pair<std::string, std::string>> mesh =
		{
			 {"Models/cube.obj", "Textures/test_mat_bc.png"}
			,{ "Models/all_obstacle.obj", "Textures/test_mat_bc.png"}
			,{"Models/bloc_L1_H1.obj", "Textures/test_mat_bc.png"}
		};
		

		int randomMeshBas = rand() % mesh.size();
		int randomMeshHaut = rand() % mesh.size();
		int randomMeshDroite = rand() % mesh.size();
		int randomMeshGauche = rand() % mesh.size();

		spawn({ lanesX[rand() % 3],  0.0f, -20.0f }, mesh[randomMeshBas].first, mesh[randomMeshBas].second);
		spawn({ lanesX[rand() % 3],   6.0f, -20.0f }, mesh[randomMeshHaut].first, mesh[randomMeshHaut].second);
		spawn({ -7.0f, lanesY[rand() % 3], -20.0f }, mesh[randomMeshGauche].first, mesh[randomMeshGauche].second);
		spawn({ 7.0f, lanesY[rand() % 3], -20.0f }, mesh[randomMeshDroite].first, mesh[randomMeshDroite].second);
	}
};

struct CSComp
{
	std::string targetScene;
};

struct MenuScene : public IGameScene
{

	MenuScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time) {}

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
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
			// here set the position in the virtual resolution
			ui.SetPos({ 1920.0f/2.0f, 1080.0f/2.0f });
			// here the scale
			ui.SetScale({ 500,500 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/texture.jpg", m_window->App());
			CSComp comp;
			comp.targetScene = "Game";
			// same as always 
			auto e = m_ecs.CreateEntity();
			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}),std::move(comp));

		}
		// TODO create backGround
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		// TODO click on button

		{

			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent,CSComp>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{
					u.SetColor({ 1,0,0,1 });
					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{m_ecs.GetComponent<CSComp>(e).targetScene});
				}
				else
					u.SetColor({ 0,1,0,1 });
			}
		}
	}


};

