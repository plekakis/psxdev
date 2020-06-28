#include "scene.h"

#include <base/core/core.h>

///////////////////////////////////////////////////
// Rendering entry points.
///////////////////////////////////////////////////

typedef enum
{
	BSRF_None 		= 0x0,
	BSRF_ClearColor	= 0x1
}BeginSceneRenderFlags;

///////////////////////////////////////////////////
void BeginSceneRender(HL_Scene* const i_scene, uint32 i_cameraIndex, uint32 i_flags)
{
	HL_SceneNode* node = i_scene->m_cameraNodes[i_cameraIndex];
	if (node != NULL)
	{
		// Is this camera supposed to set the clear color?
		HL_CameraAttachment* camera = node->m_camera;
		if (i_flags & BSRF_ClearColor)
		{
			Gfx_SetClearColor(camera->m_clearColor.r, camera->m_clearColor.g, camera->m_clearColor.b);
		}
		
		Gfx_BeginSubmission(OT_LAYER_BG);
	}
}

///////////////////////////////////////////////////
void RenderSceneNode(HL_Scene* const i_scene, uint32 i_cameraIndex, uint32 i_nodeIndex)
{
	
}

///////////////////////////////////////////////////
void EndSceneRender(HL_Scene* const i_scene, uint32 i_cameraIndex, uint32 i_flags)
{
	HL_SceneNode* node = i_scene->m_cameraNodes[i_cameraIndex];
	if (node != NULL)
	{
		Gfx_EndSubmission();
	}
}

///////////////////////////////////////////////////
void HL_SetSceneCamera(HL_Scene* const i_scene, uint32 i_index, HL_SceneNode* const i_cameraNode)
{
	VERIFY_ASSERT(i_scene, "HL_SetSceneCamera: Input scene pointer is null");
	VERIFY_ASSERT(i_index < HL_SCENE_MAX_CAMERAS, "HL_SetSceneCamera: specified index is greater than the currently allocated camera count!");
	
	if (i_cameraNode != NULL)
	{
		VERIFY_ASSERT(i_cameraNode->m_camera != NULL, "HL_SetSceneCamera: Node specified to use as camera has no camera attachment!");
	}
	
	i_scene->m_cameraNodes[i_index] = i_cameraNode;
}

///////////////////////////////////////////////////
void HL_ClearSceneCameras(HL_Scene* const i_scene)
{
	int32 i;
	
	VERIFY_ASSERT(i_scene, "HL_SetSceneCamera: Input scene pointer is null");
		
	for (i=0; i<HL_SCENE_MAX_CAMERAS; ++i)
	{
		i_scene->m_cameraNodes[i] = NULL;
	}
}

///////////////////////////////////////////////////
void HL_InitScene(HL_Scene* const io_scene)
{
	VERIFY_ASSERT(io_scene, "HL_InitScene: Input scene pointer is null");
		
	memset(io_scene->m_cameraNodes, 0, sizeof(HL_SceneNode*) *  HL_SCENE_MAX_CAMERAS);
	
	io_scene->m_nodeCount = 0u;
}

///////////////////////////////////////////////////
HL_Scene* HL_NewScene()
{
	HL_Scene* scene = Core_Malloc(sizeof(HL_Scene));
	HL_InitScene(scene);
	return scene;
}

///////////////////////////////////////////////////
int16 HL_FreeScene(HL_Scene* const i_scene)
{
	VERIFY_ASSERT(i_scene, "HL_FreeScene: Input scene pointer is null");
	
	Core_Free(i_scene);	
	return E_OK;
}

///////////////////////////////////////////////////
void HL_InitSceneNode(HL_SceneNode* const io_node)
{
	VERIFY_ASSERT(io_node, "HL_InitScene: Input scene pointer is null");
	
	io_node->m_isEnabled = TRUE;
	io_node->m_camera = NULL;
	io_node->m_model = NULL;	
}

///////////////////////////////////////////////////
HL_SceneNode* HL_NewSceneNode()
{
	HL_SceneNode* node = Core_Malloc(sizeof(HL_SceneNode));
	HL_InitSceneNode(node);
	return node;
}

///////////////////////////////////////////////////
int16 HL_FreeSceneNode(HL_SceneNode* const i_node)
{
	VERIFY_ASSERT(i_node, "HL_FreeSceneNode: Input scene node pointer is null");
	
	Core_Free(i_node);	
	return E_OK;
}

///////////////////////////////////////////////////
bool HL_IsCameraEnabled(HL_SceneNode* const i_node)
{
	VERIFY_ASSERT(i_node, "HL_IsCameraEnabled: Input scene node pointer is null");
	
	return i_node->m_isEnabled && i_node->m_camera->m_common.m_isEnabled;
}

///////////////////////////////////////////////////
int16 HL_AdvanceScene(HL_Scene* const i_scene)
{
	return E_OK;
}

///////////////////////////////////////////////////
int16 HL_RenderScene(HL_Scene* const i_scene)
{
	uint32 cameraIndex;
	int32  clearCameraIndex=-1;
	uint32 flags[HL_SCENE_MAX_CAMERAS];
		
	VERIFY_ASSERT(i_scene, "HL_BeginSceneRendering: Input scene pointer is null");
	
	memset(flags, 0u, sizeof(flags));
	
	for (cameraIndex=0u; cameraIndex<HL_SCENE_MAX_CAMERAS; ++cameraIndex)
	{
		uint32 nodeIndex=0u;
		uint32 nodeCount = i_scene->m_nodeCount;
		HL_SceneNode* const node = i_scene->m_cameraNodes[cameraIndex];
		
		// Skip disabled and NULL camera nodes.
		if (!node || !HL_IsCameraEnabled(node))
		{
			continue;
		}
		
		//
		// Setup camera rendering flags
		//
		// Is this the first camera that's being processed? Tell the rendering to grab its clear color and use it.
		// The remaining cameras render on top of what's already in the framebuffer, so need to update the clear color.
		// This could be simplified a bit by putting the clear color inside the scene structure, however I have plans for this to be more
		// flexible in the future.
		if (clearCameraIndex == -1)
		{
			flags[cameraIndex] |= BSRF_ClearColor;
			clearCameraIndex = cameraIndex;
		}
		
		BeginSceneRender(i_scene, cameraIndex, flags[cameraIndex]);
		for (nodeIndex=0u; nodeIndex<nodeCount; ++nodeIndex)
		{
			RenderSceneNode(i_scene, cameraIndex, nodeIndex);
		}
		EndSceneRender(i_scene, cameraIndex, flags[cameraIndex]);
	}
	return E_OK;
}