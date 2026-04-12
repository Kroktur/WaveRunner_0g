#pragma once
#include "Core/Font.h"
#include <vector>
#include "Buffer.h"
#include "Tools/Util.h"

namespace KGR::_Vulkan
{
	class VulkanCore;
}

struct BufferData
{
	KGR::_Vulkan::Buffer m_vertexBuffer;
	KGR::_Vulkan::Buffer m_indexBuffer;
};

struct Text
{
	enum class Align
	{
		Center,
		Left,
		Right
	};
	friend KGR::_Vulkan::VulkanCore;
	Text() = default;
	AtlasFont* font = nullptr;
	Texture* textTexture = nullptr;
	void SetText(const std::string& text);
	void SetAlign(const Align& align);
	std::string GetText() const;
	Align GetAlign() const;
	void Bind(const vk::raii::CommandBuffer* buffer, int frameId);
private:
	static float Offset(const Align& align, float totalAdvance, float currentAdvance);
	void Upload(KGR::_Vulkan::VulkanCore* core,int frameId);
	size_t GetIndexSize() const;

	std::vector<BufferData> m_buffers;

	std::vector<bool> dirty;

	DataDirty<std::string> m_message {true," "};
	size_t m_size = 0;
	Align m_align = Align::Center;
};

struct TextComp
{
	TextComp() = default;
	Text text;
};