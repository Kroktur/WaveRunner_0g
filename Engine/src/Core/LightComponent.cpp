#include "Core/LightComponent.h"
#include <stdexcept>

glm::vec3 LightComponent < LightData::Type::Point > ::GetLightColor() const
{
	return m_lightColor;
}

glm::vec3 LightComponent<LightData::Type::Point>::GetSpecularColor() const
{
	return m_specularColor;
}

float LightComponent<LightData::Type::Point>::GetIntensity() const
{
	return m_intensity;
}

float LightComponent<LightData::Type::Point>::GetShiny() const
{
	return m_shiny;
}

void LightComponent<LightData::Type::Point>::SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor)
{
	m_lightColor = lightColor;
	m_specularColor = specularColor;
}

void LightComponent<LightData::Type::Point>::SetParam(float intensity, float shiny)
{
	m_intensity = intensity;
	m_shiny = shiny;
}

LightData LightComponent<LightData::Type::Point>::ToData() const
{
	LightData result;
	result.lightColor = m_lightColor;
	result.specularColor = m_specularColor;
	result.intensity = m_intensity;
	result.shiny = m_shiny;
	result.type = static_cast<std::uint32_t>(LightData::Type::Point);
	return result;
}

LightComponent<LightData::Type::Point> LightComponent<LightData::Type::Point>::Create(const glm::vec3& lightColor,
	const glm::vec3& specularColor, float intensity, float shiny)
{
	LightComponent<LightData::Type::Point> result;
	result.SetColor(lightColor, specularColor);
	result.SetParam(intensity, shiny);
	return result;
}

glm::vec3 LightComponent<LightData::Type::Directional>::GetLightColor() const
{
	return m_lightColor;
}

glm::vec3 LightComponent<LightData::Type::Directional>::GetSpecularColor() const
{
	return m_specularColor;
}

float LightComponent<LightData::Type::Directional>::GetShiny() const
{
	return m_shiny;
}

void LightComponent<LightData::Type::Directional>::SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor)
{
	m_lightColor = lightColor;
	m_specularColor = specularColor;
}

void LightComponent<LightData::Type::Directional>::SetParam(float shiny)
{
	m_shiny = shiny;
}

LightData LightComponent<LightData::Type::Directional>::ToData() const
{
	LightData result;
	result.lightColor = m_lightColor;
	result.specularColor = m_specularColor;
	result.shiny = m_shiny;
	result.type = static_cast<std::uint32_t>(LightData::Type::Directional);
	return result;
}

LightComponent<LightData::Type::Directional> LightComponent<LightData::Type::Directional>::Create(
	const glm::vec3& lightColor, const glm::vec3& specularColor, float shiny)
{
	LightComponent<LightData::Type::Directional> result;
	result.SetColor(lightColor, specularColor);
	result.SetParam(shiny);
	return result;
}

glm::vec3 LightComponent<LightData::Type::Spot>::GetLightColor() const
{
	return m_lightColor;
}

glm::vec3 LightComponent<LightData::Type::Spot>::GetSpecularColor() const
{
	return m_specularColor;
}

float LightComponent<LightData::Type::Spot>::GetIntensity() const
{
	return m_intensity;
}

float LightComponent<LightData::Type::Spot>::GetShiny() const
{
	return m_shiny;
}

float LightComponent<LightData::Type::Spot>::GetOpenAngle() const
{
	return m_openAngle;
}

float LightComponent<LightData::Type::Spot>::GetAdditionalSmooth() const
{
	return m_additionalSmooth;
}

void LightComponent<LightData::Type::Spot>::SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor)
{
	m_lightColor = lightColor;
	m_specularColor = specularColor;
}

void LightComponent<LightData::Type::Spot>::SetParam(float intensity, float shiny)
{
	m_intensity = intensity;
	m_shiny = shiny;
}

void LightComponent<LightData::Type::Spot>::SetAngle(float angle, float additionalSmooth)
{
	m_openAngle = angle;
	m_additionalSmooth = additionalSmooth;
}

LightData LightComponent<LightData::Type::Spot>::ToData() const
{
	LightData result;
	result.lightColor = m_lightColor;
	result.specularColor = m_specularColor;
	result.intensity = m_intensity;
	result.shiny = m_shiny;
	result.openAngle = m_openAngle;
	result.additionalSmooth = m_additionalSmooth;
	result.type = static_cast<std::uint32_t>(LightData::Type::Spot);
	return result;
}

LightComponent<LightData::Type::Spot> LightComponent<LightData::Type::Spot>::Create(const glm::vec3& lightColor,
	const glm::vec3& specularColor, float intensity, float shiny, float openAngle, float addSmooth)
{
	LightComponent<LightData::Type::Spot> result;
	result.SetColor(lightColor, specularColor);
	result.SetParam(intensity, shiny);
	result.SetAngle(openAngle,addSmooth);
	return result;
}
