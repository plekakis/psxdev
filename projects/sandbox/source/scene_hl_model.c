#include <hl/caches/caches.h>
#include <hl/scene/scene.h>
#include <hl/scene/scene_attachments.h>

HL_Scene* g_scene = NULL;
HL_SceneNode* g_cameraNode = NULL;

///////////////////////////////////////////////////
void start()
{
	g_scene = HL_NewScene();
	g_cameraNode = HL_NewSceneNode();
	g_cameraNode->m_camera = HL_NewCameraAttachment();
	
	HL_SetSceneCamera(g_scene, 0, g_cameraNode);
}

///////////////////////////////////////////////////
void update()
{
	HL_AdvanceScene(g_scene);
}

///////////////////////////////////////////////////
void render()
{
	HL_RenderScene(g_scene);
}