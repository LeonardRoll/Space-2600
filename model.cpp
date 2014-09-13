#include "model.h"

// Define constant buffer structs
// Alway pack to 16 bytes. 
// Don't allow a single variable to cross 16 byte boundary, pack to enforce.
struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection ;		// 64 bytes ( 4 x 4 = 16 floats x 4 bytes)
	XMVECTOR directional_light_vector;	// 16 bytes
	XMVECTOR directional_light_colour;	// 16 bytes
	XMVECTOR ambient_light_colour;		// 16 bytes
}; // TOTAL SIZE = 112 bytes

Model::Model(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext)
{
	m_pD3DDevice = pD3DDevice;
	m_pImmediateContext = pImmediateContext;

	m_x=0.0f;
	m_y=0.0f;
	m_z=0.0f;
	m_xangle=0.0f;
	m_yangle=0.0f;
	m_zangle=0.0f;
	m_yangle=0.0f;
	m_scale=1.0f;

	m_pTex = NULL; 
	m_pSampler0 = NULL; 
}

void Model::SetXPos(float in){
	m_x = in;
}

float Model::GetXPos(){
	return m_x;
}

void Model::IncX(float in){
	m_x += in;
}

void Model::SetYPos(float in){
	m_y = in;
}

float Model::GetYPos(){
	return m_y;
}

void Model::IncY(float in){
	m_y += in;
}

void Model::SetZPos(float in){
	m_z = in;
}

float Model::GetZPos(){
	return m_z;
}

void Model::IncZ(float in){
	m_z += in;
}

void Model::SetXRot(float in){
	m_xangle = in;
}

float Model::GetXRot(){
	return m_xangle;
}

void Model::IncXRot(float in){
	m_xangle += in;
}

void Model::SetYRot(float in){
	m_yangle = in;
}

float Model::GetYRot(){
	return m_yangle;
}

void Model::IncYRot(float in){
	m_yangle += in;
}

void Model::SetZRot(float in){
	m_zangle = in;
}

float Model::GetZRot(){
	return m_zangle;
}

void Model::IncZRot(float in){
	m_zangle += in;
}

void Model::SetScale(float in){
	m_scale = in;
}

float Model::GetScale(){
	return m_scale;
}

void Model::IncScale(float in){
	m_scale += in;
}

float Model::GetBoundingSphere_x(){
	return m_bounding_sphere_centre_x;
}

float Model::GetBoundingSphere_y(){
	return m_bounding_sphere_centre_y;
}

float Model::GetBoundingSphere_z(){
	return m_bounding_sphere_centre_z;
}

float Model::GetBoundingSphereRadius(){
	return m_bounding_sphere_radius*m_scale;
}

int Model::LoadObjModel(char* s)
{
	// LOAD A MODEL
	m_pObject = new ObjFileModel(s, m_pD3DDevice, m_pImmediateContext);//11

	if(m_pObject->filename == "FILE NOT LOADED") 
	{
		return S_FALSE;
	}

	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	HRESULT hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if(error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if(FAILED(hr))
		{
			return hr;
		};
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if(error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if(FAILED(hr))
		{
			return hr;
		};
	}

	// Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);

	if(FAILED(hr))
    {
        return hr;
    }
	
	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);

	if(FAILED(hr))
    {
        return hr;
    }

	// Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		 {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		 {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if(FAILED(hr))
        return hr;

	// Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;			// Can use VerticaldateSubresource() to update
	constant_buffer_desc.ByteWidth = 64;						// MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if(FAILED(hr)) 
		return hr;

	CalculateModelCentrePoint();//12
	CalculateBoundingSphereRadius();//12

	return S_OK;
} 

void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX world;
	XMMATRIX transpose;
	XMVECTOR m_directional_light_shines_from;
	XMVECTOR m_directional_light_colour;
	XMVECTOR m_ambient_light_colour;

	//Lighting
	m_directional_light_shines_from =  XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	m_directional_light_colour = XMVectorSet(0.3f, 0.6f, 0.8f, 0.0f);
	m_ambient_light_colour = XMVectorSet(1.0f, 0.1f, 0.1f, 1.0f);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTex);

	//world = XMMatrixIdentity();
	world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));	
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	world *=XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x,m_y,m_z);
	transpose = XMMatrixTranspose(world);

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world*(*view)*(*projection);
	model_cb_values.directional_light_colour = m_directional_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from, transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize( model_cb_values.directional_light_vector );

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	m_pObject->Draw();
}

void Model::Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)//14
{
	XMMATRIX transpose;
	XMVECTOR m_directional_light_shines_from;
	XMVECTOR m_directional_light_colour;
	XMVECTOR m_ambient_light_colour;

	//Lighting
	m_directional_light_colour = XMVectorSet(1.3f, 0.6f, 0.8f, 1.0f);
	m_ambient_light_colour = XMVectorSet(1.0f, 0.1f, 0.1f, 1.0f);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
    m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
    m_pImmediateContext->IASetInputLayout(m_pInputLayout);
    m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTex);
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
 
	transpose = XMMatrixTranspose(*world);
       
	MODEL_CONSTANT_BUFFER model_cb_values;
    model_cb_values.WorldViewProjection = (*world)*(*view)*(*projection);
    model_cb_values.directional_light_colour = m_directional_light_colour;
    model_cb_values.ambient_light_colour = m_ambient_light_colour;
    model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from, transpose);
    model_cb_values.directional_light_vector = XMVector3Normalize( model_cb_values.directional_light_vector );
 
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

    m_pObject->Draw();
}


