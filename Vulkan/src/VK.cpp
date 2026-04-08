//#include "VKFiles.h"
//#include "VK.h"
//
//// STRUCT APPLICATION INFO
//void KGR::_Vulkan::_AppInfo::Create()
//{
//	m_Info = vk::ApplicationInfo
//	{
//		.pApplicationName = appName,
//		.applicationVersion = appVersion,
//		.pEngineName = engineName,
//		.engineVersion = engineVersion,
//		.apiVersion = version
//	};
//}
//
//vk::ApplicationInfo& KGR::_Vulkan::_AppInfo::GetInfo()
//{
//	return m_Info;
//}
//
//// STRUCT FENCE
//KGR::_Vulkan::_Fence::_Fence(_Device* device)
//{
//	m_fence = Fence(device->GetDevice(), { .flags = vk::FenceCreateFlagBits::eSignaled });
//}
//
//Fence& KGR::_Vulkan::_Fence::GetFence()
//{
//	return m_fence;
//}
//
//const Fence& KGR::_Vulkan::_Fence::GetFence() const
//{
//	return m_fence;
//}
//
//void KGR::_Vulkan::_Fence::Clear()
//{
//	m_fence.clear();
//}
//
//
//
//
//
//
//
//KGR::_Vulkan::_PipeLine::_PipeLine(_Vulkan::_Device* device, _Vulkan::_Swapchain* swap)
//{
//	auto& file = fileManager::Load("Shaders/slang.spv");
//	file.seekg(0, std::ios::end);
//	auto fileSize = file.tellg();
//	std::vector<char> buffer(fileSize);
//	file.seekg(0, std::ios::beg);
//	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
//	file.close();
//	fileManager::Unload("Shaders/slang.spv");
//
//	vk::ShaderModuleCreateInfo createInfo{
//		.codeSize = buffer.size() * sizeof(char),
//		.pCode = reinterpret_cast<const uint32_t*>(buffer.data()) };
//	ShaderModule shaderModule{ device->GetDevice(), createInfo };
//
//	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain" };
//	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
//	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
//	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };
//	vk::PipelineViewportStateCreateInfo      viewportState{ .viewportCount = 1, .scissorCount = 1 };
//
//	// implement here
//	auto                                   bindingDescription = Vertex::getBindingDescription();
//	auto                                   attributeDescriptions = Vertex::getAttributeDescriptions();
//	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{ .vertexBindingDescriptionCount = 1,
//															.pVertexBindingDescriptions = &bindingDescription,
//															.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
//															.pVertexAttributeDescriptions = attributeDescriptions.data() };
//
//
//
//	vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False, .polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack, .frontFace = vk::FrontFace::eClockwise, .depthBiasEnable = vk::False, .depthBiasSlopeFactor = 1.0f, .lineWidth = 1.0f };
//
//	vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };
//
//	vk::PipelineColorBlendAttachmentState colorBlendAttachment{ .blendEnable = vk::False,
//		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
//
//	vk::PipelineColorBlendStateCreateInfo colorBlending{ .logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment };
//
//	std::vector dynamicStates = {
//		vk::DynamicState::eViewport,
//		vk::DynamicState::eScissor };
//	vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };
//
//	m_layout = PipelineLayout(device->GetDevice(), vk::PipelineLayoutCreateInfo{});
//
//	std::vector<vk::Format> m_formats;
//	m_formats.push_back(static_cast<vk::Format>(swap->GetFormat().format));
//	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
//		{
//			.stageCount = 2,
//			.pStages = shaderStages,
//			.pVertexInputState = &vertexInputInfo,
//			.pInputAssemblyState = &inputAssembly,
//			.pViewportState = &viewportState,
//			.pRasterizationState = &rasterizer,
//			.pMultisampleState = &multisampling,
//			.pColorBlendState = &colorBlending,
//			.pDynamicState = &dynamicState,
//			.layout = m_layout,
//			.renderPass = nullptr},
//		{
//			.colorAttachmentCount = 1,
//			.pColorAttachmentFormats = m_formats.data()} };
//
//	m_pipeline = Pipeline(device->GetDevice(), nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
//}
//
//vk::raii::Pipeline& KGR::_Vulkan::_PipeLine::GetPipeline()
//{
//	return m_pipeline;
//}
//
//const vk::raii::Pipeline& KGR::_Vulkan::_PipeLine::GetPipeline() const
//{
//	return m_pipeline;
//}
//
//void KGR::_Vulkan::_PipeLine::Clear()
//{
//	m_layout.clear();
//	return m_pipeline.clear();
//}
//
//// STRUCT INSTANCE
//KGR::_Vulkan::_Instance::_Instance(_AppInfo&& info, std::vector<char const*> validationLayers)
//	: m_info(std::move(info))
//	, m_validationLayers(validationLayers)
//{
//	std::vector<char const*> requiredLayers;
//	requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());
//
//	const auto layerProperties = vkContext.enumerateInstanceLayerProperties();
//	if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) { return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; }); }))
//		throw std::runtime_error("One or more required layers are not supported !");
//
//	uint32_t glfwExtensionCount = 0;
//	auto     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//	auto     extensionProperties = vkContext.enumerateInstanceExtensionProperties();
//
//	std::vector<const char*> activeExtensions;
//	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
//		activeExtensions.push_back(glfwExtensions[i]);
//
//	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
//		if (std::ranges::none_of(extensionProperties, [glfwExtension = glfwExtensions[i]](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
//			throw std::runtime_error("Required GLFW extensions not supported");
//
//	m_info.Create();
//
//	auto instanceCreateInfo = vk::InstanceCreateInfo
//	{
//		.pApplicationInfo = &m_info.GetInfo(),
//		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
//		.ppEnabledLayerNames = requiredLayers.data(),
//		.enabledExtensionCount = uint32_t(activeExtensions.size()),
//		.ppEnabledExtensionNames = activeExtensions.data()
//	};
//
//	m_instance = Instance(vkContext, instanceCreateInfo);
//}
//
//void KGR::_Vulkan::_Instance::AddLayer(const char* layer)
//{
//	m_validationLayers.push_back(layer);
//}
//
//Instance& KGR::_Vulkan::_Instance::GetInstance()
//{
//	return m_instance;
//}
//
//const Instance& KGR::_Vulkan::_Instance::GetInstance() const
//{
//	return m_instance;
//}
//
//void KGR::_Vulkan::_Instance::Clear()
//{
//	m_instance.clear();
//}
//
//// STRUCT PHYSICAL DEVICE
//KGR::_Vulkan::_PhysicalDevice::_PhysicalDevice(_Instance* instance)
//{
//	auto devices = instance->GetInstance().enumeratePhysicalDevices();
//	if (devices.empty())
//		throw std::runtime_error("Unable to fetch physical device");
//
//	PhysicalDevice* selectedDevice = nullptr;
//
//	for (auto& d : devices)
//	{
//		auto dProperties = d.getProperties();
//
//		if (dProperties.apiVersion < VK_API_VERSION_1_4)
//			continue;
//
//		if (dProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
//		{
//			selectedDevice = &d;
//			break;
//		}
//
//		if (!selectedDevice)
//			selectedDevice = &d;
//	}
//
//	if (!selectedDevice)
//		throw std::runtime_error("No suitable phyisical device found");
//
//	m_device = PhysicalDevice(std::move(*selectedDevice));
//
//	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_device.getQueueFamilyProperties();
//	auto graphicsQueueFamilyProperty = std::find_if(
//		queueFamilyProperties.begin(),
//		queueFamilyProperties.end(),
//		[](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; }
//	);
//	m_queueIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
//}
//
//PhysicalDevice& KGR::_Vulkan::_PhysicalDevice::GetDevice()
//{
//	return m_device;
//}
//
//const PhysicalDevice& KGR::_Vulkan::_PhysicalDevice::GetDevice() const
//{
//	return m_device;
//}
//
//void KGR::_Vulkan::_PhysicalDevice::Clear()
//{
//	m_device.clear();
//}
//
//ui32t KGR::_Vulkan::_PhysicalDevice::GraphicsQueueIndex() const
//{
//	return m_queueIndex;
//}
//
//// STRUCT SURFACE
//KGR::_Vulkan::_Surface::_Surface(_Instance* instance, _GLFW::Window* window)
//{
//	VkSurfaceKHR _surface;
//	if (glfwCreateWindowSurface(*instance->GetInstance(), &window->GetWindow(), nullptr, &_surface) != VK_SUCCESS)
//		throw std::runtime_error("Failed to create GLFW window surface");
//
//	m_surface = SurfaceKHR(instance->GetInstance(), _surface);
//}
//
//SurfaceKHR& KGR::_Vulkan::_Surface::GetSurface()
//{
//	return m_surface;
//}
//
//const SurfaceKHR& KGR::_Vulkan::_Surface::GetSurface() const
//{
//	return m_surface;
//}
//
//void KGR::_Vulkan::_Surface::Clear()
//{
//	m_surface.clear();
//}
//
//// STRUCT DEVICE
//KGR::_Vulkan::_Device::_Device(_PhysicalDevice* device, ui32t count)
//{
//	float queuePriority = 0.0f;
//	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device->GetDevice().getQueueFamilyProperties();
//	auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo
//	{
//		.queueFamilyIndex = device->GraphicsQueueIndex(),
//		.queueCount = count,
//		.pQueuePriorities = &queuePriority
//	};
//
//	vk::StructureChain<vk::PhysicalDeviceFeatures2,
//		vk::PhysicalDeviceVulkan11Features,
//		vk::PhysicalDeviceVulkan12Features,
//		vk::PhysicalDeviceVulkan13Features> featureChain = {
//		{.features = {.samplerAnisotropy = true } },
//		{.shaderDrawParameters = true },
//		{.bufferDeviceAddress = true },
//		{.synchronization2 = true, .dynamicRendering = true },
//	};
//
//	std::vector<const char*> deviceExtensions =
//	{
//		vk::KHRSwapchainExtensionName,
//		vk::KHRSpirv14ExtensionName,
//		vk::KHRSynchronization2ExtensionName
//	};
//
//	vk::DeviceCreateInfo deviceCreateInfo
//	{
//		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
//		.queueCreateInfoCount = 1,
//		.pQueueCreateInfos = &deviceQueueCreateInfo,
//		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
//		.ppEnabledExtensionNames = deviceExtensions.data()
//	};
//
//	m_device = Device(device->GetDevice(), deviceCreateInfo);
//}
//
//Device& KGR::_Vulkan::_Device::GetDevice()
//{
//	return m_device;
//}
//
//const Device& KGR::_Vulkan::_Device::GetDevice() const
//{
//	return m_device;
//}
//
//void KGR::_Vulkan::_Device::Clear()
//{
//	m_device.clear();
//}
//
//void KGR::_Vulkan::_Device::WaitIdle()
//{
//	m_device.waitIdle();
//}
//
//// STRUCT QUEUE
//KGR::_Vulkan::_Queue::_Queue(_Device* device, _PhysicalDevice* pDevice, ui32t index)
//{
//	m_queue = Queue(device->GetDevice(), pDevice->GraphicsQueueIndex(), index);
//}
//
//Queue& KGR::_Vulkan::_Queue::GetQueue()
//{
//	return m_queue;
//}
//
//const Queue& KGR::_Vulkan::_Queue::GetQueue() const
//{
//	return m_queue;
//}
//
//void KGR::_Vulkan::_Queue::Clear()
//{
//	m_queue.clear();
//}
//
//
//// STRUCT SWAPCHAIN
//KGR::_Vulkan::_Swapchain::_Swapchain(_PhysicalDevice* pDevice,
//	_Device* device,
//	_Surface* surface,
//	_GLFW::Window* window,
//	ui32t imageCount,
//	_Swapchain* old)
//{
//	auto surfaceCaps = pDevice->GetDevice().getSurfaceCapabilitiesKHR(surface->GetSurface());
//	auto availableFormats = pDevice->GetDevice().getSurfaceFormatsKHR(surface->GetSurface());
//	auto availablePresentModes = pDevice->GetDevice().getSurfacePresentModesKHR(surface->GetSurface());
//
//	m_format = ([](const decltype(availableFormats)& formats)
//		{
//			for (const auto& availableFormat : formats)
//				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
//					return availableFormat;
//
//			return formats[0];
//		})(availableFormats);
//
//	auto swapExtent = ([&window](const vk::SurfaceCapabilitiesKHR& capabilities) 
//		{
//		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
//			return capabilities.currentExtent;
//
//		int width, height;
//		glfwGetFramebufferSize(&window->GetWindow(), &width, &height);
//
//		return vk::Extent2D
//		{
//			std::clamp<uint32_t>(width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width),
//			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
//		};
//		})(surfaceCaps);
//
//	auto swapPresentMode = ([](const decltype(availablePresentModes)& modes)
//		{
//			for (const auto& availablePresentMode : modes)
//				if (availablePresentMode == vk::PresentModeKHR::eFifoRelaxed)
//					return availablePresentMode;
//
//			return vk::PresentModeKHR::eFifo;
//		})(availablePresentModes);
//
//	auto minImageCount = std::clamp(imageCount, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);
//
//	auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR
//	{
//		.flags = vk::SwapchainCreateFlagsKHR(0),
//		.surface = surface->GetSurface(),
//		.minImageCount = minImageCount,
//		.imageFormat = static_cast<vk::Format>(m_format.format),
//		.imageColorSpace = static_cast<vk::ColorSpaceKHR>(m_format.colorSpace),
//		.imageExtent = swapExtent,
//		.imageArrayLayers = 1,
//		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
//		.imageSharingMode = vk::SharingMode::eExclusive,
//		.preTransform = surfaceCaps.currentTransform,
//		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
//		.presentMode = swapPresentMode,
//		.clipped = true,
//		.oldSwapchain = old == nullptr ? nullptr : *old->GetSwapchain()
//	};
//
//	m_extent = swapExtent;
//	m_chain = SwapchainKHR(device->GetDevice(), swapchainCreateInfo);
//}
//
//void KGR::_Vulkan::_Swapchain::Clear()
//{
//	m_chain.clear();
//}
//
//VkSurfaceFormatKHR KGR::_Vulkan::_Swapchain::GetFormat() const
//{
//	return m_format;
//}
//
//vk::Extent2D KGR::_Vulkan::_Swapchain::GetSwapchainExtent()
//{
//	return m_extent;
//}
//
//const vk::Extent2D KGR::_Vulkan::_Swapchain::GetSwapchainExtent() const
//{
//	return m_extent;
//}
//
//SwapchainKHR& KGR::_Vulkan::_Swapchain::GetSwapchain()
//{
//	return m_chain;
//}
//
//const SwapchainKHR& KGR::_Vulkan::_Swapchain::GetSwapchain() const
//{
//	return m_chain;
//}
//
//// STRUCT VKIMAGES
//KGR::_Vulkan::_VkImages::_VkImages(_Swapchain* swapchain)
//{
//	m_images = swapchain->GetSwapchain().getImages();
//}
//
//std::vector<vk::Image>& KGR::_Vulkan::_VkImages::GetImages()
//{
//	return m_images;
//}
//
//const std::vector<vk::Image>& KGR::_Vulkan::_VkImages::GetImages() const
//{
//	return m_images;
//}
//
//void KGR::_Vulkan::_VkImages::Clear()
//{
//	m_images.clear();
//}
//
//// STRUCT COMMAND POOL
//KGR::_Vulkan::_CommandPool::_CommandPool(_PhysicalDevice* pDevice, _Device* device)
//{
//	auto poolInfo = vk::CommandPoolCreateInfo
//	{
//		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
//		.queueFamilyIndex = pDevice->GraphicsQueueIndex()
//	};
//
//	m_pool = CommandPool(device->GetDevice(), poolInfo);
//}
//
//CommandPool& KGR::_Vulkan::_CommandPool::GetPool()
//{
//	return m_pool;
//}
//
//const CommandPool& KGR::_Vulkan::_CommandPool::GetPool() const
//{
//	return m_pool;
//}
//
//void KGR::_Vulkan::_CommandPool::Clear()
//{
//	m_pool.clear();
//}
//
//// STRUCT COMMAND BUFFER
//KGR::_Vulkan::_CommandBuffer::_CommandBuffer(_Device* device, _CommandPool* pool)
//{
//	vk::CommandBufferAllocateInfo allocInfo
//	{
//		.commandPool = pool->GetPool(),
//		.level = vk::CommandBufferLevel::ePrimary,
//		.commandBufferCount = 1
//	};
//
//	m_buffer = std::move(CommandBuffers(device->GetDevice(), allocInfo).front());
//}
//
//CommandBuffer& KGR::_Vulkan::_CommandBuffer::GetBuffer()
//{
//	return m_buffer;
//}
//
//const CommandBuffer& KGR::_Vulkan::_CommandBuffer::GetBuffer() const
//{
//	return m_buffer;
//}
//
//void KGR::_Vulkan::_CommandBuffer::Clear()
//{
//	m_buffer.clear();
//}
//
//// STRUCT SEMAPHORE
//KGR::_Vulkan::_Semaphore::_Semaphore(_Device* device)
//{
//	m_semaphore = Semaphore(device->GetDevice(), vk::SemaphoreCreateInfo());
//}
//
//Semaphore& KGR::_Vulkan::_Semaphore::GetSemaphore()
//{
//	return m_semaphore;
//}
//
//const Semaphore& KGR::_Vulkan::_Semaphore::GetSemaphore() const
//{
//	return m_semaphore;
//}
//
//void KGR::_Vulkan::_Semaphore::Clear()
//{
//	m_semaphore.clear();
//}
//
//// CLASS VULKAN
//KGR::Core_Vulkan::Core_Vulkan()
//{}
//
//void KGR::Core_Vulkan::Init(_GLFW::Window* window)
//{
//	m_window = window;
//	InitInstance();
//	CreateSurface(window);
//	CreatePhysicalDevice();
//	CreateDevice();
//	CreateSwapchain(window);
//	CreateViewImages();
//	CreateCommandResources();
//	CreateObjects();
//	CreatePipeline();
//	// TODO refacto
//	createVertexBuffer();
//}
//
//void KGR::Core_Vulkan::InitInstance()
//{
//	m_appInfo.appName = "GCVulkan";
//	m_appInfo.appVersion = VK_MAKE_VERSION(1, 0, 0);
//	m_appInfo.engineName = "None";
//	m_appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
//	m_appInfo.version = vk::ApiVersion14;
//
//	m_instance = _Vulkan::_Instance(std::move(m_appInfo), m_validationLayers);
//}
//
//void KGR::Core_Vulkan::CreatePhysicalDevice()
//{
//	m_physicalDevice = _Vulkan::_PhysicalDevice(&m_instance);
//}
//
//void KGR::Core_Vulkan::CreateSurface(_GLFW::Window* window)
//{
//	m_surface = _Vulkan::_Surface(&m_instance, window);
//}
//
//void KGR::Core_Vulkan::CreateDevice()
//{
//	m_device = _Vulkan::_Device(&m_physicalDevice);
//	m_graphicsQueue = _Vulkan::_Queue(&m_device, &m_physicalDevice);
//}
//
//void KGR::Core_Vulkan::CreateSwapchain(_GLFW::Window* window)
//{
//	m_swapchain = _Vulkan::_Swapchain(&m_physicalDevice, &m_device, &m_surface, window);
//	m_scImages = m_swapchain.GetSwapchain().getImages();
//}
//
//void KGR::Core_Vulkan::RecreateSwapchain()
//{
//	int width, height;
//	glfwGetFramebufferSize(&m_window->GetWindow(), &width, &height);
//
//	while (width == 0 || height == 0)
//	{
//		glfwGetFramebufferSize(&m_window->GetWindow(), &width, &height);
//		glfwWaitEvents();
//	}
//
//	m_device.WaitIdle();
//
//	m_pipeline.Clear();
//	m_viewImages.clear();
//	m_frameData.clear();
//	m_submitSemaphores.clear();
//
//	m_swapchain = _Vulkan::_Swapchain(&m_physicalDevice, &m_device, &m_surface, m_window, 3, &m_swapchain);
//	m_scImages = m_swapchain.GetSwapchain().getImages();
//
//	CreateViewImages();
//	CreateObjects();
//	CreatePipeline();
//}
//
//void KGR::Core_Vulkan::CreateCommandResources()
//{
//	m_commandPool = _Vulkan::_CommandPool(&m_physicalDevice, &m_device);
//}
//
//void KGR::Core_Vulkan::CreateObjects()
//{
//	m_frameData.clear();
//	m_submitSemaphores.clear();
//
//	for (size_t i = 0; i < _Vulkan::MAX_FRAMES_IN_FLIGHT; ++i)
//	{
//		_Vulkan::_FrameData frame;
//		frame.presentCompleteSemaphore = _Vulkan::_Semaphore(&m_device);
//		frame.perFrameFence = _Vulkan::_Fence(&m_device);
//		frame.commandBuffer = _Vulkan::_CommandBuffer(&m_device, &m_commandPool);
//		m_frameData.push_back(std::move(frame));
//	}
//
//	for (size_t i = 0; i < m_scImages.size(); ++i)
//		m_submitSemaphores.push_back(_Vulkan::_Semaphore(&m_device));
//}
//
//void KGR::Core_Vulkan::CreateViewImages()
//{
//	assert(m_viewImages.empty());
//	m_imageLayouts.assign(m_scImages.size(), vk::ImageLayout::eUndefined);
//
//	vk::ImageViewCreateInfo viewInfo
//	{
//		.viewType = vk::ImageViewType::e2D,
//		.format = static_cast<vk::Format>(m_swapchain.GetFormat().format),
//		.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
//	};
//
//	for ( auto& image : m_scImages)
//	{
//		viewInfo.image = image;
//		m_viewImages.push_back(ImageView(m_device.GetDevice(), viewInfo));
//	}
//}
//
//void KGR::Core_Vulkan::Transition(_Vulkan::TransitionType type, CommandBuffer& cb)
//{
//	auto& currentLayout = m_imageLayouts[m_currentImageIndex];
//
//	struct TransitionParams
//	{
//		vk::ImageLayout newLayout;
//		vk::PipelineStageFlags2 srcStage, dstStage;
//		vk::AccessFlags2 srcAccess, dstAccess;
//	};
//
//	TransitionParams parameters;
//
//	switch (type)
//	{
//	case _Vulkan::TransitionType::TransferDst:
//		parameters = 
//		{	  vk::ImageLayout::eTransferDstOptimal,
//			  vk::PipelineStageFlagBits2::eTopOfPipe,
//			  vk::PipelineStageFlagBits2::eTransfer,
//			  vk::AccessFlagBits2::eNone,
//			  vk::AccessFlagBits2::eTransferWrite 
//		};
//		break;
//	case _Vulkan::TransitionType::ColorAttachment:
//		parameters = 
//		{     vk::ImageLayout::eColorAttachmentOptimal,
//			  vk::PipelineStageFlagBits2::eColorAttachmentOutput,
//			  vk::PipelineStageFlagBits2::eColorAttachmentOutput,
//			  vk::AccessFlagBits2::eNone,
//			  vk::AccessFlagBits2::eColorAttachmentWrite 
//		};
//		break;
//	case _Vulkan::TransitionType::Present:
//		parameters = 
//		{     vk::ImageLayout::ePresentSrcKHR,
//			  vk::PipelineStageFlagBits2::eColorAttachmentOutput,
//			  vk::PipelineStageFlagBits2::eBottomOfPipe,
//			  vk::AccessFlagBits2::eColorAttachmentWrite,
//			  vk::AccessFlagBits2::eNone 
//		};
//		break;
//	}
//
//	TransitionImage
//	(	
//		cb,
//		GetCurrentImage(),
//		currentLayout,
//		parameters.newLayout,
//		parameters.srcStage,
//		parameters.srcAccess,
//		parameters.dstStage,
//		parameters.dstAccess,
//		vk::ImageAspectFlagBits::eColor,
//		0, 1, 0, 1
//	);
//
//	currentLayout = parameters.newLayout;
//}
//
//i32t KGR::Core_Vulkan::AcquireNextImage(ui32t frameIndex)
//{
//	auto fenceResult = m_device.GetDevice().waitForFences
//	(
//		*m_frameData[frameIndex].perFrameFence.GetFence(),
//		true,
//		UINT64_MAX
//	);
//
//	m_device.GetDevice().resetFences(*m_frameData[frameIndex].perFrameFence.GetFence());
//
//	uint32_t imageIndex = 0;
//	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*m_frameData[frameIndex].presentCompleteSemaphore.GetSemaphore());
//	VkDevice vkDevice = static_cast<VkDevice>(*m_device.GetDevice());
//	VkSwapchainKHR vkSwapChain = static_cast<VkSwapchainKHR>(*m_swapchain.GetSwapchain());
//
//	VkResult result = vkAcquireNextImageKHR
//	(
//		vkDevice,
//		vkSwapChain,
//		UINT64_MAX,
//		vkSemaphore,
//		VK_NULL_HANDLE,
//		&imageIndex
//	);
//
//	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
//		return -1;
//
//	m_currentImageIndex = imageIndex;
//	return static_cast<i32t>(imageIndex);
//}
//
//void KGR::Core_Vulkan::SubmitCommands(ui32t frameIndex)
//{
//	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
//
//	const auto submitInfo = vk::SubmitInfo
//	{
//		.waitSemaphoreCount = 1,
//		.pWaitSemaphores = &*m_frameData[frameIndex].presentCompleteSemaphore.GetSemaphore(),
//		.pWaitDstStageMask = &waitDestinationStageMask,
//		.commandBufferCount = 1,
//		.pCommandBuffers = &*m_frameData[frameIndex].commandBuffer.GetBuffer(),
//		.signalSemaphoreCount = 1,
//		.pSignalSemaphores = &*m_submitSemaphores[m_currentImageIndex].GetSemaphore(),
//	};
//
//	m_graphicsQueue.GetQueue().submit(submitInfo, *m_frameData[frameIndex].perFrameFence.GetFence());
//}
//
//i32t KGR::Core_Vulkan::Present(ui32t frameIndex, ui32t imageIndex)
//{
//	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*m_submitSemaphores[imageIndex].GetSemaphore());
//
//	VkPresentInfoKHR presentInfo{};
//	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//	presentInfo.waitSemaphoreCount = 1;
//	presentInfo.pWaitSemaphores = &vkSemaphore;
//	presentInfo.swapchainCount = 1;
//
//	VkSwapchainKHR swapchainC = static_cast<VkSwapchainKHR>(*m_swapchain.GetSwapchain());
//	presentInfo.pSwapchains = &swapchainC;
//	presentInfo.pImageIndices = &imageIndex;
//	presentInfo.pResults = nullptr;
//
//	VkQueue queueC = static_cast<VkQueue>(*m_graphicsQueue.GetQueue());
//	VkResult result = vkQueuePresentKHR(queueC, &presentInfo);
//
//	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
//		return -1;
//
//	return 0;
//}
//
//void KGR::Core_Vulkan::WaitIdle()
//{
//	m_device.WaitIdle();
//}
//
//void KGR::Core_Vulkan::Cleanup()
//{
//
//	// TODO refacto 
//	vertexBufferMemory.clear();
//	vertexBuffer.clear();
//
//
//	m_submitSemaphores.clear();
//	m_viewImages.clear();
//	m_pipeline.Clear();
//	m_frameData.clear();
//	m_commandPool.Clear();
//	m_scImages.clear();
//	m_swapchain.Clear();
//	m_graphicsQueue.Clear();
//	m_device.Clear();
//	m_surface.Clear();
//	m_physicalDevice.Clear();
//	m_instance.Clear();
//	
//}
//
//KGR::_Vulkan::_Instance& KGR::Core_Vulkan::GetInstance()
//{
//	return m_instance;
//}
//
//KGR::_Vulkan::_Device& KGR::Core_Vulkan::GetDevice()
//{
//	return m_device;
//}
//
//KGR::_Vulkan::_PhysicalDevice& KGR::Core_Vulkan::GetPhysicalDevice()
//{
//	return m_physicalDevice;
//}
//
//KGR::_Vulkan::_CommandBuffer& KGR::Core_Vulkan::GetCommandBuffer(ui32t frameIndex)
//{
//	return m_frameData[frameIndex].commandBuffer;
//}
//
//std::vector<vk::Image>& KGR::Core_Vulkan::GetSCImages()
//{
//	return m_scImages;
//}
//
//KGR::_Vulkan::_Swapchain& KGR::Core_Vulkan::GetSwapchain()
//{
//	return m_swapchain;
//}
//
//void KGR::Core_Vulkan::CreatePipeline()
//{
//	m_pipeline = _Vulkan::_PipeLine(&m_device, &m_swapchain);
//}
//
//KGR::_Vulkan::_PipeLine& KGR::Core_Vulkan::GetPipeline()
//{
//	return m_pipeline;
//}
//
//ui32t KGR::Core_Vulkan::GetFrameCount() const
//{
//	return static_cast<ui32t>(m_frameData.size());
//}
//
//ui32t KGR::Core_Vulkan::GetCurrentImageIndex() const
//{
//	return m_currentImageIndex;
//}
//
//vk::Image& KGR::Core_Vulkan::GetCurrentImage()
//{
//	return m_scImages[m_currentImageIndex];
//}
//
//vk::ImageView KGR::Core_Vulkan::GetCurrentImageView()
//{
//	return *m_viewImages[m_currentImageIndex];
//}
//
//ui32t KGR::Core_Vulkan::GetCurrentFrame() const
//{
//	return m_currentFrame;
//}
//
//void KGR::Core_Vulkan::TransitionImage(
//	CommandBuffer& cb,
//	vk::Image& image,
//	vk::ImageLayout oldLayout,
//	vk::ImageLayout newLayout,
//	vk::PipelineStageFlags2 srcStage,
//	vk::AccessFlags2 srcAccess,
//	vk::PipelineStageFlags2 dstStage,
//	vk::AccessFlags2 dstAccess,
//	vk::ImageAspectFlags aspectMask,
//	ui32t baseMipLevel,
//	ui32t levelCount,
//	ui32t baseArrayLayer,
//	ui32t layerCount)
//{
//	vk::ImageMemoryBarrier2 barrier =
//	{
//		.srcStageMask = srcStage,        // Wait for this pipeline stage to finish
//		.srcAccessMask = srcAccess,      // Flush these memory operations
//		.dstStageMask = dstStage,        // Block this pipeline stage until ready
//		.dstAccessMask = dstAccess,      // Prepare for these memory operations
//		.oldLayout = oldLayout,          // Current image layout
//		.newLayout = newLayout,          // Target image layout
//		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,  // No queue transfer
//		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.image = image,
//		.subresourceRange =
//		{
//			.aspectMask = aspectMask,         // Which part: color, depth, or stencil
//			.baseMipLevel = baseMipLevel,     // Starting mip level
//			.levelCount = levelCount,         // Number of mip levels
//			.baseArrayLayer = baseArrayLayer, // Starting array layer
//			.layerCount = layerCount          // Number of array layers
//		}
//	};
//
//	vk::DependencyInfo dependencyInfo =
//	{
//		.dependencyFlags = {},
//		.imageMemoryBarrierCount = 1,
//		.pImageMemoryBarriers = &barrier
//	};
//
//	cb.pipelineBarrier2(dependencyInfo);
//}
//
//vk::PhysicalDeviceType KGR::Core_Vulkan::GetGPU()
//{
//	return vk::PhysicalDeviceType::eDiscreteGpu;
//}
//
//i32t KGR::Core_Vulkan::Begin()
//{
//	int width, height;
//
//	glfwGetFramebufferSize(&m_window->GetWindow(), &width, &height);
//
//	if (width == 0 || height == 0)
//		return -1;
//
//	i32t imageIndex = AcquireNextImage(m_currentFrame);
//
//	if (imageIndex == -1)
//		return -1;
//
//	m_frameData[m_currentFrame].commandBuffer.GetBuffer().begin(vk::CommandBufferBeginInfo{});
//	return 0;
//}
//
//i32t KGR::Core_Vulkan::End()
//{
//	m_frameData[m_currentFrame].commandBuffer.GetBuffer().end();
//	SubmitCommands(m_currentFrame);
//
//	i32t result = Present(m_currentFrame, m_currentImageIndex);
//
//	if (result == -1)
//		return -1;
//
//	++m_currentFrame %= _Vulkan::MAX_FRAMES_IN_FLIGHT;
//	return 0;
//}
//
