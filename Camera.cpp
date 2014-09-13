#include "Camera.h"

////////////////////
//  Camera Class  //
////////////////////


//////////////////////////
//  Camera Constructor  //
//////////////////////////

Camera::Camera(float x, float y, float z, float a)
{
	m_x=x;
	m_y=y;
	m_z=z;
	m_rotate = a;
	m_pitch = 0;
	m_dx = sin(m_rotate * (XM_PI/180.0));
	m_dz = cos(m_rotate * (XM_PI/180.0));
	m_dy = sin(m_pitch * (XM_PI/180.0));	
	m_minimum_ray = 0.5f;;

}

Camera::~Camera(){}

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0);
	
	m_upposition = XMVectorSet(sin(m_rotate * (XM_PI/180.0))* cos((m_pitch+90) * (XM_PI/180.0)),
								   sin((m_pitch+90) * (XM_PI/180.0)),
								   cos(m_rotate * (XM_PI/180.0))* cos ((m_pitch+90) * (XM_PI/180.0)), 0);

	m_lookatposition = XMVectorSet(m_x + m_dx,
								   m_y +  m_dy,
								   m_z + m_dz, 0);
	
	return XMMatrixLookAtLH(m_position, m_lookatposition, m_upposition);

}

void Camera::Rotate(float a)
{
	m_rotate+=a;
	m_dx = sin(m_rotate * (XM_PI/180.0))* cos((m_pitch+90) * (XM_PI/180.0));
	m_dz = cos(m_rotate * (XM_PI/180.0))* cos ((m_pitch+90) * (XM_PI/180.0));

}

void Camera::Forwards(float a)
{
	m_x += a*m_dx;	
	m_y += a*m_dy;	
	m_z += a*m_dz;
}

void Camera::Sideways(float a)
{
	m_x += a*m_dz;	
	m_z -= a*m_dx;
}

void Camera::Vertical(float a)
{
	m_x += a*sin(m_rotate * (XM_PI/180.0))* cos((m_pitch+90) * (XM_PI/180.0));	
	m_y += a*sin((m_pitch+90) * (XM_PI/180.0));
	m_z += a*cos(m_rotate * (XM_PI/180.0))* cos ((m_pitch+90) * (XM_PI/180.0));
}

void Camera::Pitch(float a)
{
	m_pitch+=a;

	if (m_pitch <180.0) m_pitch +=360;
	if (m_pitch >180.0) m_pitch -=360;

	m_dy = sin(m_pitch * (XM_PI/180.0));			
	m_dx = sin(m_rotate * (XM_PI/180.0))* cos((m_pitch) * (XM_PI/180.0));
	m_dz = cos(m_rotate * (XM_PI/180.0))* cos ((m_pitch) * (XM_PI/180.0));
}

void Camera::SetAngle(float a)
{ 
	m_rotate=a;		
	m_dx = sin(m_rotate * (XM_PI/180.0))* cos((m_pitch) * (XM_PI/180.0));
	m_dz = cos(m_rotate * (XM_PI/180.0))* cos ((m_pitch) * (XM_PI/180.0));
}

void Camera::SetPitch(float a)
{
	m_pitch=a;
	if (m_pitch <180.0) m_pitch +=360;
	if (m_pitch >180.0) m_pitch -=360;
	m_dy = sin(m_pitch * (XM_PI/180.0));	
}

float Camera::GetX(){
	return m_x;
}

float Camera::GetY(){
	return m_y;
}

float Camera::GetZ(){
	return m_z;
}

float Camera::GetdX(){
	return m_dx;
}

float Camera::GetdY(){
	return m_dy;
}

float Camera::GetdZ(){
	return m_dz;
}

XMVECTOR Camera::get_Pos(){
	return m_position;
}

int Camera::ObjHit(scene_node* root_node, float speed)
{
	int i;
	
	XMMATRIX identity = XMMatrixIdentity();
	root_node->update_collision_tree(&identity,1.0);

	/*Collision detection code -> Forwards*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdX() * (speed + m_minimum_ray),	
	GetdY() * (speed + m_minimum_ray), GetdZ() * (speed + m_minimum_ray)))
	{
		i = 1;
		return i;
	}
	
	/*Collision detection code -> Behind*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdX() * (-speed),
	GetdY() * (-speed), GetdZ() * (-speed)))
	{
		i = 2;
		return i;
	}
	
	/*Collision detection code -> Right*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdZ() * (speed),	
	GetdY() * (speed), -GetdX() * (speed)))
	{
		i = 3;
		return i;
	}
	
	/*Collision detection code -> Left*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), -GetdZ() * (speed),	
	GetdY() * (speed), GetdX() * (speed)))
	{
		i = 4;
		return i;
	}


	/*Collision detection code -> Above*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), -GetdY() * (speed),	
	GetdX() * (speed), GetdZ() * (speed)))
	{
		i = 5;
		return i;
	}

	/*Collision detection code -> Below*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdY() * (speed),	
	GetdX() * (speed), GetdZ() * (speed)))
	{
		i = 6;
		return i;
	}
		i = 0;
		return i;
}

bool Camera::ScreenHit(scene_node* root_node, float speed)
{
	/*Collision detection code -> Forwards*/
	if(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdX() * (speed + m_minimum_ray),	
	GetdY() * (speed + m_minimum_ray), GetdZ() * (speed + m_minimum_ray)) || 
	/*Collision detection code -> Behind*/
	(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdX() * (-speed),
	GetdY() * (-speed), GetdZ() * (-speed)) ||
	/*Collision detection code -> Right*/
	(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdZ() * (speed),	
	GetdY() * (speed), -GetdX() * (speed)) ||
	/*Collision detection code -> Left*/
	(root_node->check_collision_ray(GetX(), GetY(), GetZ(), -GetdZ() * (speed),	
	GetdY() * (speed), GetdX() * (speed)) ||
	/*Collision detection code -> Above*/
	(root_node->check_collision_ray(GetX(), GetY(), GetZ(), -GetdY() * (speed),	
	GetdX() * (speed), GetdZ() * (speed)) ||
	/*Collision detection code -> Below*/
	(root_node->check_collision_ray(GetX(), GetY(), GetZ(), GetdY() * (speed),	
	GetdX() * (speed), GetdZ() * (speed))))))))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void Camera::SetX(float a){
	m_x = a;
}

void Camera::SetY(float a){
	m_y = a;
}

void Camera::SetZ(float a){
	m_z = a;
}