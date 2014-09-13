#include "scene_node.h"


scene_node::scene_node(void ){
	m_p_model = NULL;
	m_x=0.0f;
	m_y=0.0f;
	m_z=0.0f;
	m_xangle=0.0f;
	m_yangle=0.0f;
	m_zangle=0.0f;
	m_yangle=0.0f;
	m_scale=1.0f;
}

void scene_node::SetXPos(float in){ 
	m_x=in; 
}

float scene_node::GetXPos(void){ 
	return m_x;
}

bool scene_node::IncrementX(float in,scene_node* root_node){ 
	float old_x = m_x;
	m_x+=in; 

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_x = old_x;
		return true;
	}
	return false;
}

void scene_node::SetYPos(float in){ 
	m_y=in;
}

float scene_node::GetYPos(void){ 
	return m_y; 
}

bool scene_node::IncrementY(float in,scene_node* root_node){ 
	float old_y = m_y;
	m_y+=in; 

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_y = old_y;
		return true;
	}
	return false;
}

void scene_node::SetZPos(float in){
	m_z=in; 
}

float scene_node::GetZPos(void){ 
	return m_z;
}

bool scene_node::IncrementZ(float in,scene_node* root_node){ 
	float old_z = m_z;
	m_z+=in; 

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_z = old_z;
		return true;
	}
	return false;
}

void scene_node::SetXRot(float in){ 
	m_xangle=in;
}
float scene_node::GetXRot(void){ 
	return m_xangle; 
}

bool scene_node::IncrementXRot(float in,scene_node* root_node){ 
	float old_xangle = m_xangle;
	m_xangle+=in; 

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_xangle = old_xangle;
		return true;
	}
	return false;
}
	
void scene_node::SetYRot(float in){ 
	m_yangle=in; 
}

float scene_node::GetYRot(void){ 
	return m_yangle;
}

bool scene_node::IncrementYRot(float in,scene_node* root_node){ 
	float old_yangle = m_yangle;
	m_yangle+=in; 

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_yangle = old_yangle;
		return true;
	}
	return false;
}

void scene_node::SetZRot(float in){ 
	m_zangle=in; 
}

float scene_node::GetZRot(void){ 
	return m_zangle; 
}

void scene_node::SetScale(float in){ 
	m_scale=in; 
}

float scene_node::GetScale(void){ 
	return m_scale; 
}

bool scene_node::IncrementScale(float in,scene_node* root_node){ 
	float old_scale = m_scale;
	m_scale+=in; 
	XMMATRIX identity = XMMatrixIdentity();
	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_scale = old_scale;
		return true;
	}
	return false;
}

scene_node::~scene_node(void){
	while(m_children.size() > 0){
		delete m_children.back();		
		m_children.pop_back();	
	}
}

void scene_node::SetModel(Model* m){
	m_p_model = m;
}

void scene_node::execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection){
	// the local_world matrix will be used to calculate the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity() ;
	
	local_world = XMMatrixRotationX(XMConvertToRadians(m_xangle));	
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	local_world *=XMMatrixScaling(m_scale, m_scale,m_scale);
	local_world *= XMMatrixTranslation(m_x,m_y,m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	if(m_p_model) 
		m_p_model->Draw(&local_world, view, projection); // only draw if there is a model attached

	// traverse all child nodes, passing in the concatenated world matrix
	for(int i = 0; i< m_children.size();i++){
		m_children[i]->execute(&local_world, view, projection);
	}
}

void scene_node::addChildNode(scene_node *n){
	m_children.push_back(n);
}

bool scene_node::detatchNode(scene_node *n){
	 for(int i = 0; i< m_children.size();i++){
		 if (n == m_children[i]){
			 m_children.erase(m_children.begin()+i);
			 return true;
		 }
		 if( m_children[i]->detatchNode(n) == true) 
			 return true;
	 }	
	 return false;
}


bool scene_node::LookAt_XZ(float nx, float nz,scene_node* root_node){
	float old_yangle = m_yangle;
	float dx, dz;
	dx = nx-m_x;
	dz = nz-m_z;
	m_yangle = atan2(dx,dz)*180.0/XM_PI;

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_yangle = old_yangle;
		return true;
	}
	return false;
}

