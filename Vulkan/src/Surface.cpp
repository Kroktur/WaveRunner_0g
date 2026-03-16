#include "Surface.h"
#include "Instance.h"
KGR::_Vulkan::Surface::Surface(Instance* instance, GLFWwindow* window)
{
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance->Get(), window, nullptr, &_surface) != 0)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	m_surface = vk::raii::SurfaceKHR(instance->Get(), _surface);
}

KGR::_Vulkan::Surface::vkSurface& KGR::_Vulkan::Surface::Get()
{
	return m_surface;
}

const KGR::_Vulkan::Surface::vkSurface& KGR::_Vulkan::Surface::Get() const
{
	return m_surface;
}
