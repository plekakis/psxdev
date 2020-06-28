#include "scene_camera_attachment.h"

///////////////////////////////////////////////////
HL_CameraAttachment* HL_NewCameraAttachment()
{
	HL_CameraAttachment* attachment = (HL_CameraAttachment*)Core_Malloc(sizeof(HL_CameraAttachment));
	HL_InitCameraAttachment(attachment);
	return attachment;
}

///////////////////////////////////////////////////
void HL_InitCameraAttachment(HL_CameraAttachment* const io_attachment)
{
	VERIFY_ASSERT(io_attachment, "HL_InitCameraAttachment: Input attachment pointer is null");
	
	// Common block
	io_attachment->m_common.m_isEnabled = TRUE;
	
	// Default camera settings
	setColor(&io_attachment->m_clearColor, 0, 0, 128);
	io_attachment->m_renderingMask = ~0u;
}

///////////////////////////////////////////////////
int16 HL_FreeCameraAttachment(HL_CameraAttachment* const i_attachment)
{
	VERIFY_ASSERT(i_attachment, "HL_FreeCameraAttachment: Input attachment pointer is null");
	
	Core_Free(i_attachment);	
	return E_OK;
}