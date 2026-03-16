#include "DebugDraw3D.h"
#include <optional>

void DebugDraw3D::BeginFrame(const glm::mat4& view, const glm::mat4& proj, float screenW, float screenH)
{
	m_viewProjMatrix = proj * view;
	m_screenWidth = screenW;
	m_screenHeight = screenH;
	m_drawList = ImGui::GetForegroundDrawList();
}

std::optional<ImVec2> DebugDraw3D::WorldToScreen(const glm::vec3& worldPos) const
{
	glm::vec4 clipPos = m_viewProjMatrix * glm::vec4(worldPos, 1.0f);

	if (clipPos.w <= 0.001f)
		return std::nullopt;

	glm::vec3 screenCoordPos = glm::vec3(clipPos) / clipPos.w;

	return ImVec2{
		(screenCoordPos.x * 0.5f + 0.5f) * m_screenWidth,
		(1.0f - (screenCoordPos.y * 0.5f + 0.5f)) * m_screenHeight
	};
}

void DebugDraw3D::DrawLine(const glm::vec3& start, const glm::vec3& end, ImU32 color, float thickness)
{
	auto screenStart = WorldToScreen(start);
	auto screenEnd = WorldToScreen(end);

	if (!screenStart || !screenEnd)
		return;

	m_drawList->AddLine(*screenStart, *screenEnd, color, thickness);
}

void DebugDraw3D::DrawPoint(const glm::vec3& worldPos, float radius, ImU32 color)
{
	auto screenPos = WorldToScreen(worldPos);

	if (!screenPos)
		return;

	m_drawList->AddCircleFilled(*screenPos, radius, color);
}

void DebugDraw3D::DrawFrame(const glm::vec3& origin, const KGR::CurveFrame& frame, float axisLength,
	float thickness, uint8_t alpha)
{
	DrawLine(origin, origin + frame.forward * axisLength, IM_COL32(0, 0, 255, alpha), thickness);
	DrawLine(origin, origin + frame.up * axisLength, IM_COL32(0, 255, 0, alpha), thickness);
	DrawLine(origin, origin + frame.right * axisLength, IM_COL32(255, 0, 0, alpha), thickness);
}