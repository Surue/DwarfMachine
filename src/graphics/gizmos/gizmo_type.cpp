/*
MIT License

Copyright (c) 2019 Nicolas Schneider

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <graphics/gizmos/gizmo_type.h>
#include <graphics/gizmos/gizmo.h>
#include "entity/entity_handle.h"

namespace dm
{
static const uint32_t MAX_INSTANCES = 512;

std::shared_ptr<GizmoType> GizmoType::Create(Mesh* mesh, const float& lineThickness, const Color& color)
{
	return std::make_shared<GizmoType>(mesh, lineThickness, color);
}

GizmoType::GizmoType(Mesh* mesh, const float& lineThickness, const Color& color) :
	m_Mesh(mesh),
	m_LineThickness(lineThickness),
	m_Color(color),
	m_MaxInstances(0),
	m_Instances(0),
	m_InstanceBuffer(sizeof(Instance) * MAX_INSTANCES)
{
}

void GizmoType::Update(std::vector<Gizmo>& gizmos)
{
	m_MaxInstances = MAX_INSTANCES;
	m_Instances = 0;

	if(gizmos.empty())
	{
		return;
	}

	Instance *instances;
	m_InstanceBuffer.MapMemory(reinterpret_cast<void**>(&instances));

	for(const auto &gizmo : gizmos)
	{
		if(m_Instances >= m_MaxInstances)
		{
			break;
		}

		if(!EntityHandle(gizmo.entity).GetComponent<Drawable>(ComponentType::DRAWABLE)->isDrawable)
		{
			continue;
		}

		auto instance = &instances[m_Instances];
		instance->m_ModelMatrix = TransformManager::GetWorldMatrix(*gizmo.transform);
		instance->m_Color = gizmo.color;
		m_Instances++;
	}

	m_InstanceBuffer.UnmapMemory();
}

bool GizmoType::CmdRender(const CommandBuffer& commandBuffer, const PipelineGraphics& pipeline,
	UniformHandle& uniformScene)
{
	if(m_Instances == 0)
	{
		return false;
	}

	m_DescriptorSet.Push("UniformScene", uniformScene);
	const auto updateSuccess = m_DescriptorSet.Update(pipeline);

	if(!updateSuccess)
	{
		return false;
	}

	vkCmdSetLineWidth(commandBuffer, m_LineThickness);

	m_DescriptorSet.BindDescriptor(commandBuffer, pipeline);

	VkBuffer vertexBuffers[] = { m_Mesh->GetVertexBuffer()->GetBuffer(), m_InstanceBuffer.GetBuffer() };
	VkDeviceSize offset[] = { 0, 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offset);
	vkCmdBindIndexBuffer(commandBuffer, m_Mesh->GetIndexBuffer()->GetBuffer(), 0, dm::Mesh::GetIndexType());
	vkCmdDrawIndexed(commandBuffer, m_Mesh->GetIndexCount(), m_Instances, 0, 0, 0);
	return true;
}
}
