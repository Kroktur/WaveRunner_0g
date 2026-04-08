#include "SwapChain.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"


KGR::_Vulkan::SwapChain::SwapChain(PhysicalDevice* physicalDevice, Device* device, Surface* surface, GLFWwindow* window,
	PresMode wanted, ui32t imageCount,  SwapChain* old)
{
	

	auto surfaceCapabilities = physicalDevice->Get().getSurfaceCapabilitiesKHR(*surface->Get());
	m_swapChainExtent = chooseSwapExtent(surfaceCapabilities,window);
	m_swapChainSurfaceFormat = chooseSwapSurfaceFormat(physicalDevice->Get().getSurfaceFormatsKHR(*surface->Get()));
	m_imageCount = chooseSwapMinImageCount(surfaceCapabilities, imageCount);
	vk::SwapchainCreateInfoKHR swapChainCreateInfo{ .surface = *surface->Get(),
		.minImageCount = m_imageCount,
		.imageFormat = m_swapChainSurfaceFormat.format,
		.imageColorSpace = m_swapChainSurfaceFormat.colorSpace,
		.imageExtent = m_swapChainExtent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = chooseSwapPresentMode(physicalDevice->Get().getSurfacePresentModesKHR(*surface->Get()),wanted),
		.clipped = true,
	.oldSwapchain = old == nullptr ? nullptr : *old->Get()
	};

	m_swapChain = vk::raii::SwapchainKHR(device->Get(), swapChainCreateInfo);
	m_swapChainImages = m_swapChain.getImages();




}

KGR::_Vulkan::SwapChain::vkSwapChain& KGR::_Vulkan::SwapChain::Get()
{
	return m_swapChain;
}

const KGR::_Vulkan::SwapChain::vkSwapChain& KGR::_Vulkan::SwapChain::Get() const
{
	return m_swapChain;
}

vk::Extent2D KGR::_Vulkan::SwapChain::GetExtend() const
{
	return m_swapChainExtent;
}

vk::SurfaceFormatKHR&   KGR::_Vulkan::SwapChain::GetFormat()
{
	return m_swapChainSurfaceFormat;
}

const KGR::_Vulkan::SwapChain::vkFormat& KGR::_Vulkan::SwapChain::GetFormat() const
{
	return m_swapChainSurfaceFormat;
}

std::vector<KGR::_Vulkan::SwapChain::vkImage>& KGR::_Vulkan::SwapChain::GetImages()
{
	return m_swapChainImages;
}

const std::vector<KGR::_Vulkan::SwapChain::vkImage>& KGR::_Vulkan::SwapChain::GetImages() const
{
	return m_swapChainImages;
}

uint32_t KGR::_Vulkan::SwapChain::GetImagesCount() const
{
	return m_imageCount;
}

KGR::_Vulkan::SwapChain::vkExtend KGR::_Vulkan::SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
	if (capabilities.currentExtent.width != 0xFFFFFFFF)
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return {
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height) };
}

uint32_t KGR::_Vulkan::SwapChain::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities, uint32_t imageTargetCount)
{
	auto minImageCount = std::max(imageTargetCount, surfaceCapabilities.minImageCount);

	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
	{
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

KGR::_Vulkan::SwapChain::vkFormat KGR::_Vulkan::SwapChain::chooseSwapSurfaceFormat(
	const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	assert(!availableFormats.empty());
	const auto formatIt = std::ranges::find_if(
		availableFormats,
		[](const auto& format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
	return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR KGR::_Vulkan::SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, PresMode wanted)
{
	assert(std::ranges::any_of(availablePresentModes, [wanted](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
	return std::ranges::any_of(availablePresentModes,
		[wanted](const vk::PresentModeKHR value) { return IsWantedPresentModel(value, wanted); }) ?
		KGRToVulkan(wanted):
		KGRToVulkan(PresMode::Fifo);
}

bool KGR::_Vulkan::SwapChain::IsWantedPresentModel(vk::PresentModeKHR get, PresMode wanted)
{
	switch (wanted)
	{
	case PresMode::Fifo:
		return get == vk::PresentModeKHR::eFifo;
	case PresMode::FifoLatestReady:
		return get == vk::PresentModeKHR::eFifoLatestReady;
	case PresMode::FifoLatestReadyEXT:
		return get == vk::PresentModeKHR::eFifoLatestReadyEXT;
	case PresMode::FifoRelaxed:
		return get == vk::PresentModeKHR::eFifoRelaxed;
	case PresMode::Immediate:
		return get == vk::PresentModeKHR::eImmediate;
	case PresMode::Mailbox:
		return get == vk::PresentModeKHR::eMailbox;
	case PresMode::SharedContinuousRefresh:
		return get == vk::PresentModeKHR::eSharedContinuousRefresh;
	case PresMode::SharedDemandRefresh:
		return get == vk::PresentModeKHR::eSharedDemandRefresh;
	default:
		throw std::out_of_range("case not handle");
	}
}

vk::PresentModeKHR KGR::_Vulkan::SwapChain::KGRToVulkan(PresMode wanted)
{
	switch (wanted)
	{
	case PresMode::Fifo:
		return vk::PresentModeKHR::eFifo;
	case PresMode::FifoLatestReady:
		return vk::PresentModeKHR::eFifoLatestReady;
	case PresMode::FifoLatestReadyEXT:
		return  vk::PresentModeKHR::eFifoLatestReadyEXT;
	case PresMode::FifoRelaxed:
		return vk::PresentModeKHR::eFifoRelaxed;
	case PresMode::Immediate:
		return vk::PresentModeKHR::eImmediate;
	case PresMode::Mailbox:
		return vk::PresentModeKHR::eMailbox;
	case PresMode::SharedContinuousRefresh:
		return vk::PresentModeKHR::eSharedContinuousRefresh;
	case PresMode::SharedDemandRefresh:
		return vk::PresentModeKHR::eSharedDemandRefresh;
	default:
		throw std::out_of_range("case not handle");
	}
}
