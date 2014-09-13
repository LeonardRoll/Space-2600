#pragma once

#include "ObjFileModel.h"

class Model
{
private:

	ID3D11Device*           m_pD3DDevice;
	ID3D11DeviceContext*    m_pImmediateContext;

	ObjFileModel*			m_pObject;
	ID3D11VertexShader*		m_pVShader;         
	ID3D11PixelShader*		m_pPShader;          
	ID3D11InputLayout*		m_pInputLayout;  
	ID3D11Buffer*			m_pConstantBuffer;   

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

	float m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z;
	float m_bounding_sphere_radius; 
	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

	ID3D11ShaderResourceView*	m_pTex;   
	ID3D11SamplerState*			m_pSampler0; 

	XMVECTOR m_directional_light_shines_from;
	XMVECTOR m_directional_light_colour;
	XMVECTOR m_ambient_light_colour;
 
public:
	Model(ID3D11Device* device, ID3D11DeviceContext* context );
	~Model();

	ObjFileModel* GetObject();
	int LoadObjModel(char* s);
	void NoCollision();
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection); //14
	void LookAt_XZ(float x, float z);
	void MoveForwards(float amount);
	void LookAt_XYZ(float x, float ny,float z);
	void MoveForwards_XYZ(float amount);

	void SetXPos(float in);
	float GetXPos(void);
	void IncX(float in);

	void SetYPos(float in);
	float GetYPos(void);
	void IncY(float in);

	void SetZPos(float in);
	float GetZPos(void);
	void IncZ(float in);

	void SetXRot(float in);
	float GetXRot(void);
	void IncXRot(float in);	
	
	void SetYRot(float in);
	float GetYRot(void);
	void IncYRot(float in);

	void SetZRot(float in);
	float GetZRot(void);
	void IncZRot(float in);

	void SetScale(float in);
	float GetScale(void);
	void IncScale(float in);

	float GetBoundingSphere_x();
	float GetBoundingSphere_y();
	float GetBoundingSphere_z();

	float GetBoundingSphereRadius();
	bool CheckCollision(Model* m);
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	void SetTexture(ID3D11ShaderResourceView* t);
	void SetSampler(ID3D11SamplerState* s);

	void LightSource(float x, float y, float z);

};
