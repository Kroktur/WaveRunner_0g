#include "Instance.h"
#include "VulkanCore.h"


void KGR::_Vulkan::AppInfo::Create()
{
	m_info = vk::ApplicationInfo
	{
		.pApplicationName = appName,
		.applicationVersion = appVersion,
		.pEngineName = engineName,
		.engineVersion = engineVersion,
		.apiVersion = version
	};
	
}

KGR::_Vulkan::Instance::Instance(AppInfo&& info, std::vector<char const*> validationLayers)
{
	m_info = std::move(info);
	m_info.Create();

	// Get the required layers
	std::vector<char const*> requiredLayers;
	if (enableValidationLayers)
	{
		requiredLayers.assign(validationLayers.begin(), validationLayers.end());
	}

	// Check if the required layers are supported by the Vulkan implementation.
	auto layerProperties = vkContext.enumerateInstanceLayerProperties();
	for (auto const& requiredLayer : requiredLayers)
	{
		if (std::ranges::none_of(layerProperties,
			[requiredLayer](auto const& layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; }))
		{
			throw std::runtime_error("Required layer not supported: " + std::string(requiredLayer));
		}
	}

	// Get the required extensions.
	auto requiredExtensions = getRequiredExtensions();

	// Check if the required extensions are supported by the Vulkan implementation.
	auto extensionProperties = vkContext.enumerateInstanceExtensionProperties();
	for (auto const& requiredExtension : requiredExtensions)
	{
		if (std::ranges::none_of(extensionProperties,
			[requiredExtension](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; }))
		{
			throw std::runtime_error("Required extension not supported: " + std::string(requiredExtension));
		}
	}

	vk::InstanceCreateInfo createInfo{
		.pApplicationInfo = &m_info.m_info,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data() };

	m_instance = vkInstance(vkContext, createInfo);
}


KGR::_Vulkan::Instance::vkInstance& KGR::_Vulkan::Instance::Get()
{
	return m_instance;
}

const KGR::_Vulkan::Instance::vkInstance& KGR::_Vulkan::Instance::Get() const
{
	return m_instance;
}

KGR::_Vulkan::AppInfo KGR::_Vulkan::Instance::GetInfo() const
{
	return m_info;
}

std::vector<char const*> KGR::_Vulkan::Instance::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	auto     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers)
	{
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	return extensions;
}
