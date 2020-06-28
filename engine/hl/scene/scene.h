#ifndef HL_SCENE_H_DEF
#define HL_SCENE_H_DEF

#include <engine.h>
#include "scene_attachments.h"

#define HL_SCENE_MAX_CAMERAS (4) // maximum potentially active cameras in the scene

typedef struct
{
	bool m_isEnabled;
	
	// Attachments
	HL_TransformAttachment* m_transform;
	HL_CameraAttachment* m_camera;
	HL_ModelAttachment* m_model;
	
}HL_SceneNode;

typedef struct
{
	HL_SceneNode* 	m_cameraNodes[HL_SCENE_MAX_CAMERAS];
	uint32 			m_nodeCount;
}HL_Scene;

// Create a new scene instance.
HL_Scene* HL_NewScene();

// Initialise an existing scene instance.
void HL_InitScene(HL_Scene* const io_scene);

// Create a new scene node instance.
HL_SceneNode* HL_NewSceneNode();

// Initialise an existing scene node instance.
void HL_InitSceneNode(HL_SceneNode* const io_node);

// Free a previously created scene node instance.
int16 HL_FreeSceneNode(HL_SceneNode* const i_node);

// Free a previously created scene instance.
int16 HL_FreeScene(HL_Scene* const i_scene);

// Set the camera to use for the specified scene rendering pass.
void HL_SetSceneCamera(HL_Scene* const i_scene, uint32 i_index, HL_SceneNode* i_node);

// Clear the scene cameras.
void HL_ClearSceneCameras(HL_Scene* const i_scene);

// Check if a camera node has an enabled camera attachment.
bool HL_IsCameraEnabled(HL_SceneNode* const i_node);

// Get the number of nodes in the specified scene.
uint32 HL_GetSceneNodeCount(HL_Scene* const i_scene);

// Perform any processing that the scene requires.
int16 HL_AdvanceScene(HL_Scene* const i_scene);

// Start rendering the scene.
int16 HL_RenderScene(HL_Scene* const i_scene);

#endif // HL_SCENE_H_DEF