#include <iostream>
#include "game.h"
#include "core/cameracomponent.h"
#include "core/physiccomponent.h"


float VirtualToNDC_X(float x, float virtualwidth, float aspectratio,bool scale = false)
{
	return (x / virtualwidth) * (2.0f * aspectratio) - (scale == false ? aspectratio : 0.0f);
}

float VirtualToNDC_Y(float y, float virtualheight, bool scale = false)
{
	return (y / virtualheight) * 2.0f - (scale == false? 1.0f : 0.0f);
}

enum class DirectionState
{
	HAUT
	, BAS
	, GAUCHE
	, DROITE

};

struct playerComponent
{
	DirectionState StartDir = DirectionState::BAS;
	bool isMoove = false;
	float distMoove = 0;
	glm::vec3 actualPos;
	glm::vec3 targetPos;
};

struct plateformComponent
{
	DirectionState Direction;
};

struct obstaclePlateform
{
	glm::vec3 position;
	float velocity = 6.0f;
	DirectionState Direction;
};

int main(int argc, char** argv)
{

	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	KGR::RenderWindow::Init();
	std::unique_ptr<KGR::RenderWindow> window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,800 }, "test", projectRoot / "Ressources");
	window->GetInputManager()->SetMode(GLFW_CURSOR);

	ecsType registry = ecsType{};


	{
		CameraComponent cam = CameraComponent::Create(glm::radians(70.0f),window->GetSize().x,window->GetSize().y,0.01f,100.0f,CameraComponent::Type::Perspective);
		TransformComponent transform;
		transform.SetPosition({ 0,15,15 });
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

		PhysicComponent gravity;
		TransformComponent transform;
		CollisionComp collider;
		collider.collider = &ColliderManager::Load("playerCollider", mesh.mesh);
		//GenerateBox("model/cube.obj", mesh.mesh);
		transform.SetPosition({ 0,0,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(mesh), std::move(text), std::move(transform), std::move(gravity), std::move(collider), playerComponent{});

	}

	///// plateforme bas
	{
		MeshComponent bas;
		bas.mesh = &MeshLoader::Load("Models/cube.obj", window->App());

		TextureComponent text1;
		text1.SetSize(bas.mesh->GetSubMeshesCount());
		for (int i = 0; i < bas.mesh->GetSubMeshesCount(); ++i)
			text1.AddTexture(i, &TextureLoader::Load("Textures/viking_room.png", window->App()));

		TransformComponent transform1;
		CollisionComp collider;
		collider.collider = &ColliderManager::Load("plateformBas", bas.mesh);
		transform1.SetPosition({ 0,-10,0 });
		transform1.SetScale({ 10,1,10 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(bas), std::move(text1), std::move(transform1), std::move(collider), plateformComponent{ DirectionState::BAS });
	}
	//////////////////

	///// plateforme haut
	{
		MeshComponent haut;
		haut.mesh = &MeshLoader::Load("Models/cube.obj", window->App());

		TextureComponent text1;
		text1.SetSize(haut.mesh->GetSubMeshesCount());
		for (int i = 0; i < haut.mesh->GetSubMeshesCount(); ++i)
			text1.AddTexture(i, &TextureLoader::Load("Textures/viking_room.png", window->App()));

		TransformComponent transform1;
		CollisionComp collider;
		collider.collider = &ColliderManager::Load("plateformHaut", haut.mesh);
		transform1.SetPosition({ 0,10,0 });
		transform1.SetScale({ 10,1,10 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(haut), std::move(text1), std::move(transform1), std::move(collider), plateformComponent{ DirectionState::HAUT });
	}
	/////////////////////


	///// plateforme droite
	{
		MeshComponent droite;
		droite.mesh = &MeshLoader::Load("Models/cube.obj", window->App());

		TextureComponent text1;
		text1.SetSize(droite.mesh->GetSubMeshesCount());
		for (int i = 0; i < droite.mesh->GetSubMeshesCount(); ++i)
			text1.AddTexture(i, &TextureLoader::Load("Textures/viking_room.png", window->App()));

		TransformComponent transform1;
		CollisionComp collider;
		collider.collider = &ColliderManager::Load("plateformBas", droite.mesh);
		transform1.SetPosition({ -10,0,0 });
		transform1.SetScale({ 1,10,10 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(droite), std::move(text1), std::move(transform1), std::move(collider), plateformComponent{ DirectionState::DROITE });
	}
	//////////////////


	///// plateforme gauche
	{
		MeshComponent gauche;
		gauche.mesh = &MeshLoader::Load("Models/cube.obj", window->App());

		TextureComponent text1;
		text1.SetSize(gauche.mesh->GetSubMeshesCount());
		for (int i = 0; i < gauche.mesh->GetSubMeshesCount(); ++i)
			text1.AddTexture(i, &TextureLoader::Load("Textures/viking_room.png", window->App()));

		TransformComponent transform1;
		CollisionComp collider;
		collider.collider = &ColliderManager::Load("plateformBas", gauche.mesh);
		transform1.SetPosition({ 10,0,0 });
		transform1.SetScale({ 1,10,10 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(gauche), std::move(text1), std::move(transform1), std::move(collider), plateformComponent{ DirectionState::GAUCHE});
	}
	//////////////////

	{
		LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 1.0f);
		TransformComponent transform;
		transform.SetPosition({ 0,20,0 });
		transform.LookAtDir({ 0,-1,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(lc), std::move(transform));
	}

	{
		LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 1.0f);
		TransformComponent transform;
		transform.SetPosition({ 0,0,0 });
		transform.LookAtDir({ 0,1,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(lc), std::move(transform));
	}
	{
		LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 1.0f);
		TransformComponent transform;
		transform.SetPosition({ 0,0,0 });
		transform.LookAtDir({ -1,0,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(lc), std::move(transform));
	}
	{
		LightComponent<LightData::Type::Directional> lc = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 1.0f);
		TransformComponent transform;
		transform.SetPosition({ 0,0, -5 });
		transform.LookAtDir({ 1,0,0 });
		auto e = registry.CreateEntity();
		registry.AddComponents(e, std::move(lc), std::move(transform));
	}
	////////////////////////////////////////////////////////////////////////////////


	glm::vec3 camCurrentPos = {0,8,15};
	glm::vec3 camCurrentLook = {0,-10, 0};
	float smoothCamSpeed = 2.0f;

	float current = 0.0f;
	KGR::Tools::Chrono<float> chrono;
	while (!window->ShouldClose())
	{
		float actual = chrono.GetElapsedTime().AsSeconds();
		float dt = actual - current;
		current = actual;

		{
			auto es = registry.GetAllComponentsView<MeshComponent,TransformComponent>();

			//for (auto& e : es)
			//{
			//	auto input = window->GetInputManager();

			//	static float speed = 25.0f;
			//	if (input->IsKeyDown(KGR::Key::Q))
			//		registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Yaw>(glm::radians(speed * dt));
			//	if (input->IsKeyDown(KGR::Key::D))
			//		registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Yaw>(glm::radians(-speed * dt));

			//	if (input->IsKeyDown(KGR::Key::Z))
			//		registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Pitch>(glm::radians(-speed * dt));
			//	if (input->IsKeyDown(KGR::Key::S))
			//		registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Pitch>(glm::radians(speed * dt));



			//	if (input->IsKeyDown(KGR::Key::A))
			//		registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Roll>(glm::radians(-speed * dt));
			//	if (input->IsKeyDown(KGR::Key::E))
			//		registry.GetComponent<TransformComponent>(e).RotateQuat<RotData::Orientation::Roll>(glm::radians(speed * dt));
			//}



			///////////////////// physique /////////////////////
			for (auto& e : es)
			{
				auto input = window->GetInputManager();

				if (registry.HasComponent<PhysicComponent>(e))
				{
					auto& gravity = registry.GetComponent<PhysicComponent>(e);

					auto view = registry.GetAllComponentsView<playerComponent, TransformComponent, CollisionComp>();

					for (auto e : view)
					{
						auto& playerComp = registry.GetComponent<playerComponent>(e);
						auto& transformBox = registry.GetComponent<TransformComponent>(e);
						auto& boxCol = registry.GetComponent<CollisionComp>(e);
						auto camView = registry.GetAllComponentsView<CameraComponent, TransformComponent>();


						if (playerComp.isMoove == false)
						{
							switch (playerComp.StartDir)
							{
							case DirectionState::BAS:
								if (input->IsKeyDown(KGR::Key::I))
									playerComp.StartDir = DirectionState::HAUT;
								if (input->IsKeyDown(KGR::Key::J))
									playerComp.StartDir = DirectionState::GAUCHE;
								if (input->IsKeyDown(KGR::Key::L))
									playerComp.StartDir = DirectionState::DROITE;
								break;

							case DirectionState::HAUT:
								if (input->IsKeyDown(KGR::Key::J))
									playerComp.StartDir = DirectionState::GAUCHE;
								if (input->IsKeyDown(KGR::Key::K))
									playerComp.StartDir = DirectionState::BAS;
								if (input->IsKeyDown(KGR::Key::L))
									playerComp.StartDir = DirectionState::DROITE;
								break;

							case DirectionState::GAUCHE:
								if (input->IsKeyDown(KGR::Key::I))
									playerComp.StartDir = DirectionState::HAUT;
								if (input->IsKeyDown(KGR::Key::K))
									playerComp.StartDir = DirectionState::BAS;
								if (input->IsKeyDown(KGR::Key::L))
									playerComp.StartDir = DirectionState::DROITE;
								break;

							case DirectionState::DROITE:
								if (input->IsKeyDown(KGR::Key::I))
									playerComp.StartDir = DirectionState::HAUT;
								if (input->IsKeyDown(KGR::Key::J))
									playerComp.StartDir = DirectionState::GAUCHE;
								if (input->IsKeyDown(KGR::Key::K))
									playerComp.StartDir = DirectionState::BAS;
								break;
							}
							
						}

						glm::vec3 directionVector(0.0f);


						if (playerComp.StartDir == DirectionState::BAS)
						{
							directionVector = glm::vec3(0, -1, 0);
						}
						if (playerComp.StartDir == DirectionState::HAUT)
						{
							directionVector = glm::vec3(0, 1, 0);
						}
						if (playerComp.StartDir == DirectionState::GAUCHE)
						{
							directionVector = glm::vec3(-1, 0, 0);
						}
						if (playerComp.StartDir == DirectionState::DROITE)
						{
							directionVector = glm::vec3(1, 0, 0);
						}

						if (input->IsKeyDown(KGR::Key::Z))
						{
							gravity.jump();
						}


						glm::vec3 camTargetPos = { 0,0,0 };
						glm::vec3 camTargetLook = { 0,0,0 };

						for (const auto& camEntity : camView)
						{
							auto& camTransform = registry.GetComponent<TransformComponent>(camEntity);

							switch (playerComp.StartDir)
							{
							case DirectionState::BAS:
								camTargetPos = { 0,8,15 };
								camTargetLook = { 0, -10, 0 };
								break;

							case DirectionState::HAUT:
								camTargetPos = { 0,-8,15 };
								camTargetLook = { 0, 10, 0 };
								break;

							case DirectionState::GAUCHE:
								camTargetPos = { 0,8,15 };
								camTargetLook = { -15, 0, 0 };
								break;

							case DirectionState::DROITE:
								camTargetPos = { 0,8,15 };
								camTargetLook = { 15, 0, 0 };
								break;
							}

							camCurrentPos = glm::mix(camCurrentPos, camTargetPos, smoothCamSpeed * dt);
							camCurrentLook = glm::mix(camCurrentLook, camTargetLook, smoothCamSpeed * dt);

							camTransform.SetPosition(camCurrentPos);
							camTransform.LookAt(camCurrentLook);
						}


						

						gravity.uptadePhysique(dt);

						//transformBox.Translate(glm::vec3(0, -1, 0)* gravity.getVelocity()* dt); //tombe en bas 
						glm::vec3 move = directionVector * gravity.getVelocity() * dt;
						transformBox.Translate(directionVector * gravity.getVelocity()* dt); //monte en haut
						//transformBox.Translate(glm::vec3(1, 0, 0) * gravity.getVelocity() * dt); //va a droite
						//transformBox.Translate(glm::vec3(-1, 0, 0) * gravity.getVelocity() * dt); //va a gauche
						
						auto boxOBB = boxCol.collider->ComputeGlobalOBB(
							transformBox.GetScale(),
							transformBox.GetPosition(),
							transformBox.GetOrientation());

						gravity.setIsGround(false);

						auto plateforms = registry.GetAllComponentsView<plateformComponent, TransformComponent, CollisionComp>();
						for (auto pltfrm : plateforms)
						{
							auto& transformPlateform = registry.GetComponent<TransformComponent>(pltfrm);
							auto& pltfrmCol = registry.GetComponent<CollisionComp>(pltfrm);

							auto pltfrmOBB = pltfrmCol.collider->ComputeGlobalOBB(
								transformPlateform.GetScale(),
								transformPlateform.GetPosition(),
								transformPlateform.GetOrientation());

							std::cout << boxOBB.GetCenter().x << "x " << boxOBB.GetCenter().y << "y " << boxOBB.GetCenter().z << "z " << std::endl;
							//std::cout << test.y << std::endl;
							auto collision = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(boxOBB, pltfrmOBB);

							if (collision.IsColliding())
							{
								transformBox.Translate(-move);
								gravity.resetVelocity();
								gravity.setIsGround(true);
							}
						}
					}
				}
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

		//auto scaleX_ = VirtualToNDC_X(1920, 1920.0f, aspectRatio,true);
		//std::cout << scaleX_;
		//auto posX_ = VirtualToNDC_X(0, 1920.0f, aspectRatio);
		//std::cout << posX_;

		//auto scaleY_ = VirtualToNDC_Y(1080, 1080.0f,true);
		//std::cout << scaleY_;
		//auto posY_ = VirtualToNDC_Y(0, 1080.0f);
		//std::cout << posY_;

		glm::mat3 fullScreenMat = glm::mat3(
			VirtualToNDC_X(1920,1920.0f,aspectRatio,true), 0.0f, VirtualToNDC_X(0, 1920.0f, aspectRatio),
			0.0f, VirtualToNDC_Y(1080,1080,true), VirtualToNDC_Y(0,1080),
			0.0f,		0.0f,	1.0f		 );


		//window->App()->RegisterUi(UiData{ {1,1,1,1},fullScreenMat }, &TextureLoader::Load("Textures/texture.jpg", window->App()),window->GetSize());
		window->Render({ 0.53f, 0.81f, 0.92f, 1.0f });


	}


	window->Destroy();
	KGR::RenderWindow::End();
}