bool scene_node::moveforward(float amount,scene_node* root_node){
		float old_m_x = m_x;
		float old_m_z = m_z;

		m_x += sin(m_yangle * (XM_PI/180.0)) * amount;
		m_z += cos(m_yangle * (XM_PI/180.0)) * amount;

		XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_x = old_m_x;
		m_z = old_m_z;
		return true;
	}
	return false;
}



bool scene_node::LookAt_XYZ(float nx, float ny, float nz,scene_node* root_node){
	float old_yangle = m_yangle;
	float old_xangle = m_xangle;
	float dx, dy, dz;
	dx = nx-m_x;dz = nz-m_z;dy=ny-m_y;
	m_yangle = atan2(dx,dz)*180.0/XM_PI;
	m_xangle = -atan2(dy, sqrt(dx*dx+dz*dz))*180.0/XM_PI;

	XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_yangle = old_yangle;
		m_xangle = old_xangle;
		return true;
	}
	return false;
}

bool scene_node::moveforward_XYZ(float amount,scene_node* root_node){
		float old_m_x = m_x;
		float old_m_y = m_y;
		float old_m_z = m_z;

		m_x += sin(m_yangle * (XM_PI/180.0)) * amount *cos(m_xangle * (XM_PI/180.0));
		m_y += -sin(m_xangle * (XM_PI/180.0)) * amount;
		m_z += cos(m_yangle * (XM_PI/180.0)) * amount * cos(m_xangle * (XM_PI/180.0));

		XMMATRIX identity = XMMatrixIdentity();

	root_node->update_collision_tree(&identity,1.0);
	if(check_collision(root_node) == true){
		m_x = old_m_x;
		m_y = old_m_y;
		m_z = old_m_z;
		return true;
	}
	return false;
}

XMVECTOR scene_node::get_position(){
 return XMVectorSet(m_x,m_y,m_z,0.0);
}

XMVECTOR scene_node::get_world_centre_position(){
	return XMVectorSet(m_world_centre_x, 
			  m_world_centre_y, 
			  m_world_centre_z, 0.0);
}

void scene_node::update_collision_tree(XMMATRIX* world,float scale){
	m_local_world_matrix = XMMatrixIdentity();
	m_local_world_matrix = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	m_local_world_matrix *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	m_local_world_matrix *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	m_local_world_matrix *= XMMatrixScaling(m_scale, m_scale, m_scale);
	m_local_world_matrix *= XMMatrixTranslation(m_x, m_y, m_z);
	m_local_world_matrix *= *world;
	m_world_scale = scale * m_scale;
	XMVECTOR v;
	if(m_p_model){
		v = XMVectorSet(m_p_model->GetBoundingSphere_x(),
			m_p_model->GetBoundingSphere_y(),
			m_p_model->GetBoundingSphere_z(),0.0);
	}
	else
		v = XMVectorSet(0,0,0,0); //occurs in the absence of a model
	v = XMVector3Transform(v, m_local_world_matrix);
	m_world_centre_x = XMVectorGetX(v);
	m_world_centre_y = XMVectorGetY(v);
	m_world_centre_z = XMVectorGetZ(v);

	for(int i = 0; i < m_children.size(); i++){
		m_children[i]->update_collision_tree(&m_local_world_matrix,m_world_scale);
	}
}

bool scene_node::check_collision(scene_node* compare_tree){
		return check_collision(compare_tree, this);
	}

bool scene_node::check_collision(scene_node* compare_tree, scene_node* object_tree_root){
	if(object_tree_root == compare_tree) 
		return false;

	if(m_p_model && compare_tree->m_p_model) {
		XMVECTOR v1 = get_world_centre_position();
		XMVECTOR v2 = compare_tree->get_world_centre_position();
		XMVECTOR vdiff = v1-v2;

		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		if(sqrt(dx*dx+dy*dy+dz*dz) < 
		  (compare_tree->m_p_model->GetBoundingSphereRadius() * compare_tree->m_world_scale)+ 
		   (this->m_p_model->GetBoundingSphereRadius() * m_world_scale)){
				 return true;
		}
	}
	
	for(int i = 0; i< compare_tree->m_children.size();i++) {
	  if(check_collision(compare_tree->m_children[i], object_tree_root) == true) 
		  return true; 
	}

	for(int i = 0; i< m_children.size();i++) {
	  if(m_children[i]->check_collision(compare_tree, object_tree_root) == true) 
		  return true; 
	}

	return false;
}

