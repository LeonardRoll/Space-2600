////////////////////
//  DXBase Class  //
////////////////////

#include "DXBase.h"

//////////////////////////
//  DXBase Constructor  //
//////////////////////////

DXBase::DXBase(HINSTANCE hInst,HWND hWnd,float width,float height)
{
	m_HInst = hInst;
	m_HWnd = hWnd;
	m_ScrWidth = width;
	m_ScrHeight = height;
	m_driverType = D3D_DRIVER_TYPE_NULL;
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	m_pD3DDevice = NULL;
	m_pImmediateContext = NULL;
	m_p_SwapChain = NULL;
	m_p_BackBufferRTView = NULL;
	m_FHitCounter = 0;
	m_BHitCounter = 0;
	m_RHitCounter = 0;
	m_LHitCounter = 0;
	m_AboHitCounter = 0;
	m_BelHitCounter = 0;
	m_SpeedBarText = "Not Moving";
	cycle_count = 0;
}

/*Grabs camera*/
Camera* DXBase::getCamera()
{
	return m_p_Camera;
}

/*Creates an Input object*/
void DXBase::setInput(Input* input)
{
	m_input = input;
}

//////////////////////////////////////
//  Create D3D Device & Swap Chain  //
//////////////////////////////////////

HRESULT DXBase::InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(m_HWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		/*comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it*/
		D3D_DRIVER_TYPE_HARDWARE, 
		/*comment this out also to use reference device*/
		D3D_DRIVER_TYPE_WARP, 
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_HWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, 
											numFeatureLevels, D3D11_SDK_VERSION, &sd, & m_p_SwapChain, 
											& m_pD3DDevice, & m_featureLevel, & m_pImmediateContext);
		if(SUCCEEDED(hr))
			break;
	}

	if(FAILED(hr))
		return hr;

	/*Get pointer to back buffer texture*/
	ID3D11Texture2D *pBackBufferTexture;
	hr = m_p_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                      (LPVOID*)&pBackBufferTexture);

	if(FAILED(hr)) return hr;
	
	/*Use the back buffer texture pointer to create the render target view*/
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
                                                    &m_p_BackBufferRTView);
	pBackBufferTexture->Release();

	if(FAILED(hr)) return hr;
	

	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = m_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if(FAILED(hr)) return hr;

	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &m_pZBuffer);
	pZBufferTexture->Release();

	/*Set the render target view*/
	m_pImmediateContext->OMSetRenderTargets(1, & m_p_BackBufferRTView, m_pZBuffer);

	/*Set the viewport*/
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &viewport);

	return S_OK;
}


//////////////////////
//  Deleter Method  //
//////////////////////

void DXBase::ShutdownD3D()
{
	delete m_RootNode;
	
	m_input->~Input();

	if(m_pAlphaBlendEnable) m_pAlphaBlendEnable->Release();	
	if(m_pAlphaBlendDisable) m_pAlphaBlendDisable->Release();

	delete m_HUD;
	delete m_HUD2;
	delete m_HUD3;
	delete m_HUDFHit;
	delete m_HUDBHit;
	delete m_HUDRHit;
	delete m_HUDLHit;
	delete m_HUDAboHit;
	delete m_HUDBelHit;
	delete m_HUDMoveF;
	delete m_HUDMoveB;
	delete m_HUDMoveR;
	delete m_HUDMoveL;
	delete m_HUDSpeedBar;

	//if(m_p_Tex0) m_p_Tex0->Release();
	//if(m_p_Tex1) m_p_Tex1->Release();
	//if(m_pSampler0) m_pSampler0->Release();

	delete m_p_Camera;
	if(m_pZBuffer) m_pZBuffer->Release();
	if(m_pConstantBuffer0) m_pConstantBuffer0->Release();
	if(m_pVertexBuffer) m_pVertexBuffer->Release();
	if(m_pInputLayout) m_pInputLayout->Release();
	if(m_pVertexShader) m_pVertexShader->Release();
	if(m_pPixelShader) m_pPixelShader->Release();

	if(m_p_BackBufferRTView) m_p_BackBufferRTView->Release();
	if(m_p_SwapChain) m_p_SwapChain->Release();
	if(m_pImmediateContext) m_pImmediateContext->Release();
	if(m_pD3DDevice) m_pD3DDevice->Release();
}

