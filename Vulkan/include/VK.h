//#pragma once
//#include <vulkan/vulkan_raii.hpp>
//#include <vma/vk_mem_alloc.h>
//#include "_GLFW.h"
//#include "Core/ManagerImple.h"
//#include "Global.h"
//
//
//
//namespace KGR
//{
//	namespace _Vulkan
//	{
//	
//		enum class TransitionType
//		{
//			TransferDst,
//			Present,
//			ColorAttachment,
//		};
//
//		struct _AppInfo
//		{
//			const char* appName = "Basic_Api";
//			const char* engineName = "None";
//			std::uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
//			std::uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
//			std::uint32_t version = vk::ApiVersion14;
//
//			void Create();
//			vk::ApplicationInfo& GetInfo();
//
//		private:
//			vk::ApplicationInfo m_Info;
//		};
//
//		struct _Instance
//		{
//			_Instance() = default;
//			_Instance(_AppInfo&& info, std::vector<char const*> validationLayers);
//
//			void AddLayer(const char* layer);
//			Instance& GetInstance();
//			const Instance& GetInstance() const;
//			void Clear();
//
//		private:
//			std::vector<char const*> m_validationLayers;
//			_AppInfo m_info;
//			Instance m_instance = nullptr;
//		};
//
//		struct _PhysicalDevice
//		{
//			_PhysicalDevice() = default;
//			_PhysicalDevice(_Instance* instance);
//
//			PhysicalDevice& GetDevice();
//			const PhysicalDevice& GetDevice() const;
//			void Clear();
//			ui32t GraphicsQueueIndex() const;
//
//		private:
//			ui32t m_queueIndex;
//			PhysicalDevice m_device = nullptr;
//		};
//
//		struct _Surface
//		{
//			_Surface() = default;
//			_Surface(_Instance* instance, _GLFW::Window* window);
//			SurfaceKHR& GetSurface();
//			const SurfaceKHR& GetSurface() const;
//			void Clear();
//
//		private:
//			SurfaceKHR m_surface = nullptr;
//		};
//
//		struct _Device
//		{
//			_Device() = default;
//			_Device(_PhysicalDevice* device, ui32t count = 1);
//			Device& GetDevice();
//			const Device& GetDevice() const;
//			void Clear();
//			void WaitIdle();
//
//		private:
//			Device m_device = nullptr;
//		};
//
//		struct _Queue
//		{
//			_Queue() = default;
//			_Queue(_Device* device, _PhysicalDevice* pDevice, ui32t index = 0);
//			Queue& GetQueue();
//			const Queue& GetQueue() const;
//			void Clear();
//
//		private:
//			Queue m_queue = nullptr;
//		};
//
//		struct _Swapchain
//		{
//			_Swapchain() = default;
//			_Swapchain(_PhysicalDevice* pDevice,
//				_Device* device,
//				_Surface* surface,
//				_GLFW::Window* window,
//				ui32t imageCount = 3,
//				_Swapchain* old = nullptr);
//
//			SwapchainKHR& GetSwapchain();
//			const SwapchainKHR& GetSwapchain() const;
//			void Clear();
//			VkSurfaceFormatKHR GetFormat() const;
//			vk::Extent2D GetSwapchainExtent();
//			const vk::Extent2D GetSwapchainExtent() const;
//
//		private:
//			VkSurfaceFormatKHR m_format;
//			SwapchainKHR m_chain = nullptr;
//			vk::Extent2D m_extent;
//		};
//
//		struct _VkImages
//		{
//			_VkImages() = default;
//			_VkImages(_Swapchain* swapchain);
//
//			std::vector<vk::Image>& GetImages();
//			const std::vector<vk::Image>& GetImages() const;
//			void Clear();
//
//		private:
//			std::vector<vk::Image> m_images;
//		};
//
//		struct _CommandPool
//		{
//			_CommandPool() = default;
//			_CommandPool(_PhysicalDevice* pDevice, _Device* device);
//
//			CommandPool& GetPool();
//			const CommandPool& GetPool() const;
//			void Clear();
//
//		private:
//			CommandPool m_pool = nullptr;
//		};
//
//		struct _CommandBuffer
//		{
//			_CommandBuffer() = default;
//			_CommandBuffer(_Device* device, _CommandPool* pool);
//
//			CommandBuffer& GetBuffer();
//			const CommandBuffer& GetBuffer() const;
//			void Clear();
//
//		private:
//			CommandBuffer m_buffer = nullptr;
//		};
//
//		struct _Semaphore
//		{
//			_Semaphore() = default;
//			_Semaphore(_Device* device);
//
//			Semaphore& GetSemaphore();
//			const Semaphore& GetSemaphore() const;
//			void Clear();
//
//		private:
//			Semaphore m_semaphore = nullptr;
//		};
//
//		struct _Fence
//		{
//			_Fence() = default;
//			_Fence(_Device* device);
//
//			Fence& GetFence();
//			const Fence& GetFence() const;
//			void Clear();
//
//		private:
//			Fence m_fence = nullptr;
//		};
//
//		struct _FrameData
//		{
//			_Semaphore presentCompleteSemaphore;
//			_CommandBuffer commandBuffer;
//			_Fence perFrameFence;
//		};
//
//		struct _PipeLine
//		{
//			_PipeLine() = default;
//			_PipeLine(_Vulkan::_Device* device, _Vulkan::_Swapchain* swap);
//			vk::raii::Pipeline& GetPipeline();
//			const vk::raii::Pipeline& GetPipeline() const;
//			void Clear();
//
//		private:
//			Pipeline m_pipeline = nullptr;
//			PipelineLayout m_layout = nullptr;
//		};
//	}
//
//	class Core_Vulkan
//	{
//	public:
//		Core_Vulkan();
//
//		void Init(_GLFW::Window* window);
//
//		i32t Begin();
//		i32t End();
//
//		void WaitIdle();
//		void Cleanup();
//
//		void RecreateSwapchain();
//		void Transition(_Vulkan::TransitionType type, CommandBuffer& cb);
//
//		_Vulkan::_Instance& GetInstance();
//		_Vulkan::_Device& GetDevice();
//		_Vulkan::_PhysicalDevice& GetPhysicalDevice();
//		_Vulkan::_CommandBuffer& GetCommandBuffer(ui32t frameIndex);
//		_Vulkan::_Swapchain& GetSwapchain();
//		_Vulkan::_PipeLine& GetPipeline();
//
//		std::vector<vk::Image>& GetSCImages();
//		vk::Image& GetCurrentImage();
//		vk::ImageView GetCurrentImageView();
//
//		ui32t GetFrameCount() const;
//		ui32t GetCurrentImageIndex() const;
//		ui32t GetCurrentFrame() const;
//
//		vk::PhysicalDeviceType GetGPU();
//
//
//		//TODO refacto
//		void createVertexBuffer() {
//			vk::BufferCreateInfo bufferInfo{ .size = sizeof(vertices[0]) * vertices.size(), .usage = vk::BufferUsageFlagBits::eVertexBuffer, .sharingMode = vk::SharingMode::eExclusive };
//			vertexBuffer = vk::raii::Buffer(m_device.GetDevice(), bufferInfo);
//
//			vk::MemoryRequirements memRequirements = vertexBuffer.getMemoryRequirements();
//			vk::MemoryAllocateInfo memoryAllocateInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) };
//			vertexBufferMemory = vk::raii::DeviceMemory(m_device.GetDevice(), memoryAllocateInfo);
//
//			vertexBuffer.bindMemory(*vertexBufferMemory, 0);
//
//			void* data = vertexBufferMemory.mapMemory(0, bufferInfo.size);
//			memcpy(data, vertices.data(), bufferInfo.size);
//			vertexBufferMemory.unmapMemory();
//		}
//		//TODO refacto
//		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
//		{
//			vk::PhysicalDeviceMemoryProperties memProperties = m_physicalDevice.GetDevice().getMemoryProperties();
//
//			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
//			{
//				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
//				{
//					return i;
//				}
//			}
//
//			throw std::runtime_error("failed to find suitable memory type!");
//		}
//		vk::raii::Buffer& GetVertexBuffer()
//		{
//			return vertexBuffer;
//		}
//
//	protected:
//
//		void InitInstance();
//		void CreatePhysicalDevice();
//		void CreateSurface(_GLFW::Window* window);
//		void CreateDevice();
//		void CreateSwapchain(_GLFW::Window* window);
//		void CreateCommandResources();
//		void CreateObjects();
//		void CreateViewImages();
//		void CreatePipeline();
//
//		i32t AcquireNextImage(ui32t frameIndex);
//		void SubmitCommands(ui32t frameIndex);
//		i32t Present(ui32t frameIndex, ui32t imageIndex);
//
//	private:
//
//		// Generic method for Transitions
//		void TransitionImage(
//			CommandBuffer& cb,
//			vk::Image& image,
//			vk::ImageLayout oldLayout,
//			vk::ImageLayout newLayout,
//			vk::PipelineStageFlags2 srcStage,
//			vk::AccessFlags2 srcAccess,
//			vk::PipelineStageFlags2 dstStage,
//			vk::AccessFlags2 dstAccess,
//			vk::ImageAspectFlags aspectMask,
//			ui32t baseMipLevel,
//			ui32t levelCount,
//			ui32t baseArrayLayer,
//			ui32t layerCount);
//
//	private:
//		Context m_vkContext;
//
//		vk::raii::Buffer       vertexBuffer = nullptr;
//		vk::raii::DeviceMemory vertexBufferMemory = nullptr;
//
//		_Vulkan::_AppInfo m_appInfo;
//		_Vulkan::_Instance m_instance;
//		_Vulkan::_PhysicalDevice m_physicalDevice;
//		_Vulkan::_Surface m_surface;
//		_Vulkan::_Device m_device;
//		_Vulkan::_Queue m_graphicsQueue;
//		_Vulkan::_Swapchain m_swapchain;
//		_Vulkan::_CommandPool m_commandPool;
//		_Vulkan::_PipeLine m_pipeline;
//
//		std::vector<vk::Image> m_scImages;
//		std::vector<ImageView> m_viewImages;
//		std::vector<_Vulkan::_FrameData> m_frameData;
//		std::vector<_Vulkan::_Semaphore> m_submitSemaphores;
//		std::vector<vk::ImageLayout> m_imageLayouts;
//
//		ui32t m_currentFrame = 0;
//		ui32t m_currentImageIndex = 0;
//
//		_GLFW::Window* m_window;
//
//		const std::vector<char const*> m_validationLayers = 
//		{ "VK_LAYER_KHRONOS_validation" };
//	};
//}