#include <iostream>

float VirtualToNDC_X(float x, float virtualWidth, float aspectRatio,bool scale = false)
{
	return (x / virtualWidth) * (2.0f * aspectRatio) - (scale == false ? aspectRatio : 0.0f);
}

float VirtualToNDC_Y(float y, float virtualHeight, bool scale = false)
{
	return (y / virtualHeight) * 2.0f - (scale == false? 1.0f : 0.0f);
}

#include "Game.h"
#include "Core/CameraComponent.h"
int main(int argc, char** argv)
{

	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	KGR::RenderWindow::Init();
	std::unique_ptr<KGR::RenderWindow> window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,800 }, "test", projectRoot / "Ressources");
	window->GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);

	ecsType registry = ecsType{};


	{
		CameraComponent cam = CameraComponent::Create(glm::radians(45.0f),window->GetSize().x,window->GetSize().y,0.01f,100.0f,CameraComponent::Type::Perspective);
		TransformComponent transform;
		transform.SetPosition({ 0,3,5 });
		transform.LookAt({ 0,0,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(cam), std::move(transform));
	}


	{
		MeshComponent mesh;
		mesh.mesh = &MeshLoader::Load("Models/cube.obj",window->App());

		TextureComponent text;
		text.SetSize(mesh.mesh->GetSubMeshesCount());
		for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
			text.AddTexture(i, &TextureLoader::Load("Textures/viking_room.png", window->App()));

		TransformComponent transform;
		transform.SetPosition({ 0,0,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(mesh), std::move(text), std::move(transform));
	}

	{
		LightComponent<LightData::Type::Spot> lc = LightComponent<LightData::Type::Spot>::Create({ 1,0,1 }, { 1,1,1 }, 10.0f,100.0f,glm::radians(5.0f),0.15f);
		TransformComponent transform;
		transform.SetPosition({ 0,5,0 });
		transform.LookAtDir({ 0,-1,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(lc), std::move(transform));
	}

	float current = 0.0f;
	KGR::Tools::Chrono<float> chrono;
	while (!window->ShouldClose())
	{
		float actual = chrono.GetElapsedTime().AsSeconds();
		float dt = actual - current;
		current = actual;
		

		{
			auto es = registry.GetAllComponentsView<MeshComponent,TransformComponent>();
			for (auto& e : es)
			{
				auto input = window->GetInputManager();

				static float speed = 25.0f;
				if (input->IsKeyDown(KGR::Key::Q))
					registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Yaw>(glm::radians(speed * dt));
				if (input->IsKeyDown(KGR::Key::D))
					registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Yaw>(glm::radians(-speed * dt));

				if (input->IsKeyDown(KGR::Key::Z))
					registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Pitch>(glm::radians(-speed * dt));
				if (input->IsKeyDown(KGR::Key::S))
					registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Pitch>(glm::radians(speed * dt));



				if (input->IsKeyDown(KGR::Key::A))
					registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Roll>(glm::radians(-speed * dt));
				if (input->IsKeyDown(KGR::Key::E))
					registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Roll>(glm::radians(speed * dt));
			}

		}

		KGR::RenderWindow::PollEvent();
		window->Update();

		{
			auto es = registry.GetAllComponentsView<CameraComponent, TransformComponent>();
			if (es.Size() != 1)
				throw std::runtime_error("need one and one cam");
			for (auto& e : es)
			{
				registry.GetComponent<CameraComponent>(e).UpdateCamera(registry.GetComponent<TransformComponent>(e).GetFullTransform());
				registry.GetComponent<CameraComponent>(e).SetAspect(window->GetSize().x, window->GetSize().y);
				window->RegisterCam(registry.GetComponent<CameraComponent>(e), registry.GetComponent<TransformComponent>(e));
			}
		}


		{
			auto es = registry.GetAllComponentsView<MeshComponent, TransformComponent, TextureComponent>();
			for (auto& e : es)
			{
				window->RegisterRender(
					registry.GetComponent<MeshComponent>(e),
					registry.GetComponent<TransformComponent>(e),
					registry.GetComponent<TextureComponent>(e));

				auto& t = registry.GetComponent<TransformComponent>(e);
			}

		}


		

		{
			auto es = registry.GetAllComponentsView<LightComponent<LightData::Type::Point>, TransformComponent>();
			for (auto& e : es)
				window->RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Point>>(e), registry.GetComponent<TransformComponent>(e));
		}
		{
			auto es = registry.GetAllComponentsView<LightComponent<LightData::Type::Spot>, TransformComponent>();
			for (auto& e : es)
				window->RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Spot>>(e), registry.GetComponent<TransformComponent>(e));
		}
		{
			auto es = registry.GetAllComponentsView<LightComponent<LightData::Type::Directional>, TransformComponent>();
			for (auto& e : es)
				window->RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Directional>>(e), registry.GetComponent<TransformComponent>(e));
		}


		float aspectRatio = static_cast<float>(window->GetSize().x) / static_cast<float>(window->GetSize().y);
		auto scaleX = [&](float x)-> float
			{
				return 2.0f * (((x/ 1920.0f) * aspectRatio) / (16.0f / 9.0f));
			};
		auto scaleY = [&](float x)-> float
			{
				return 2.0f * (x / 1080.0f);
			};
		auto posX = [&](float x)-> float
			{

				return (2.0f * aspectRatio / 1920.0f) * x - aspectRatio;
			};
		auto posY = [&](float y)-> float
			{
				
				return (2.0f / 1080.0f) * y - 1.0f;
			};

		auto scaleX_ = VirtualToNDC_X(1920, 1920.0f, aspectRatio,true);
		std::cout << scaleX_;
		auto posX_ = VirtualToNDC_X(0, 1920.0f, aspectRatio);
		std::cout << posX_;

		auto scaleY_ = VirtualToNDC_Y(1080, 1080.0f,true);
		std::cout << scaleY_;
		auto posY_ = VirtualToNDC_Y(0, 1080.0f);
		std::cout << posY_;

		glm::mat3 fullScreenMat = glm::mat3(
			VirtualToNDC_X(1920,1920.0f,aspectRatio,true), 0.0f, VirtualToNDC_X(0, 1920.0f, aspectRatio),
			0.0f, VirtualToNDC_Y(1080,1080,true), VirtualToNDC_Y(0,1080),
			0.0f,		0.0f,	1.0f		 );


		window->App()->RegisterUi(UiData{ {1,1,1,1},fullScreenMat }, &TextureLoader::Load("Textures/texture.jpg", window->App()),window->GetSize());
		window->Render({ 0.53f, 0.81f, 0.92f, 1.0f });


	}


	window->Destroy();
	KGR::RenderWindow::End();
}