Model::~Model()
{
	delete m_pObject;
	if(m_pTex) 
		m_pTex->Release();   
	if(m_pSampler0) 
		m_pSampler0->Release();
	if(m_pVShader) 
		m_pVShader->Release();
	if(m_pPShader) 
		m_pPShader->Release();
	if(m_pInputLayout) 
		m_pInputLayout->Release();
	if(m_pConstantBuffer) 
		m_pConstantBuffer->Release();
}

void Model::LookAt_XZ(float nx, float nz)
{
	float dx, dz;
	dx = nx-m_x;dz = nz-m_z;
	m_yangle = atan2(dx,dz)*180.0/XM_PI;
}

void Model::MoveForwards(float amount)
{
		m_x += sin(m_yangle * (XM_PI/180.0)) * amount;
		m_z += cos(m_yangle * (XM_PI/180.0)) * amount;
}

void Model::LookAt_XYZ(float nx, float ny, float nz)
{
	float dx, dy, dz;
	dx = nx-m_x;
	dz = nz-m_z;
	dy=ny-m_y;
	m_yangle = atan2(dx,dz)*180.0/XM_PI;
	m_xangle = -atan2(dy, sqrt(dx*dx+dz*dz))*180.0/XM_PI;
}

void Model::MoveForwards_XYZ(float amount)
{
		m_x += sin(m_yangle * (XM_PI/180.0)) * amount *cos(m_xangle * (XM_PI/180.0));
		m_y += -sin(m_xangle * (XM_PI/180.0)) * amount;
		m_z += cos(m_yangle * (XM_PI/180.0)) * amount * cos(m_xangle * (XM_PI/180.0));
}

void Model::CalculateModelCentrePoint()//12
{
	float max_x=0, min_x=0 ,max_y=0, min_y=0,max_z=0, min_z=0;
	// centre_x, centre_y, centre_z;//13
	for(int i=0; i<m_pObject->numverts; i++)
	{
		if(m_pObject->vertices[i].Pos.x  > max_x)   
			max_x = m_pObject->vertices[i].Pos.x;
		if(m_pObject->vertices[i].Pos.x  < min_x)  
			min_x = m_pObject->vertices[i].Pos.x;
		if(m_pObject->vertices[i].Pos.y  > max_y)   
			max_y = m_pObject->vertices[i].Pos.y;
		if(m_pObject->vertices[i].Pos.y  < min_y)   
			min_y = m_pObject->vertices[i].Pos.y;
		if(m_pObject->vertices[i].Pos.z  > max_z)  
			max_z = m_pObject->vertices[i].Pos.z;
		if(m_pObject->vertices[i].Pos.z  < min_z)  
			min_z = m_pObject->vertices[i].Pos.z;
	}

	m_bounding_sphere_centre_x = (max_x+min_x)/2;
	m_bounding_sphere_centre_y = (max_y+min_y)/2;
	m_bounding_sphere_centre_z = (max_z+min_z)/2;

}

void Model::CalculateBoundingSphereRadius()//12
{
	float temp_x, temp_y, temp_z, dx, dy, dz, distance;

	m_bounding_sphere_radius = 0;

	for(int i=0; i<m_pObject->numverts; i++)
	{
		temp_x = m_pObject->vertices[i].Pos.x;
		temp_y = m_pObject->vertices[i].Pos.y;
		temp_z = m_pObject->vertices[i].Pos.z;

		dx = temp_x - m_bounding_sphere_centre_x;
		dy = temp_y - m_bounding_sphere_centre_y;
		dz = temp_z - m_bounding_sphere_centre_z;

		distance = sqrt(dx*dx + dy*dy + dz*dz);

		if(distance > m_bounding_sphere_radius) m_bounding_sphere_radius = distance;
	}

}

bool Model::CheckCollision(Model* m) //12
{ 
	if(m==this) return false;

	XMVECTOR v1 = GetBoundingSphereWorldSpacePosition();
	XMVECTOR v2 = m->GetBoundingSphereWorldSpacePosition();

	XMVECTOR vdiff = v1-v2;


	//XMVECTOR a = XMVector3Length(vdiff);
	float x1 =XMVectorGetX(v1);
	float x2 =XMVectorGetX(v2);
	float y1 =XMVectorGetY(v1);
	float y2 =XMVectorGetY(v2);
	float z1 =XMVectorGetZ(v1);
	float z2 =XMVectorGetZ(v2);

	float dx = x1 - x2;
	float dy = y1 - y2;
	float dz = z1 - z2;

//	if(XMVectorGetX(a) < m->GetBoundingSphereRadius() + this->GetBoundingSphereRadius()) return true;
	if(sqrt(dx*dx+dy*dy+dz*dz) < m->GetBoundingSphereRadius() + this->GetBoundingSphereRadius()) return true;
	else return false;

}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition() //12
{
	XMMATRIX world;
	XMVECTOR offset;

	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);
	world = XMMatrixRotationX(XMConvertToRadians(m_xangle));	
	world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	world *=XMMatrixScaling(m_scale, m_scale, m_scale);

	world *= XMMatrixTranslation(m_x,m_y,m_z);

	offset = XMVector3Transform(offset, world);
	return offset;
}

void Model::SetTexture(ID3D11ShaderResourceView* t)//14
{
	m_pTex = t;
}

void Model::SetSampler(ID3D11SamplerState* s)//14
{
	m_pSampler0 = s;
}

ObjFileModel* Model::GetObject(){
	return m_pObject;
}

void Model::LightSource(float x,float y,float z){
        m_directional_light_shines_from =  XMVectorSet(500, 0, 500, 0.0f);
}

void Model::NoCollision(){
	m_bounding_sphere_radius = 0;
}