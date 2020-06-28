#ifndef HL_SCENE_CAMERA_ATTACHMENT_H_DEF
#define HL_SCENE_CAMERA_ATTACHMENT_H_DEF

#include "scene_attachment_common.h"

typedef struct
{
	HL_CommonAttachment m_common;
	CVECTOR 			m_clearColor;
	uint32				m_renderingMask;
}HL_CameraAttachment;

// Create a new camera attachment instance.
HL_CameraAttachment* HL_NewCameraAttachment();

// Initialise an existing camera attachment instance.
void HL_InitCameraAttachment(HL_CameraAttachment* const io_attachment);

// Free a previously created camera attachment instance.
int16 HL_FreeCameraAttachment(HL_CameraAttachment* const i_attachment);

#endif // HL_SCENE_CAMERA_ATTACHMENT_H_DEF