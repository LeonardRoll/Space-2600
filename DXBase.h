#pragma once

/////////////////
//  #Includes  //
/////////////////

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <dinput.h> // <-----| dinput? 
#include <ctime>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

/*Connecting the other classes*/
#include "camera.h" 
#include "Text2D.h" 
#include "model.h" 
#include "scene_node.h"
#include "Input.h"
#include "Player.h"

////////////////////
//  DXBase Class  //
////////////////////

class DXBase
{
	protected:
		
		/*Direct3D 11 variables*/
		D3D_DRIVER_TYPE            m_driverType;
		D3D_FEATURE_LEVEL          m_featureLevel;
		ID3D11Device*              m_pD3DDevice;
		ID3D11DeviceContext*       m_pImmediateContext;
		IDXGISwapChain*            m_p_SwapChain;
		ID3D11RenderTargetView*    m_p_BackBufferRTView; 
		ID3D11Buffer*			   m_pVertexBuffer;              
		ID3D11Buffer*			   m_pConstantBuffer0; 
		ID3D11VertexShader*		   m_pVertexShader;        
		ID3D11PixelShader*		   m_pPixelShader;        
		ID3D11InputLayout*		   m_pInputLayout;   
		ID3D11DepthStencilView*    m_pZBuffer;
		ID3D11SamplerState*		   m_pSampler0; 
		ID3D11BlendState*          m_pAlphaBlendEnable;
		ID3D11BlendState*		   m_pAlphaBlendDisable;

		//Textures
		ID3D11ShaderResourceView* m_p_MineTexture_Seeking;
		ID3D11ShaderResourceView* m_p_MineTexture_Active;
		ID3D11ShaderResourceView*  m_p_MoonTexture; 
		ID3D11ShaderResourceView* m_p_TokenTexture;
		ID3D11ShaderResourceView* m_p_UFOTexture;
		ID3D11ShaderResourceView* m_Star_Plane_Texture_XPos;
		ID3D11ShaderResourceView* m_Star_Plane_Texture_YPos;
		ID3D11ShaderResourceView* m_Star_Plane_Texture_ZPos;
		ID3D11ShaderResourceView* m_Star_Plane_Texture_XNeg;
		ID3D11ShaderResourceView* m_Star_Plane_Texture_YNeg;
		ID3D11ShaderResourceView* m_Star_Plane_Texture_ZNeg;
		vector<ID3D11ShaderResourceView*> m_PlanetTextures;

		/*Class objects*/
		Camera* m_p_Camera;
		Text2D* m_HUD;
		Text2D* m_HUD2;
		Text2D* m_HUD3;
		Text2D* m_HUDFHit;
		Text2D* m_HUDBHit;
		Text2D* m_HUDRHit;
		Text2D* m_HUDLHit;
		Text2D* m_HUDAboHit;
		Text2D* m_HUDBelHit;
		Text2D* m_HUDMoveF;
		Text2D* m_HUDMoveB;
		Text2D* m_HUDMoveR;
		Text2D* m_HUDMoveL;
		Text2D* m_HUDSpeedBar;

		Model*  m_Moon;
		Model* m_Mines;
		Model* m_Tokens;
		Model* m_UFO;
		vector <Model*> m_Star_Plane;
		vector <Model*> m_Planets;

		/*Input Device*/
		Input* m_input;

		/*Scene node control variables*/
		scene_node* m_RootNode;
		scene_node* m_PlanetNode;
		scene_node* m_Mines_Source;
		scene_node* m_Tokens_Source;
		scene_node* m_Plane_Source;
		scene_node* m_UFO_node;
		vector <scene_node*> m_planet_Plane_nodes;
		vector <scene_node*> m_planets;
		vector <scene_node*> m_p_Moons;
		vector <scene_node*> m_Mines_Nodes;
		vector <scene_node*> m_Token_Nodes;

		/*Instance controlling variables*/
		HINSTANCE	m_HInst;
		HWND		m_HWnd;
		
		/*Screen size variables*/
		float m_ScrWidth;
		float m_ScrHeight;

		/*Time control for text on screen*/
		int m_FHitCounter;
		int m_BHitCounter;
		int m_RHitCounter;
		int m_LHitCounter;
		int m_AboHitCounter;
		int m_BelHitCounter;
		string m_SpeedBarText;

		int RanFar;
		int cycle_count;
		bool gotHit;
		Player* m_player;

		XMVECTOR m_directional_light_shines_from;
		XMVECTOR m_directional_light_colour;
		XMVECTOR m_ambient_light_colour;

	private:
		
		/* Constant buffer structures (packs of 16)*/
		struct CONSTANT_BUFFER0 
		{
			XMMATRIX WorldViewProjection ; // 64 bytes 
			XMVECTOR m_directional_light_shines_from;
			XMVECTOR m_directional_light_colour;
			XMVECTOR m_ambient_light_colour;
		}; // TOTAL SIZE = 144 bytes

		public:
			
			DXBase(HINSTANCE g_HInst,HWND g_HWnd,float width,float height);
			Camera* getCamera();
			void setInput(Input* input);
			void ShutdownD3D();
			float RandFloatX(int max,int min);
			float RandFloatY(int max,int min);
			float RandFloatZ(int max,int min);
			float RandScale(int max,int min);
			HRESULT InitialiseD3D();
			HRESULT InitialiseGraphics(void);
			void RenderFrame(void);
			void SetHUD(void);
	};