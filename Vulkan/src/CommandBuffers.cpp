#include "CommandBuffers.h"
#include "Device.h"
#include "SwapChain.h"
KGR::_Vulkan::CommandBuffers::CommandBuffers(Device* device)
{
	vk::CommandPoolCreateInfo poolInfo{ 
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = device->GetQueueIndex() };

	m_pool = vk::raii::CommandPool(device->Get(), poolInfo);
}
KGR::_Vulkan::CommandBuffers::vkCommandPool& KGR::_Vulkan::CommandBuffers::GetPool()
{
	return m_pool;
}

const KGR::_Vulkan::CommandBuffers::vkCommandPool& KGR::_Vulkan::CommandBuffers::GetPool() const
{
	return m_pool;
}

KGR::_Vulkan::CommandBuffers::vkCommandBuffer& KGR::_Vulkan::CommandBuffers::Acquire(Device* device)
{
	for (auto& [_, cb] : m_commandBuffers)
	{
		if (cb.isCpuFree && (cb.isGpuFree.getStatus() == vk::Result::eSuccess))
		{
			cb.isCpuFree = false;
			return cb.commandBuffer;
		}
	}

	auto allocInfo = vk::CommandBufferAllocateInfo{
		.commandPool = *m_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	auto cb = std::move(vk::raii::CommandBuffers(device->Get(), allocInfo).front());
	auto cbPtr = *cb;

	m_commandBuffers[cbPtr] = KGRCommandBuffer{
		.commandBuffer = std::move(cb),
		.isGpuFree = vk::raii::Fence(device->Get(), {.flags = vk::FenceCreateFlagBits::eSignaled }),
		.isCpuFree = false
	};

	return m_commandBuffers[cbPtr].commandBuffer;
}

KGR::_Vulkan::CommandBuffers::vkFence& KGR::_Vulkan::CommandBuffers::GetFence(vkCommandBuffer& buffer)
{
	return m_commandBuffers[*buffer].isGpuFree;
}

void KGR::_Vulkan::CommandBuffers::ReleaseCommandBuffer(vkCommandBuffer& commandBuffer)
{
		m_commandBuffers[*commandBuffer].isCpuFree = true;
}


