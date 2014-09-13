#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "model.h"

#include "Maths.h"

using namespace std;

class scene_node
{
private:
	Model* m_p_model;
	
	vector<scene_node*> m_children;
	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;
	float m_world_centre_x;
	float m_world_centre_y;
	float m_world_centre_z;
	float m_world_scale;
	XMMATRIX m_local_world_matrix;
public:
	scene_node(void);
	~scene_node(void);

	XMVECTOR get_position();
	XMVECTOR get_world_centre_position();
	void update_collision_tree(XMMATRIX* world,float scale);
	bool check_collision(scene_node* compare_tree);
	bool check_collision(scene_node* compare_tree, scene_node* object_tree_root);
	bool check_collision_ray(float x,float y,float z,float rx,float ry,float rz);
	void SetModel(Model* m);

	void execute(XMMATRIX* world,XMMATRIX* view, XMMATRIX* projection);
    void addChildNode(scene_node *n); 
    bool detatchNode(scene_node *n); 

	void SetXPos(float in);
	float GetXPos(void);
	bool IncrementX(float in,scene_node* root_node);

	void SetYPos(float in);
	float GetYPos(void);
	bool IncrementY(float in,scene_node* root_node);

	void SetZPos(float in);
	float GetZPos(void);
	bool IncrementZ(float in,scene_node* root_node);

	void SetXRot(float in);
	float GetXRot(void);
	bool IncrementXRot(float in,scene_node* root_node);	
	
	void SetYRot(float in);
	float GetYRot(void);
	bool IncrementYRot(float in,scene_node* root_node);

	void SetZRot(float in);
	float GetZRot(void);
	bool IncrementZRot(float in,scene_node* root_node);

	void SetScale(float in);
	float GetScale(void);
	bool IncrementScale(float in,scene_node* root_node);

	bool LookAt_XZ(float x, float z, scene_node* root_node);
	bool moveforward(float amount, scene_node* root_node);	
	bool LookAt_XYZ(float x, float ny,float z, scene_node* root_node);
	bool moveforward_XYZ(float amount, scene_node* root_node);
	bool closeEnough(float x,float y,float z,float mindist);
	Model* GetModel(void);
};