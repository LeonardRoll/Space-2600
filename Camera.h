#pragma once

/////////////////
//  #Includes  //
/////////////////

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <xnamath.h>
#include <math.h>

#include "scene_node.h"

////////////////////
//  Camera Class  //
////////////////////

class Camera 
{
	private:	

		float  m_x, m_y, m_z;
		float m_rotate;
		float m_pitch;

		float m_dx;	
		float m_dy;
		float m_dz;
		float m_minimum_ray;
	
		XMVECTOR m_position;
		XMVECTOR m_lookatposition;	
		XMVECTOR m_upposition;

	public:	
		Camera(float x, float y, float z, float a);
		~Camera();
	
		void Forwards(float a);
		void Sideways(float a);	
		void Vertical(float a);
	
		void Rotate(float a);
		void Pitch(float a);
		void SetAngle(float a);
		void SetPitch(float a);
	
		XMMATRIX GetViewMatrix();
		void SetX(float a);
		void SetY(float a);
		void SetZ(float a);
		
		float GetX();
		float GetY();
		float GetZ();

		float GetdX();
		float GetdY();
		float GetdZ();
		XMVECTOR get_Pos();
		int Camera::ObjHit(scene_node* root_node, float speed);
		bool Camera::ScreenHit(scene_node* root_node, float speed);
};

