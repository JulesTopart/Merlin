#pragma once

// Main header file - include into application for complete access

#include "Merlin/Core/Log.h"

#include "Merlin/Core/Application.h"

#include "Merlin/Memory/ShaderStorageBuffer.h"
#include "Merlin/Memory/IndexBuffer.h"
#include "Merlin/Memory/VertexArray.h"
#include "Merlin/Memory/FrameBuffer.h"
#include "Merlin/Memory/RenderBuffer.h"

#include "Merlin/Renderer/Shader.h"
#include "Merlin/Renderer/Mesh.h"
#include "Merlin/Renderer/Primitive.h"
#include "Merlin/Renderer/ScreenQuadRenderer.h"

#include "Merlin/Scene/Camera.h"
#include "Merlin/Scene/CameraController.h"
#include "Merlin/Events/Event.h"
#include "Merlin/Events/ApplicationEvent.h"
#include "Merlin/Scene/SceneManager.h"

#include "Merlin/Tensor/ComputeShader.h"
#include "Merlin/Tensor/ParticleSystem.h"

#include "Merlin/Util/OpenGLDebug.h"
#include "Merlin/Util/ModelLoader.h"