bool scene_node::check_collision_ray(float x,
	float y,float z,float rx,float ry,float rz){
		if(m_p_model){
			XMVECTOR v1 = get_world_centre_position();
			XMVECTOR v2 = XMVectorSet(x,y,z,0.0f);

			XMVECTOR vdiff = v1 - v2;
			float raydist = sqrt(rx*rx+ry*ry+rz*rz);

			float x1 =XMVectorGetX(v1);
			float x2 =XMVectorGetX(v2);
			float y1 =XMVectorGetY(v1);
			float y2 =XMVectorGetY(v2);
			float z1 =XMVectorGetZ(v1);
			float z2 =XMVectorGetZ(v2);

			float dx = x1 - x2;
			float dy = y1 - y2;
			float dz = z1 - z2;

			if(sqrt(dx*dx+dy*dy+dz*dz) < m_p_model->GetBoundingSphereRadius()*m_scale + raydist){
				XMVECTOR p1,p2,p3;
				xyz triangle[3];
				plane triplane;
				float start_ray_plane,end_ray_plane;
				xyz start_ray,end_ray,intersect;

				start_ray.x = x;
				start_ray.y = y;
				start_ray.z = z;

				end_ray.x = x + rx;
				end_ray.y = y + ry;
				end_ray.z = z + rz;

				ObjFileModel* pObject = m_p_model->GetObject();

				for(unsigned int i = 0; i< pObject->numverts; i+=3){
					p1 = XMVectorSet(pObject->vertices[i].Pos.x,   pObject->vertices[i].Pos.y,    pObject->vertices[i].Pos.z, 0.0f);
					p2 = XMVectorSet(pObject->vertices[i+1].Pos.x, pObject->vertices[i+1].Pos.y,  pObject->vertices[i+1].Pos.z, 0.0f);
					p3 = XMVectorSet(pObject->vertices[i+2].Pos.x, pObject->vertices[i+2].Pos.y,  pObject->vertices[i+2].Pos.z, 0.0f);

					p1 =  XMVector3Transform(p1, m_local_world_matrix);
					p2 =  XMVector3Transform(p2, m_local_world_matrix);
					p3 =  XMVector3Transform(p3, m_local_world_matrix);

					triangle[0].x = XMVectorGetX(p1);
					triangle[0].y = XMVectorGetY(p1);
					triangle[0].z = XMVectorGetZ(p1);
					triangle[1].x = XMVectorGetX(p2);
					triangle[1].y = XMVectorGetY(p2);
					triangle[1].z = XMVectorGetZ(p2);
					triangle[2].x = XMVectorGetX(p3);
					triangle[2].y = XMVectorGetY(p3);
					triangle[2].z = XMVectorGetZ(p3);

					triplane = CalcPlaneEq(&triangle[0], &triangle[1], &triangle[2]);

					start_ray_plane = CalcPlaneEqValue(&triplane, &start_ray );
					end_ray_plane = CalcPlaneEqValue(&triplane, &end_ray );
						
					if(sign(start_ray_plane) != sign(end_ray_plane)) {
					 intersect = planeIntersection(&triplane, &start_ray, &end_ray);

						if( in_triangle(&triangle[0], &triangle[1], &triangle[2], &intersect)){
							return true;
						}
					}
				}
			}
		}
	for(int j = 0; j  < m_children.size(); j++){
		if(m_children[j]->check_collision_ray( x,  y,  z,  rx,  ry,  rz) == true) 
			return true;
	}
	return false;
}

bool scene_node::closeEnough(float x,float y,float z,float mindist){
			XMVECTOR theObject = get_world_centre_position();
			XMVECTOR Camera = XMVectorSet(x,y,z,0.0f);

			XMVECTOR vdiff = theObject - Camera;

			float x1 =XMVectorGetX(theObject);
			float x2 =XMVectorGetX(Camera);
			float y1 =XMVectorGetY(theObject);
			float y2 =XMVectorGetY(Camera);
			float z1 =XMVectorGetZ(theObject);
			float z2 =XMVectorGetZ(Camera);

			float dx = x1 - x2;
			float dy = y1 - y2;
			float dz = z1 - z2;

			if(sqrt(dx*dx+dy*dy+dz*dz) < m_p_model->GetBoundingSphereRadius()*m_scale + mindist)
				return true;
			else
				return false;
}

Model* scene_node::GetModel(void)
{
	return m_p_model;
}