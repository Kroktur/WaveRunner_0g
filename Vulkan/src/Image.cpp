#include "Image.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "Buffer.h"

KGR::_Vulkan::Image::Image(uint32_t width, uint32_t height, uint32_t mipLevel, vk::Format format, vk::ImageTiling tiling,
	vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, Device* device, PhysicalDevice* physicalDevice) : m_width(width),m_height(height), m_mipmapLevel(mipLevel)
{

	createImage(width, height, mipLevel, format, tiling, usage, properties, m_image, m_imageMemory, device, physicalDevice);
}

void KGR::_Vulkan::Image::CreateView(vk::Format format, vk::ImageAspectFlags aspectFlags, Device* device)
{
	m_imageView = createImageView(m_image, format, aspectFlags,m_mipmapLevel, device);
}

KGR::_Vulkan::Image::vkImageView& KGR::_Vulkan::Image::GetView()
{
	return m_imageView;
}

const KGR::_Vulkan::Image::vkImageView& KGR::_Vulkan::Image::GetView() const
{
	return m_imageView;
}

KGR::_Vulkan::Image::vkImage& KGR::_Vulkan::Image::Get()
{
	return m_image;
}

const KGR::_Vulkan::Image::vkImage& KGR::_Vulkan::Image::Get() const
{
	return m_image;
}

void KGR::_Vulkan::Image::createImage(uint32_t width, uint32_t height, uint32_t mimMapLevel, vk::Format format, vk::ImageTiling tiling,
                                      vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image,
                                      vk::raii::DeviceMemory& imageMemory, Device* device, PhysicalDevice* physicalDevice)
{
	vk::ImageCreateInfo imageInfo{
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = {width, height, 1},
		.mipLevels = mimMapLevel,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined };	image = vk::raii::Image(device->Get(), imageInfo);
	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size,
		.memoryTypeIndex = Buffer::findMemoryType(memRequirements.memoryTypeBits, properties,physicalDevice) };
	imageMemory = vk::raii::DeviceMemory(device->Get(), allocInfo);
	image.bindMemory(imageMemory, 0);
}

KGR::_Vulkan::Image::vkImageView KGR::_Vulkan::Image::createImageView(vk::raii::Image& image, vk::Format format,
	vk::ImageAspectFlags aspectFlags,uint32_t mipMapLevel, Device* device)
{
	vk::ImageViewCreateInfo viewInfo{
		.image = image,
		.viewType = vk::ImageViewType::e2D,
		.format = format,
		.subresourceRange = {aspectFlags, 0, mipMapLevel, 0, 1}
	};
	return vk::raii::ImageView(device->Get(), viewInfo);
}
