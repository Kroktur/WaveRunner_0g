#include "DescriptorPool.h"
#include "Device.h"
KGR::_Vulkan::DescriptorPool::DescriptorPool(const std::vector<vkPoolSize>& info, size_t maxCount, Device* device)
{
	vk::DescriptorPoolCreateInfo poolInfo{
.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
.maxSets = static_cast<uint32_t>(maxCount),
.poolSizeCount = static_cast<uint32_t>(info.size()),
.pPoolSizes = info.data() };
	m_pool = vk::raii::DescriptorPool(device->Get(), poolInfo);
}
