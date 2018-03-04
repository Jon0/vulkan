#include "vertex.h"

GeometryBuffer::GeometryBuffer(Device &deviceObj)
    :
    device {deviceObj.getVulkanDevice()},
    vertexBufferSize {sizeof(Vertex) * 1024},
    indexBufferSize {sizeof(uint16_t) * 1024},
    vertexBuffer {
        deviceObj.getPhysicalDevice(),
        device,
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    },
    indexBuffer {
        deviceObj.getPhysicalDevice(),
        device,
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    } {

    verts = vertices;
    inds = indices;
}


void GeometryBuffer::copyData(Device &deviceObj) {
    Memory vertexStagingBuffer {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    vertexStagingBuffer.copyData(verts.data());
    vertexBuffer.copyFromBuffer(deviceObj.getQueue(), vertexStagingBuffer);


    Memory indexStagingBuffer {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    indexStagingBuffer.copyData(inds.data());
    indexBuffer.copyFromBuffer(deviceObj.getQueue(), indexStagingBuffer);
}


void GeometryBuffer::render(VkCommandBuffer &commandBuffer) {
    VkBuffer vertexBuffers[] = {vertexBuffer.getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