//////////////////////////////////
//  Graphics Initialiser Method //
//////////////////////////////////

HRESULT DXBase::InitialiseGraphics()
{
	HRESULT hr = S_OK;
	srand(time(NULL)) ;
	m_p_Camera = new Camera(0.0f, 0.0f, 0.0f, 0.0f);
	m_player = new Player(100);
	gotHit = false;
	RanFar = 0;
	
	m_UFO = new Model(m_pD3DDevice, m_pImmediateContext);
	hr = m_UFO->LoadObjModel("assets/UFOMk2.obj");
	if(FAILED(hr)){
		return hr;
	}

	m_Mines = new Model(m_pD3DDevice, m_pImmediateContext);
	hr = m_Mines->LoadObjModel("assets/Mines.obj");
	if(FAILED(hr)){
		return hr;
	}
	
	m_Tokens = new Model(m_pD3DDevice, m_pImmediateContext);
	hr = m_Tokens->LoadObjModel("assets/Token.obj");
	if(FAILED(hr)){
		return hr;
	}

	for(int i = 0 ; i < 20 ; i++){
		m_Planets.push_back(new Model(m_pD3DDevice,m_pImmediateContext));
		hr = m_Planets[i]->LoadObjModel("assets/sphere.obj");
		if(FAILED(hr)){
			return hr;
		}
			m_PlanetTextures.push_back(NULL);
			m_planets.push_back(new scene_node());
			m_p_Moons.push_back(new scene_node());
	}


	m_Moon = new Model(m_pD3DDevice, m_pImmediateContext);
	hr = m_Moon->LoadObjModel("assets/sphere.obj");
	if(FAILED(hr)){
		return hr;
	}

	/*Scene node stuff*/
	m_RootNode = new scene_node();
	m_PlanetNode = new scene_node();
	m_Tokens_Source = new scene_node();
	m_Mines_Source = new scene_node();
	m_UFO_node = new scene_node();
	m_Plane_Source = new scene_node();
	
	m_RootNode->addChildNode(m_UFO_node);
	m_UFO_node->SetModel(m_UFO);

	m_RootNode->addChildNode(m_Plane_Source);
	for(int i = 0 ; i < 6 ; i++){
		m_Star_Plane.push_back(new Model(m_pD3DDevice, m_pImmediateContext));
		hr = m_Star_Plane[i]->LoadObjModel("assets/Plane.obj");
			if(FAILED(hr)){
			return hr;
			}
		m_Star_Plane[i]->NoCollision();
		m_planet_Plane_nodes.push_back(new scene_node);
		m_planet_Plane_nodes[i]->SetModel(m_Star_Plane[i]);
		m_Plane_Source->addChildNode(m_planet_Plane_nodes[i]);
	}

	for(int i = 0 ; i < m_planets.size(); i++){
		m_planets[i]->SetModel(m_Planets[i]);
		m_p_Moons[i]->SetModel(m_Moon);
	}	

	for(int i = 0; i < 20 ; i++){ // <<<<<<<<<<<<<<< ------ 001
			m_Mines_Nodes.push_back(new scene_node());
		}

	for(int i = 0 ; i < 20 ; i++){
		m_Token_Nodes.push_back(new scene_node());
	}

	for(int i = 0 ; i < m_Mines_Nodes.size() ; i++){
		m_Mines_Nodes[i]->SetModel(m_Mines);
		m_Mines_Nodes[i]->SetScale(0.3);
	}

	for(int i = 0 ; i < m_Token_Nodes.size() ; i++){
		m_Token_Nodes[i]->SetModel(m_Tokens);
		m_Token_Nodes[i]->SetScale(0.8);
	}

	m_RootNode->addChildNode(m_PlanetNode);
	for(int i = 0 ; i < m_planets.size() ; i++){
		m_PlanetNode->addChildNode(m_planets[i]);
		m_planets[i]->addChildNode(m_p_Moons[i]);
		m_planets[i]->SetXPos(RandFloatX(700,0));
		m_planets[i]->SetYPos(RandFloatY(100,0));
		m_planets[i]->SetZPos(RandFloatZ(700,0));
		m_planets[i]->SetScale(RandScale(10,0));
	}
	
	m_RootNode->addChildNode(m_Mines_Source);
	for(int i = 0 ; i < m_Mines_Nodes.size() ; i++){
		m_Mines_Source->addChildNode(m_Mines_Nodes[i]);
		m_Mines_Nodes[i]->SetXPos(RandFloatX(700,0));
		m_Mines_Nodes[i]->SetYPos(RandFloatY(100,0));
		m_Mines_Nodes[i]->SetZPos(RandFloatZ(700,0));
		//m_Mines_Nodes[i]->SetXPos(20);
		//m_Mines_Nodes[i]->SetYPos(20);
		//m_Mines_Nodes[i]->SetZPos(20);
	}
	
	m_RootNode->addChildNode(m_Tokens_Source);
	for(int i = 0 ; i < m_Token_Nodes.size() ; i++){
		m_Tokens_Source->addChildNode(m_Token_Nodes[i]);
		m_Token_Nodes[i]->SetXPos(RandFloatX(700,0));
		m_Token_Nodes[i]->SetYPos(RandFloatY(100,0));
		m_Token_Nodes[i]->SetZPos(RandFloatZ(700,0));
	}

	//Set Moons
	for(int i = 0; i < m_p_Moons.size() ; i++){
		m_p_Moons[i]->SetScale(0.1);
		m_p_Moons[i]->SetXPos(5);
	}

	//Plane Setup
	//Forward
	m_planet_Plane_nodes[0]->SetZPos(1000);
	m_planet_Plane_nodes[0]->SetScale(200);
	//Backward
	m_planet_Plane_nodes[1]->SetZPos(-1000);
	m_planet_Plane_nodes[1]->SetScale(200);
	//Left
	m_planet_Plane_nodes[2]->SetXPos(1000);
	m_planet_Plane_nodes[2]->SetScale(200);
	//Right
	m_planet_Plane_nodes[3]->SetXPos(-1000);
	m_planet_Plane_nodes[3]->SetScale(200);
	//Above
	m_planet_Plane_nodes[4]->SetYPos(-1000);
	m_planet_Plane_nodes[4]->SetScale(200);
	//Below
	m_planet_Plane_nodes[5]->SetYPos(1000);
	m_planet_Plane_nodes[5]->SetScale(200);

	for(int i = 0; i < m_planet_Plane_nodes.size();i++){
		m_planet_Plane_nodes[i]->LookAt_XYZ(0, 0,0, m_RootNode);
	}

	//Camera Center to level
	m_p_Camera->SetX(500);
	m_p_Camera->SetY(0);
	m_p_Camera->SetZ(500);
	
	/*Set up and create vertex buffer*/
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	
	/*Used by CPU and GPU*/
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;		
	
	/*Total size of buffer*/ 
	bufferDesc.ByteWidth = 100;	
	
	/*Use as a vertex buffer*/
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	
	
	/*Allow CPU access*/
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	/*Create the buffer*/
	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	/*Return error code on failure*/
	if(FAILED(hr)) {
		 return hr;
	}

	/*Copy the vertices into the buffer*/
	D3D11_MAPPED_SUBRESOURCE ms;
	
	/*Lock the buffer to allow writing*/
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	/*Unlock the buffer*/
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);	

	/*Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only*/
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	/*Check for shader compilation error*/
	if(error != 0) {
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		
		/*Don't fail if error is just a warning*/
		if(FAILED(hr)) {
			return hr;
		};
	}


	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	/*Check for shader compilation error*/
	if(error != 0){
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		
		/*Don't fail if error is just a warning*/
		if(FAILED(hr)){
			return hr;
		};
	}

	/*Create shader objects*/
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVertexShader);

	if(FAILED(hr)){
		return hr;
	}
	
	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPixelShader);

	if(FAILED(hr)){
		return hr;
	}

	/*Set the shader objects as active*/
	m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);

	/*Create and set the input layout object*/
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
	  {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	  {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	  {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	  {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	m_pD3DDevice->CreateInputLayout(	iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), 
					VS->GetBufferSize(), &m_pInputLayout);

	if(FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	/*Create constant buffer*/
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	/*Can use VerticaldateSubresource() to update*/
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;	
	
	/*MUST be a multiple of 16, calculate from CB struct*/
	constant_buffer_desc.ByteWidth = 144;	
	
	/*Use as a constant buffer*/
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer0);

	if(FAILED(hr)) return hr;

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, 
					"assets/Barrakus.bmp",NULL, NULL, 
					& m_PlanetTextures[3], NULL);
	
	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, 
					"assets/sun.bmp",NULL, NULL, 
					& m_PlanetTextures[0], NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, 
					"assets/Thunrad.bmp",NULL, NULL, 
					& m_PlanetTextures[1], NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, 
					"assets/dead.bmp",NULL, NULL, 
					& m_PlanetTextures[2], NULL);
	
	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/Moon.bmp",NULL, NULL,
					& m_p_MoonTexture, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/GreenNormal.bmp",NULL, NULL,
					& m_p_MineTexture_Seeking, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/RedAlert.bmp",NULL, NULL,
					& m_p_MineTexture_Active, NULL);
	
	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/XNegSkyBox.bmp",NULL, NULL,
					& m_Star_Plane_Texture_XNeg, NULL);
	
	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/XPosSkyBox.bmp",NULL, NULL,
					& m_Star_Plane_Texture_XPos, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/YNegSkyBox.bmp",NULL, NULL,
					& m_Star_Plane_Texture_YNeg, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/YPosSkyBox.bmp",NULL, NULL,
					& m_Star_Plane_Texture_YPos, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/ZPosSkyBox.bmp",NULL, NULL,
					& m_Star_Plane_Texture_ZPos, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/ZNegSkyBox.bmp",NULL, NULL,
					& m_Star_Plane_Texture_ZNeg, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/Token.bmp",NULL, NULL,
					& m_p_TokenTexture, NULL);

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice,
					"assets/UFOTexture.bmp",NULL, NULL,
					& m_p_UFOTexture, NULL);

	D3D11_SAMPLER_DESC sampler_desc; 
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);
	m_Mines->SetTexture(m_p_MineTexture_Seeking);
	m_Mines->SetSampler(m_pSampler0);
	m_Tokens->SetTexture(m_p_TokenTexture);
	m_Tokens->SetSampler(m_pSampler0);
	m_Moon->SetTexture(m_p_MoonTexture);
	m_Moon->SetSampler(m_pSampler0);
	m_UFO->SetTexture(m_p_UFOTexture);
	m_UFO->SetSampler(m_pSampler0);
	
	//Skybox
	m_Star_Plane[0]->SetTexture(m_Star_Plane_Texture_ZPos);
	m_Star_Plane[0]->SetSampler(m_pSampler0);
	m_Star_Plane[1]->SetTexture(m_Star_Plane_Texture_ZNeg);
	m_Star_Plane[1]->SetSampler(m_pSampler0);
	m_Star_Plane[2]->SetTexture(m_Star_Plane_Texture_XNeg);
	m_Star_Plane[2]->SetSampler(m_pSampler0);
	m_Star_Plane[3]->SetTexture(m_Star_Plane_Texture_XPos);
	m_Star_Plane[3]->SetSampler(m_pSampler0);
	m_Star_Plane[4]->SetTexture(m_Star_Plane_Texture_YPos);
	m_Star_Plane[4]->SetSampler(m_pSampler0);
	m_Star_Plane[5]->SetTexture(m_Star_Plane_Texture_YNeg);
	m_Star_Plane[5]->SetSampler(m_pSampler0);

	int i = 0;
	int j = 0;
	do{
		if(j >= 3){
			j = 0;}
		m_Planets[i]->SetTexture(m_PlanetTextures[j]);
		m_Planets[i]->SetSampler(m_pSampler0);
		i++;
		j++;
	}while(i < m_Planets.size());

	m_HUD = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUD2 = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUD3 = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDFHit = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDBHit = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDRHit = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDLHit = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDAboHit = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDBelHit = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDMoveF = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDMoveB = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDMoveR = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDMoveL = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);
	m_HUDSpeedBar = new Text2D("assets/font2.bmp", m_pD3DDevice, m_pImmediateContext);

	D3D11_BLEND_DESC b;
	b.RenderTarget[0].BlendEnable = TRUE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = FALSE;
	b.AlphaToCoverageEnable = FALSE;

	m_pD3DDevice->CreateBlendState(&b, &m_pAlphaBlendEnable);

	b.RenderTarget[0].BlendEnable = FALSE;
	m_pD3DDevice->CreateBlendState(&b, &m_pAlphaBlendDisable);

	return S_OK;
}

