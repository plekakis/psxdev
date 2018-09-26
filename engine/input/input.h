#ifndef INPUT_H_INC
#define INPUT_H_INC

#include "../engine.h"

// Initializes the input subsystem
int16 Input_Initialize();

// Shutdown input subsystem
int16 Input_Shutdown();

// Update pad status
int16 Input_Update();

// Returns a mask of controllers that are connected
uint32 Input_GetConnectionMask();

// Returns the controller's name string
char* Input_GetControllerId(uint32 i_index);

// Returns the pad button mask
uint32 Input_GetPad(uint32 i_index);

// Returns true if the specified key is clicked (held down in previous frames, released in current)
bool Input_IsClicked(uint32 i_index, uint32 i_mask);

// Same as the above, but checks a different prev and current mask
bool Input_IsClickedEx(uint32 i_index, uint32 i_prevMask, uint32 i_mask);

// Returns true if the specified key is pressed in the current frame
bool Input_IsPressed(uint32 i_index, uint32 i_mask);

#endif //INPUT_H_INC