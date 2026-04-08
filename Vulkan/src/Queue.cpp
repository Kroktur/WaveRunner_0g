#include "Queue.h"
#include "Device.h"
KGR::_Vulkan::Queue::Queue(Device* device,uint32_t id)
{
	m_queue = vk::raii::Queue(device->Get(), device->GetQueueIndex(), id);
}

KGR::_Vulkan::Queue::vkQueue& KGR::_Vulkan::Queue::Get()
{
	return m_queue;
}

const KGR::_Vulkan::Queue::vkQueue& KGR::_Vulkan::Queue::Get() const
{
	return m_queue;
}
