#include "DescriptorLayout.h"
#include "Device.h"
#include "Pipeline.h"
KGR::_Vulkan::DescriptorLayout::DescriptorLayout(const std::vector<vkBinding>& bindings, Device* device)
{
	vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = static_cast<uint32_t>(bindings.size()), .pBindings = bindings.data() };
	m_layout = vk::raii::DescriptorSetLayout(device->Get(), layoutInfo);
}

KGR::_Vulkan::DescriptorLayout::vkDescriptorLayout& KGR::_Vulkan::DescriptorLayout::Get()
{
	return m_layout;
}

const KGR::_Vulkan::DescriptorLayout::vkDescriptorLayout& KGR::_Vulkan::DescriptorLayout::Get() const
{
	return m_layout;
}

void KGR::_Vulkan::DescriptorLayouts::Add(DescriptorLayout&& layout)
{
	m_layouts.push_back(std::move(layout));
	m_realLayouts.push_back(&m_layouts.back().Get());
}

KGR::_Vulkan::DescriptorLayout& KGR::_Vulkan::DescriptorLayouts::Get(size_t index)
{
	return m_layouts[index];
}

const KGR::_Vulkan::DescriptorLayout& KGR::_Vulkan::DescriptorLayouts::Get(size_t index) const
{
	return m_layouts[index];
}

size_t KGR::_Vulkan::DescriptorLayouts::Size()
{
	return m_layouts.size();
}

std::vector<KGR::_Vulkan::DescriptorLayout::vkDescriptorLayout*> KGR::_Vulkan::DescriptorLayouts::GetLayouts()
{
	std::vector<DescriptorLayout::vkDescriptorLayout*> result;
	for (auto& it : m_layouts)
	{
		result.push_back(&it.Get());
	}
	return result;
}
