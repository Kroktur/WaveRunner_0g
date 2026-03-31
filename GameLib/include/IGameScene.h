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

#include "Core/Materials.h"
#include "EventBus.h"
#include "Core/Font.h"
using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;


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
		SceneManager::Destroy();
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
		m_window->Render({ 0.53f, 0.81f, 0.92f, 1.0f });
	}
protected:
	ecsType m_ecs;
	KGR::RenderWindow* m_window;
};

struct control
{
	
};


struct GameScene : public IGameScene
{
	
	GameScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time){}
	void Init(SceneManager* manager) override
	{
		

		IGameScene::Init(manager);
		// camera 
		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(45.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100000.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,3,5 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}


		
		
		for (int i = 0 ; i < 700; ++i)
		{
			// a mesh need a meshComponent a transform and a texture 

			// create a mesh and load it with the cash loader
			MeshComponent mesh;
			mesh.mesh = &MeshLoader::Load("Models/CUBE.obj", m_window->App());

			// create a texture 
			MaterialComponent text;
			// allocate the size of the texture must be the same as the number of submeshes 
			text.materials.resize(mesh.mesh->GetSubMeshesCount());
			// then fill the texture ( this system need to be refact but for now you need to do it like that
			for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat;
				mat.baseColor = &TextureLoader::Load("Textures/bloc_BaseColor_Emissive.png", m_window->App());
				mat.emissive = &TextureLoader::Load("Textures/bloc_BaseColor_Emissive.png", m_window->App());
				mat.normalMap = &TextureLoader::Load("Textures/bloc_Normal.png", m_window->App());
				mat.pbrMap = &TextureLoader::Load("Textures/bloc_ORM.png", m_window->App());





				text.materials[i] = mat;
			}

			// create the transform and set all the data
			TransformComponent transform;
			transform.SetPosition({ 0,0,0 });
			transform.SetScale({ 3.0f, 3.0f,3.0f });
			// same create an entity / id
			auto e = m_ecs.CreateEntity();
			// fill the component
			m_ecs.AddComponents(e, std::move(mesh), std::move(text), std::move(transform));
		}


		{
			// a mesh need a meshComponent a transform and a texture 

			// create a mesh and load it with the cash loader
			MeshComponent mesh;
			mesh.mesh = &MeshLoader::Load("Models/bloc.obj", m_window->App());

			// create a texture 
			MaterialComponent text;
			// allocate the size of the texture must be the same as the number of submeshes 
			text.materials.resize(mesh.mesh->GetSubMeshesCount());
			// then fill the texture ( this system need to be refact but for now you need to do it like that
			for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat;
				mat.baseColor = &TextureLoader::Load("Textures/bloc_BaseColor_Emissive.png", m_window->App());
				mat.emissive = &TextureLoader::Load("Textures/bloc_BaseColor_Emissive.png", m_window->App());
				mat.normalMap = &TextureLoader::Load("Textures/bloc_Normal.png", m_window->App());
				mat.pbrMap = &TextureLoader::Load("Textures/bloc_ORM.png", m_window->App());





				text.materials[i] = mat;
			}

			// create the transform and set all the data
			TransformComponent transform;
			transform.SetPosition({ 0,0,0 });
			transform.SetScale({ 3.0f, 3.0f,3.0f });
			// same create an entity / id
			auto e = m_ecs.CreateEntity();
			// fill the component
			m_ecs.AddComponents(e, std::move(mesh), std::move(text), std::move(transform), std::move(control{}));
		}



		// light
		{
			// the light need transform component and light component
			// all lights type have their own system to create them go in the file to understand
			LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1, 1,1 }, { 1,1,1 }, 10.0f);
			// set the transform but certain light need dir some position or both so just use what necessary 
			TransformComponent transform;
			transform.SetPosition({ 0,0,0 });
			transform.LookAtDir({ 0,-1,0 });
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
			auto es = m_ecs.GetAllComponentsView<MeshComponent, TransformComponent, control>();
			for (auto& e : es)
			{
				auto input = m_window->GetInputManager();

				static float speed = 25.0f;
				if (input->IsKeyDown(KGR::Key::Q))
					m_ecs.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Yaw>(glm::radians(speed * dt));
				if (input->IsKeyDown(KGR::Key::D))
					m_ecs.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Yaw>(glm::radians(-speed * dt));

				if (input->IsKeyDown(KGR::Key::Z))
					m_ecs.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Pitch>(glm::radians(-speed * dt));
				if (input->IsKeyDown(KGR::Key::S))
					m_ecs.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Pitch>(glm::radians(speed * dt));



				if (input->IsKeyDown(KGR::Key::A))
					m_ecs.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Roll>(glm::radians(-speed * dt));
				if (input->IsKeyDown(KGR::Key::E))
					m_ecs.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Roll>(glm::radians(speed * dt));
			}

		}
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
			ui.SetScale({ 1000,500 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/texture.jpg", m_window->App());
			CSComp comp;
			comp.targetScene = "Game";
			// same as always 
			auto e = m_ecs.CreateEntity();
			TextComp text;
			//text.text.font = &FontLoader::Load("Fonts/arial.ttf", m_window->App());
			text.text.SetText( "je pense donc je suis !\nje mange des arbres ");
			text.text.textTexture = &TextureLoader::Load("Textures/viking_room.png", m_window->App());
			text.text.SetAlign(Text::Align::Center);



			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}),std::move(comp),std::move(text));

		}
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