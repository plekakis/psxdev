#ifndef STATES_H_DEF
#define STATES_H_DEF

#define STATE_LOAD (0)
#define STATE_MENU (1)
#define STATE_GAME (2)

// State functionality
void State_Initialize();
void State_Update();
void State_Transition(int8 i_stateFrom, int8 i_stateTo);

typedef void(*StateFunc)();

extern StateFunc g_stateUpdateFunc;
extern StateFunc g_statePreRenderFunc;
extern StateFunc g_stateRenderFunc;

#endif // STATES_H_DEF