///////////////////////////
//  Render Frame Method  //
///////////////////////////

void DXBase::RenderFrame()
{
	m_directional_light_shines_from =  XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	m_directional_light_colour = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	static float f = 0.0f;
	f=f+1.0f;
	srand(time(NULL)) ;

	static float m_Mine_Speed = 0.9f;

	/*The game space colour, is black, cause space.*/
	float g_Gamespace_Colour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_input->ReadInputStates();
	m_input->Player_Movement(m_RootNode);

	/*Clear the back buffer with the game space colour*/
	m_pImmediateContext->ClearRenderTargetView(m_p_BackBufferRTView, g_Gamespace_Colour);
	
	m_pImmediateContext->ClearDepthStencilView
		(m_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX projection, world, view, inverse, transpose, identity;

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), (float)m_ScrWidth/m_ScrHeight, .001f, 1000000.0f);

	view = m_p_Camera->GetViewMatrix();

	identity = XMMatrixIdentity();

	if(RanFar >= 40){
		gotHit = false;
		RanFar = 0;
	}

	if(gotHit){
		if(m_UFO_node->moveforward_XYZ(-10.0f,m_RootNode)){
					m_UFO_node->IncrementYRot(0.5f,m_RootNode);
					m_UFO_node->moveforward_XYZ(1.0f,m_RootNode);
					gotHit = false;
		}

		else{
			m_UFO_node->moveforward_XYZ(-1.0f,m_RootNode);
			RanFar++;
		}
	}

	if(m_UFO_node->closeEnough(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(),100)){
		m_UFO_node->LookAt_XYZ(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(), m_RootNode);
		if(m_UFO_node->closeEnough(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(),2)) //Camera Collision
			{
				gotHit = true;
			}
	}

	else{
			if(cycle_count >= 10){
				m_UFO_node->LookAt_XYZ(rand() % 600 - 1,rand() % 10 - 1,rand() % 600 - 1, m_RootNode);
			}
			else
				m_UFO_node->moveforward_XYZ(m_Mine_Speed,m_RootNode);
	}

	for(int i = 0 ; i < m_Mines_Nodes.size() ; i++){
		
		if(m_Mines_Nodes[i]->closeEnough(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(),100)){
			m_Mines_Nodes[i]->LookAt_XYZ(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(), m_RootNode);
			m_Mines_Nodes[i]->moveforward_XYZ(m_Mine_Speed,m_RootNode);
		}
		
		else{
			if(cycle_count >= 10){
				m_Mines_Nodes[i]->LookAt_XYZ(rand() % 600 - 1,rand() % 10 - 1,rand() % 600 - 1, m_RootNode);
			}
			else
				m_Mines_Nodes[i]->moveforward_XYZ(m_Mine_Speed,m_RootNode);
		}
	
		if(m_Mines_Nodes[i]->closeEnough(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(),2)) //Camera Collision
		{
			m_player->damageShields(10);
			m_player->reduceScore(m_Mine_Speed);
			m_HUDFHit->AddText("Front Hit", -0.20, 0.1, .05);
			m_Mines_Nodes[i]->SetModel(NULL);
			m_Mines_Nodes.erase(m_Mines_Nodes.begin() + i);
			m_Mines_Nodes.shrink_to_fit();			
		}	
	}

	if(cycle_count >= 10){
		for(int i = 0 ; i < m_Token_Nodes.size(); i++){
			m_Token_Nodes[i]->LookAt_XYZ(rand() % 600 - 1,rand() % 10 - 1,rand() % 600 - 1, m_RootNode);
			cycle_count = 0;
		}
	}
	else{
		for(int i = 0 ; i < m_Token_Nodes.size(); i++){
			if(m_Token_Nodes[i]->closeEnough(m_p_Camera->GetX(), m_p_Camera->GetY(),m_p_Camera->GetZ(),2)) //Camera Collision
				{
					m_player->addScore(m_input->GetCurrentSpeed());
					m_HUDFHit->AddText("Front Hit", -0.20, 0.1, .05);
					m_Token_Nodes[i]->SetModel(NULL);
					m_Token_Nodes.erase(m_Token_Nodes.begin() + i);
					m_Token_Nodes.shrink_to_fit();			
				}
			else
				m_Token_Nodes[i]->moveforward_XYZ(0.1f,m_RootNode);	
		}
		cycle_count++;
	}

	for(int i = 0 ; i < m_planets.size() ; i++)
	{
		m_planets[i]->SetYRot(f/4);
	}


	for(int i = 0; i < m_planets.size(); i++)
	{
		m_planets[i]->GetModel()->LightSource(m_p_Camera->GetX(), m_p_Camera->GetdY(), m_p_Camera->GetZ());
	}

	for(int i = 0; i < m_p_Moons.size(); i++)
	{
		m_p_Moons[i]->GetModel()->LightSource(m_p_Camera->GetX(), m_p_Camera->GetdY(), m_p_Camera->GetZ());
	}

	
	for(int i = 0; i < m_Mines_Nodes.size(); i++)
	{
		m_Mines_Nodes[i]->GetModel()->LightSource(m_p_Camera->GetX(), m_p_Camera->GetdY(), m_p_Camera->GetZ());
	}

	m_RootNode->execute(&identity, &view, &projection);

	SetHUD();

	m_pImmediateContext->OMSetBlendState(m_pAlphaBlendEnable, 0, 0xffffffff);
	m_HUD->RenderText();
	m_HUD2->RenderText();
	m_HUD3->RenderText();
	m_HUDFHit->RenderText();
	m_HUDBHit->RenderText();
	m_HUDRHit->RenderText();
	m_HUDLHit->RenderText();
	m_HUDAboHit->RenderText();
	m_HUDBelHit->RenderText();
	m_HUDMoveF->RenderText();
	m_HUDMoveB->RenderText();
	m_HUDMoveR->RenderText();
	m_HUDMoveL->RenderText();
	m_HUDSpeedBar->RenderText();

	m_pImmediateContext->OMSetBlendState(m_pAlphaBlendDisable, 0, 0xffffffff);

	/*Display what has just been rendered*/
	m_p_SwapChain->Present(1, 0);
}


