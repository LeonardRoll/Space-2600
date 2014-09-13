#pragma once
 
/////////////////
//  #Includes  //
/////////////////
 
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <dinput.h>
 
#include "camera.h" // <--- So you can move the camera.
#include "scene_node.h"
#include "Maths.h"

///////////////////
//  Input Class  //
///////////////////
 
class Input
{
        private:
               
                IDirectInput8* m_direct_input;
                IDirectInputDevice8* m_keyboard_device;
                IDirectInputDevice8*    m_mouse_device;
                DIMOUSESTATE            m_mouse_state;
 
                unsigned char m_keyboard_keys_state[256];
				float m_forwards;
				float m_backwards;
				float m_right;
				float m_left;
				float m_acceleration ; 
				float m_max_speed;
				float m_current_speed;
        public:
 
                Camera* m_p_Camera;
                HINSTANCE       m_HInst;
                HWND            m_HWnd;
 
                Input(Camera* p_Camera,HINSTANCE HInst,HWND HWnd);
                HRESULT InitialiseInput(bool share);
                void ReadInputStates(void);
                bool IsKeyPressed(unsigned char DI_keycode);
                void Player_Movement(scene_node* root_node);
				float GetMomentum(void);
				float GetCurrentSpeed(void);
				float GetForwardSpeed(void);
				float GetBackwardsSpeed(void);
				float GetRightSpeed(void);
				float GetLeftSpeed(void);
				~Input();
};