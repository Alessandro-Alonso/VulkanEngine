#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "../Vulkan/Utils/VulkanTypes.h"

namespace NETAEngine {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		// Helpers para configurar la Pipeline (Input Assembly)
		static VkVertexInputBindingDescription getBindingDescription();


		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

	};

	struct Mesh {
		std::vector<Vertex> _vertices;
		AllocatedBuffer _vertexBuffer;

		void upload(VmaAllocator allocator, VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool);

		void cleanup(VmaAllocator allocator);
	};
}