float DXBase::RandFloatX(int max,int min){
		 // This will ensure a really randomized number by help of time.
		float xRan=rand()% max + min; // Randomizing the number between 1-15.
		return xRan;
}

float DXBase::RandFloatY(int max,int min){
		 // This will ensure a really randomized number by help of time.
		float xRan=rand() % max + min; // Randomizing the number between 1-15.
		return xRan;
}

float DXBase::RandFloatZ(int max,int min){
		 // This will ensure a really randomized number by help of time.
		 float xRan = rand() % max + min; // Randomizing the number between 1-15.
		 return xRan;
}


float DXBase::RandScale(int max,int min){
		 // This will ensure a really randomized number by help of time.
		 float xRan = rand() % max + min; // Randomizing the number between 1-15.
		 return xRan;
}

void DXBase::SetHUD(void)
{

	/*-1.0 to +1.0 for x and y -> top left of screen*/
	/*Z is fraction of screen size*/
	m_HUD->AddText("Shields: ", -1.0, +1.0, .05);
	m_HUD2->AddText(" :Tokens Required", 0.15, +1.0, .05);
	m_HUD3->AddText("Level: ", -1.0, -0.95, .05);


	//////////VFront Hit Message ControlV//////////

	if(m_p_Camera->ObjHit(m_RootNode, m_input->GetCurrentSpeed()) == 1)
	{
		/*Starts the message display control counter*/
		m_FHitCounter = 100;
	}

	/*Continues displaying message for a set time*/
	if(m_FHitCounter != 0)
	{
		//m_HUDFHit->AddText("Front Hit", -0.20, 0.1, .05);
		m_FHitCounter--;
	}
	else
	{
		m_HUDFHit->AddText("",  -0.20, 0.1, .05);
	}
	
	//////////VBehind Hit Message ControlV//////////

	if(m_p_Camera->ObjHit(m_RootNode, m_input->GetCurrentSpeed()) == 2)
	{
		/*Starts the message display control counter*/
		m_BHitCounter = 100;
	}

	/*Continues displaying message for a set time*/
	if(m_BHitCounter != 0)
	{
		m_HUDBHit->AddText("Hit From Behind", -0.20, 0.05, .05);
		m_BHitCounter--;
	}
	else
	{
		m_HUDBHit->AddText("", -0.20, 0.05, .05);
	}

	//////////VHit on the Right Message ControlV//////////

	if(m_p_Camera->ObjHit(m_RootNode, m_input->GetCurrentSpeed()) == 3)
	{
		/*Starts the message display control counter*/
		m_RHitCounter = 100;
	}

	/*Continues displaying message for a set time*/
	if(m_RHitCounter != 0)
	{
		m_HUDRHit->AddText("Right Hit",  0.5, 0.0, .05);
		m_RHitCounter--;
	}
	else
	{
		m_HUDRHit->AddText("",  0.5, 0.0, .05);
	}

	//////////VHit on the Left Message ControlV//////////

	if(m_p_Camera->ObjHit(m_RootNode, m_input->GetCurrentSpeed()) == 4)
	{
		/*Starts the message display control counter*/
		m_LHitCounter = 100;
	}

	/*Continues displaying message for a set time*/
	if(m_LHitCounter != 0)
	{
		m_HUDLHit->AddText("Left Hit", -0.95, 0.0, .05);
		m_LHitCounter--;
	}
	else
	{
		m_HUDLHit->AddText("",  -0.95, 0.0, .05);
	}

	//////////VHit on the Above Message ControlV//////////

	if(m_p_Camera->ObjHit(m_RootNode, m_input->GetCurrentSpeed()) == 5)
	{
		/*Starts the message display control counter*/
		m_AboHitCounter = 100;
	}

	/*Continues displaying message for a set time*/
	if(m_AboHitCounter != 0)
	{
		m_HUDAboHit->AddText("Above Hit", -0.20, 0.8, .05);
		m_AboHitCounter--;
	}
	else
	{
		m_HUDAboHit->AddText("",  -0.20, 0.8, .05);
	}

	
	//////////VHit on the Below Message ControlV//////////

	if(m_p_Camera->ObjHit(m_RootNode, m_input->GetCurrentSpeed()) == 6)
	{
		/*Starts the message display control counter*/
		m_BelHitCounter = 100;
	}

	/*Continues displaying message for a set time*/
	if(m_BelHitCounter != 0)
	{
		m_HUDBelHit->AddText("Below Hit", -0.20, -0.8, .05);
		m_BelHitCounter--;
	}
	else
	{
		m_HUDBelHit->AddText("",  -0.20, -0.8, .05);
	}

	if(m_input->GetForwardSpeed() > m_input->GetBackwardsSpeed())
	{
		m_HUDMoveF->AddText("Forwards", 0.4, -0.7, .05);
	}
	else if(m_input->GetBackwardsSpeed() > m_input->GetForwardSpeed())
	{
		m_HUDMoveB->AddText("Backwards",  0.4, -0.9, .05);
	}
	
	if(m_input->GetRightSpeed() > m_input->GetLeftSpeed())
	{
		m_HUDMoveB->AddText("Right", 0.75, -0.8, .05);
	}
	else if(m_input->GetLeftSpeed() > m_input->GetRightSpeed())
	{
		m_HUDMoveB->AddText("Left", 0.25, -0.8, .05);
	}
	
	if(m_input->GetForwardSpeed() == 0.0)
	{
		m_SpeedBarText = "Zzz";
	}
	else if ((m_input->GetForwardSpeed() >= 0.001) && (m_input->GetForwardSpeed() < 5.0))
	{
		m_SpeedBarText = "Acc x1";
	}
	else if ((m_input->GetForwardSpeed() >= 5.0) && (m_input->GetForwardSpeed() < 10.0))
	{
		m_SpeedBarText = "Acc x2";
	}
	else if((m_input->GetForwardSpeed() == 10.0))
	{
		m_SpeedBarText = "Max Acc";
	}
	else if ((m_input->GetForwardSpeed() <= -0.001) && (m_input->GetForwardSpeed() > -5.0))
	{
		m_SpeedBarText = "Rev x1";
	}
	else if ((m_input->GetForwardSpeed() <= -5.0) && (m_input->GetForwardSpeed() > -10.0))
	{
		m_SpeedBarText = "Rev x2";
	}
	else if((m_input->GetForwardSpeed() == -10.0))
	{
		m_SpeedBarText = "Full Rev";
	}

	m_HUDSpeedBar->AddText(m_SpeedBarText,  0.73, -0.3 + m_input->GetForwardSpeed()/30, .04);

};