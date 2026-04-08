#include "PhysicalDevice.h"
#include "Instance.h"
KGR::_Vulkan::PhysicalDevice::PhysicalDevice(Instance* instance,DeviceType wanted)
{
	auto devices = instance->Get().enumeratePhysicalDevices();
	if (devices.empty())
		throw std::runtime_error("Unable to fetch physical device");

	vkPhysicDevice* selectedDevice = nullptr;
	int bestScore = -1;

	for (auto& d : devices)
	{
		auto dProperties = d.getProperties();
		auto memProps = d.getMemoryProperties();
		if (dProperties.apiVersion < instance->GetInfo().version)
			continue;
		if (!IsMatchingDeviceType(dProperties.deviceType, wanted))
			continue;

		uint32_t score = 0;
		uint64_t vRam = 0;

		// try to find the local device memory heap 
		for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
		{
			// only local device memory vRam
			if (memProps.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
				vRam += memProps.memoryHeaps[i].size;
		}

		score += dProperties.limits.maxImageDimension2D;
		score += dProperties.limits.maxComputeWorkGroupInvocations;
		score += vRam / (1024ull * 1024ull * 1024ull);
		auto totoScore = static_cast<int>(score);
		if (static_cast<int>(score) > bestScore)
		{
			selectedDevice = &d;
			bestScore = score;
		}
	}

	if (!selectedDevice || bestScore < 0)
		throw std::runtime_error("No suitable phyisical device found");

	m_device = vkPhysicDevice(std::move(*selectedDevice));
}

KGR::_Vulkan::PhysicalDevice::vkPhysicDevice& KGR::_Vulkan::PhysicalDevice::Get()
{
	return m_device;
}

const KGR::_Vulkan::PhysicalDevice::vkPhysicDevice& KGR::_Vulkan::PhysicalDevice::Get() const
{
	return m_device;
}

KGR::_Vulkan::PhysicalDevice::vkFormat KGR::_Vulkan::PhysicalDevice::findSupportedFormat(
	const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	auto formatIt = std::ranges::find_if(candidates, [&](auto const format) {
		vk::FormatProperties props = m_device.getFormatProperties(format);
		return (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
			((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)));
	});
	if (formatIt == candidates.end())
	{
		throw std::runtime_error("failed to find supported format!");
	}
	return *formatIt;
}

bool KGR::_Vulkan::PhysicalDevice::IsMatchingDeviceType(vk::PhysicalDeviceType get, DeviceType wanted)
{
	switch (wanted)
	{
	case DeviceType::Best:
		return true;
	case  DeviceType::Cpu:
		return get == vk::PhysicalDeviceType::eCpu;
	case DeviceType::Other:
		return get == vk::PhysicalDeviceType::eOther;
	case DeviceType::Discrete:
		return get == vk::PhysicalDeviceType::eDiscreteGpu;
	case  DeviceType::Integrated:
		return get == vk::PhysicalDeviceType::eIntegratedGpu;
	case DeviceType::Virtual:
		return get == vk::PhysicalDeviceType::eVirtualGpu;
	default :
		throw std::out_of_range("case not handle");
	}
}
