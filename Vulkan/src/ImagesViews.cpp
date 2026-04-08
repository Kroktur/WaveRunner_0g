#include "ImagesViews.h"
#include "SwapChain.h"
#include "Device.h"
KGR::_Vulkan::ImagesViews::ImagesViews(SwapChain* swapChain, Device* device, const ViewType& viewType)
{
	m_views.reserve(swapChain->GetImages().size());
	vk::ImageViewCreateInfo imageViewCreateInfo{ 
		.viewType = KGRToVulkan(viewType), 
		.format = swapChain->GetFormat().format, 
		// TODO encapsulate flag aspect 
		.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} };
	for (auto& image : swapChain->GetImages())
	{
		imageViewCreateInfo.image = image;
		m_views.emplace_back(device->Get(), imageViewCreateInfo);
	}
}

KGR::_Vulkan::ImagesViews::vkImagesViews& KGR::_Vulkan::ImagesViews::Get()
{
	return m_views;
}

const KGR::_Vulkan::ImagesViews::vkImagesViews& KGR::_Vulkan::ImagesViews::Get() const
{
	return m_views;
}

vk::ImageViewType KGR::_Vulkan::ImagesViews::KGRToVulkan(const ViewType& viewType)
{
	switch (viewType)
	{
	case ViewType::vt1D:
		return vk::ImageViewType::e1D;
	case ViewType::vt2D:
		return vk::ImageViewType::e2D;
	case ViewType::vt3D:
		return vk::ImageViewType::e3D;
	case ViewType::vt1DArray:
		return vk::ImageViewType::e1DArray;
	case ViewType::vt2DArray:
		return vk::ImageViewType::e2DArray;
	case ViewType::vtCube:
		return vk::ImageViewType::eCube;
	case ViewType::vtCubeArray:
		return vk::ImageViewType::eCubeArray;
	default:
		throw std::out_of_range("case not handle");
	}
}
