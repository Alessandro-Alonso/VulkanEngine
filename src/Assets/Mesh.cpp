#include "Mesh.h"
#include <cstring>

namespace NETAEngine {

    VkVertexInputBindingDescription Vertex::getBindingDescription() {
	    VkVertexInputBindingDescription bindingDescription{};
	    bindingDescription.binding = 0;
	    bindingDescription.stride = sizeof(Vertex);
	    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	    return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

    void Mesh::upload(VmaAllocator allocator, VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool) {
        if (_vertices.empty()) return;

        // 1. Calcular tamaño en bytes
        size_t bufferSize = sizeof(Vertex) * _vertices.size();

        // 2. Crear STAGING BUFFER (CPU Visible)
        // Este es el buffer temporal donde escribiremos desde C++
        VkBufferCreateInfo stagingBufferInfo{};
        stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // Fuente de la copia

        VmaAllocationCreateInfo stagingAllocInfo{};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; // Queremos escribir desde CPU

        AllocatedBuffer stagingBuffer;
        vmaCreateBuffer(allocator, &stagingBufferInfo, &stagingAllocInfo,
            &stagingBuffer._buffer, &stagingBuffer._allocation, nullptr);

        // 3. Copiar datos (Map -> Memcpy -> Unmap)
        void* data;
        vmaMapMemory(allocator, stagingBuffer._allocation, &data);
        memcpy(data, _vertices.data(), bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer._allocation);

        // 4. Crear VERTEX BUFFER (GPU Local - Muy Rapido)
        // Aqui es donde viviran los datos permanentemente
        VkBufferCreateInfo vertexBufferInfo{};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = bufferSize;
        // IMPORTANTE: TRANSFER_DST (destino de copia) + VERTEX_BUFFER (uso real)
        vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vertexAllocInfo{};
        vertexAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY; // Solo la GPU toca esto

        vmaCreateBuffer(allocator, &vertexBufferInfo, &vertexAllocInfo,
            &_vertexBuffer._buffer, &_vertexBuffer._allocation, nullptr);

        // 5. Ejecutar comando de COPIA (Immediate Submit)
        // Creamos un CommandBuffer temporal solo para esta transferencia
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        // Grabar comando
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = bufferSize;
        vkCmdCopyBuffer(commandBuffer, stagingBuffer._buffer, _vertexBuffer._buffer, 1, &copyRegion);
        vkEndCommandBuffer(commandBuffer);

        // Enviar a la cola
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue); // Esperar a que termine la copia

        // 6. Limpieza
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        vmaDestroyBuffer(allocator, stagingBuffer._buffer, stagingBuffer._allocation);
    }

    void Mesh::cleanup(VmaAllocator allocator) {
        if (_vertexBuffer._buffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, _vertexBuffer._buffer, _vertexBuffer._allocation);
            _vertexBuffer._buffer = VK_NULL_HANDLE;
        }
    }
}