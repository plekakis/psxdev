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

#endif //INPUT_H_INC