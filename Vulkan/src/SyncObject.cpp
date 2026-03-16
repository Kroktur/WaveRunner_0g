#include "SyncObject.h"
#include "SwapChain.h"
#include "Device.h"
KGR::_Vulkan::SyncObject::SyncObject(Device* device, uint32_t imageCount)
: m_imageCount(imageCount)
, m_frameIndex(0)
, m_imageIndex(0)
{
	assert(m_presentCompleteSemaphores.empty() && m_renderFinishedSemaphores.empty() && m_inFlightFences.empty());
	
			for (size_t i = 0; i < imageCount; i++)
			{
				m_renderFinishedSemaphores.emplace_back(device->Get(), vk::SemaphoreCreateInfo());
			}
	
			for (size_t i = 0; i < imageCount; i++)
			{
				m_presentCompleteSemaphores.emplace_back(device->Get(), vk::SemaphoreCreateInfo());
				m_inFlightFences.emplace_back(device->Get(), vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
			}
}

KGR::_Vulkan::SyncObject::MyVkSemaphore& KGR::_Vulkan::SyncObject::GetCurrentPresentSemaphore()
{
	return m_presentCompleteSemaphores[m_frameIndex];
}

const KGR::_Vulkan::SyncObject::MyVkSemaphore& KGR::_Vulkan::SyncObject::GetCurrentPresentSemaphore() const
{
	return m_presentCompleteSemaphores[m_frameIndex];
}

KGR::_Vulkan::SyncObject::MyVkSemaphore& KGR::_Vulkan::SyncObject::GetCurrentRenderSemaphore()
{
	return m_renderFinishedSemaphores[m_imageIndex];
}

const KGR::_Vulkan::SyncObject::MyVkSemaphore& KGR::_Vulkan::SyncObject::GetCurrentRenderSemaphore() const
{
	return m_renderFinishedSemaphores[m_imageIndex];
}

KGR::_Vulkan::SyncObject::MyvkFence& KGR::_Vulkan::SyncObject::GetCurrentFence()
{
	return m_inFlightFences[m_frameIndex];
}

const KGR::_Vulkan::SyncObject::MyvkFence& KGR::_Vulkan::SyncObject::GetCurrentFence() const
{
	return m_inFlightFences[m_frameIndex];
}

uint32_t KGR::_Vulkan::SyncObject::AcquireNextImage(SwapChain* swapChain, Device* device)
{
	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*m_presentCompleteSemaphores[m_frameIndex]);
	VkDevice vkDevice = static_cast<VkDevice>(*device->Get());
	VkSwapchainKHR vkSwapChain = static_cast<VkSwapchainKHR>(*swapChain->Get());

	VkResult result = vkAcquireNextImageKHR
	(
		vkDevice,
		vkSwapChain,
		UINT64_MAX,
		vkSemaphore,
		VK_NULL_HANDLE,
		&m_imageIndex
	);
	return result;
}

void KGR::_Vulkan::SyncObject::IncrementFrame()
{
	m_frameIndex = (m_frameIndex + 1) % m_imageCount;
}

const uint32_t& KGR::_Vulkan::SyncObject::GetCurrentImage() const
{
	return m_imageIndex;
}

const uint32_t& KGR::_Vulkan::SyncObject::GetCurrentFrame() const
{
	return m_frameIndex;
}
