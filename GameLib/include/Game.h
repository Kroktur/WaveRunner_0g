#pragma once
#include <string>
#include <vector>

#include "Core/Frenet.h"
#include "Core/Spline.h"
#include "Core/Window.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"
#include "Tools/Chrono.h"

// to move 
struct ControllerComponent {};

struct LivingComponent
{
	float health;
	bool isAlive;
};

struct TakeDamageComponent
{
	float damage;
};

struct WaveManager
{
	WaveManager(const std::vector<float >& pts) : wavePositions(pts) {}
	WaveManager() = default;
	int currentWave = 0;
	int enemiesAlive = 0;

	bool isWaveActive = false;
	bool platformPaused = false;

	std::vector<float> wavePositions = { 0.2f, 0.5f, 0.8f };
};
using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct Scene
{
	void Init(const std::string& fp);
	void UpdateGame(float dt);
	void Run(const KGR::Tools::Chrono<float>::Time& fixedTime);
	void Render();
private:
	std::unique_ptr<KGR::RenderWindow> window;
	ecsType registry = ecsType{};
	WaveManager waveManager;
	std::vector<glm::vec3> rmfForwardDirs;
	std::vector<KGR::CurveFrame>rmfFrames;
	HermitCurve curve;
	float curvesTest = 0.0f;
	float rmfStep = 0.0f;
};
