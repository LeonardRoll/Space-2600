///////////////////
//  Input Class  //
///////////////////
 
#include "Input.h"
#include "DXBase.h"
 
/////////////////////////
//  Input Constructor  //
/////////////////////////

Input::Input(Camera* p_Camera,HINSTANCE HInst,HWND HWnd)
{
	m_HInst = HInst;
	m_HWnd = HWnd;
	m_p_Camera = p_Camera;
	m_forwards = 0.0;
	m_backwards = 0.0;
	m_right = 0.0;
	m_left = 0.0;
	m_acceleration  = 0.01f;
	m_max_speed = 10.0f;
	m_current_speed = 0.1f;
}
 
/////////////////////////////////////
//  Sets up the input devices ...  //
/////////////////////////////////////
 
HRESULT Input::InitialiseInput(bool share)
{
        HRESULT hr;
 
		/*Keyboard*/
        ZeroMemory(m_keyboard_keys_state, sizeof(m_keyboard_keys_state));
 
        /*Initialise direct input*/
        hr = DirectInput8Create(m_HInst,DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)& m_direct_input, NULL);
        if(FAILED(hr)) return hr;
 
        /*Initialise keyboard device*/
        hr = m_direct_input->CreateDevice(GUID_SysKeyboard, & m_keyboard_device, NULL);
        if(FAILED(hr)) return hr;
 
        /*Use predefined format to set up keyboard device*/
        hr = m_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
        if(FAILED(hr)) return hr;
 
        /*Set keyboard device sharing settings, DISCL_EXCLUSIVE won't share*/
        if(share)
                hr = m_keyboard_device->SetCooperativeLevel(m_HWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        else
                hr = m_keyboard_device->SetCooperativeLevel(m_HWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
 
        if(FAILED(hr)) return hr;
 
        /*Get access to the keyboard*/
        hr = m_keyboard_device->Acquire();
        if(FAILED(hr)) return hr;
 
        /*Mouse*/
        ZeroMemory(&m_mouse_state, sizeof(m_mouse_state));
 
        /*Initialize the direct input interface for the mouse*/
        hr = m_direct_input->CreateDevice(GUID_SysMouse, & m_mouse_device, NULL);
        if(FAILED(hr)) return hr;
 
        /*Set the data format for the mouse using the pre-defined mouse data format*/
        hr = m_mouse_device->SetDataFormat(&c_dfDIMouse);
        if(FAILED(hr)) return hr;
 
        /*Set mouse device sharing settings, DISCL_EXCLUSIVE won't share*/
        if(share)
                hr = m_mouse_device->SetCooperativeLevel(m_HWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        else
                hr = m_mouse_device->SetCooperativeLevel(m_HWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
        if(FAILED(hr)) return hr;
 
        /*Acquire the mouse*/
        hr = m_mouse_device->Acquire();
        if(FAILED(hr)) return hr;
 
        return S_OK;
}
 
//////////////////////////
//  Read Inputs Method  //
//////////////////////////
 
void Input::ReadInputStates(void)
{
        HRESULT hr;
 
        // Read the keyboard state
        hr = m_keyboard_device->GetDeviceState(sizeof(m_keyboard_keys_state), (LPVOID)&m_keyboard_keys_state);
        if(FAILED(hr))
        {
                // Reaquire keyboard access if lost
                if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
                {
                        m_keyboard_device->Acquire();
                }
        }
 
        // Read the mouse state
        hr = m_mouse_device->GetDeviceState(sizeof(m_mouse_state), (LPVOID)&m_mouse_state);
        if(FAILED(hr))
        {
                // Reaquire keyboard access if lost
                if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
                {
                        m_mouse_device->Acquire();
                }
        }      
}
 
 
/////////////////////////////////////
//  Key Press Return (used below)  //
/////////////////////////////////////
 
bool Input::IsKeyPressed(unsigned char DI_keycode)
{
        return m_keyboard_keys_state[DI_keycode] & 0x80;
}
 
////////////////////////////
//  Key Responses Method  //
////////////////////////////
 
void Input::Player_Movement(scene_node* root_node)
{
	//XMMATRIX identity = XMMatrixIdentity();
	//root_node->update_collision_tree(&identity,1.0); // ??? 

	/*Constant moving control*/
	if(m_forwards > m_backwards)
	{
		m_p_Camera->Forwards(m_forwards);
	}
	else if(m_backwards > m_forwards)
	{
		m_p_Camera->Forwards(-m_backwards);
	};

	if(m_right > m_left)
	{
		m_p_Camera->Sideways(m_right);
	}
	else if(m_left > m_right)
	{
		m_p_Camera->Sideways(-m_left);
	};
	
	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 1)
	{
		m_forwards = 0.0;
		m_backwards = 1.0;
	}
	
	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 2)
	{
		m_backwards = 0.0;
		m_forwards = 1.0;
	}

	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 3)
	{
		m_backwards = 0.0;
		m_forwards = 0.0;
		m_right = 0.0;
		m_left = 1.0;
	}
	
	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 4)
	{
		m_backwards = 0.0;
		m_forwards = 0.0;
		m_left = 0.0;
		m_right = 1.0;
	}

	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 5)
	{
		m_backwards = 0.0;
		m_forwards = 0.0;
		m_left = 0.0;
		m_right = 0.0;
	}

	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 6)
	{
		m_backwards = 0.0;
		m_forwards = 0.0;
		m_left = 0.0;
		m_right = 0.0;
	}

	if(m_p_Camera->ObjHit(root_node, m_current_speed) == 0)
	{
		// do nothing
	}

	/*Esc key = Close down game.*/
	if(IsKeyPressed(DIK_ESCAPE)) DestroyWindow(m_HWnd);

	/*W or Up fires the spaceship's thrusters...*/
	if(IsKeyPressed(DIK_W) || (IsKeyPressed(DIK_UP)))
	{
		if(m_forwards >= m_max_speed)
		{
			m_forwards = m_max_speed;
		}
		else
		{
			m_forwards = m_forwards + m_acceleration ;
			m_backwards = m_backwards - m_acceleration ;
		}

		if(m_forwards > m_current_speed)
		{
			m_current_speed = m_forwards;
		}
	};
	
	/*S or Down fires the reverse thrusters...*/
	if(IsKeyPressed(DIK_S) || (IsKeyPressed(DIK_DOWN)))
	{
		if(m_backwards >= m_max_speed)
		{
			m_backwards = m_max_speed;
		}
		else
		{
			m_backwards = m_backwards + m_acceleration ;	
			m_forwards = m_forwards - m_acceleration ;
		}

		if(m_backwards > m_current_speed)
		{
			m_current_speed = m_backwards;
		}
	};
	
	/*A or Left fires the side on thrusters...*/
	if(IsKeyPressed(DIK_LEFT) || (IsKeyPressed(DIK_A)))
	{
		if(m_left >= m_max_speed)
		{
			m_left = m_max_speed;
		}
		else
		{
			m_left = m_left + m_acceleration ;	
			m_right = m_right - m_acceleration ;
		}

		if(m_forwards > m_current_speed)
		{
			m_current_speed = m_right;
		}
	};

	/*D or Right fires the other sides thrusters...*/
	if(IsKeyPressed(DIK_RIGHT) || (IsKeyPressed(DIK_D)))
	{
		if(m_right >= m_max_speed)
		{
			m_right = m_max_speed;
		}
		else
		{
			m_right = m_right + m_acceleration ;	
			m_left = m_left - m_acceleration ;
		}

		if(m_forwards > m_current_speed)
		{
			m_current_speed = m_left;
		}
	};

	if(IsKeyPressed(DIK_SPACE))
	{
		m_forwards = 0.0f;
		m_backwards = 0.0f;
		m_left = 0.0f;
		m_right = 0.0f;
	};

	m_p_Camera->Rotate(m_mouse_state.lX*.9);
	m_p_Camera->Pitch(-m_mouse_state.lY*.9);
           
}

////////////////////////
//  Displays Momentum // <------ used in the HUD
////////////////////////

float Input::GetMomentum(void)
{
	float a = 0.0;

	if(m_forwards > m_backwards)
	{
		a + m_forwards;
		return a;
	}
	else if (m_backwards > m_forwards)
	{
		a - m_backwards;
		return a;
	}
};


float Input::GetCurrentSpeed(void)
{
	return m_current_speed;
};

float Input::GetForwardSpeed(void)
{
	return m_forwards;
};

float Input::GetBackwardsSpeed(void)
{
	return m_backwards;
};

float Input::GetRightSpeed(void)
{
	return m_right;
};

float Input::GetLeftSpeed(void)
{
	return m_left;
};

//////////////////////
//  Input Clean Up  //
//////////////////////

Input::~Input()
{
	if(m_keyboard_device)
	{
		m_keyboard_device->Unacquire();
		m_keyboard_device->Release();
	}

	if(m_mouse_device)
	{
		m_mouse_device->Unacquire();
		m_mouse_device->Release();
	}

	if(m_direct_input) m_direct_input->Release();
}