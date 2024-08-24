#pragma once
#include "stdafx.h" //따라하기와 다른점. CALLBACk 식별자가 인식이 안되서 미리 헤더에 추가함
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "UI.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;
	//DXGI 팩토리 인터페이스에 대한 포인터이다.

	IDXGISwapChain3* m_pdxgiSwapChain;
	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.

	ID3D12Device* m_pd3dDevice;
	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.

	bool m_bMsaa4xEnable = false;

	UINT m_nMsaa4xQualityLevels = 0;
	//MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	
	static const UINT m_nSwapChainBuffers = 2;
	//스왑 체인의 후면 버퍼의 개수이다.

	UINT m_nSwapChainBufferIndex;
	//현재 스왑 체인의 후면 버퍼 인덱스이다.

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.

	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.

	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.

	ID3D12PipelineState* m_pd3dPipelineState;
	//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.

	ID3D12Fence* m_pd3dFence;

	UINT64 m_nFenceValue[m_nSwapChainBuffers]; 
	HANDLE m_hFenceEvent;
	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.


	//다음은 게임 프레임워크에서 사용할 타이머이다.
	CGameTimer m_GameTimer;

	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR m_pszFrameRate[50];

private:
	// DX2D전용
#ifdef _WITH_DIRECT2D
	ID3D11On12Device* m_pd3d11On12Device = NULL;
	ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
	ID2D1Factory3* m_pd2dFactory = NULL;
	IDWriteFactory* m_pdWriteFactory = NULL;
	ID2D1Device2* m_pd2dDevice = NULL;
	ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

	ID3D11Resource* m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1* m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush* m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush* m_pd2dbrBorder = NULL;

	IDWriteTextFormat* m_pdw_Timer_Font = NULL;
	IDWriteTextFormat* m_pdw_Inventory_Font = NULL;

	IDWriteTextLayout* m_pdwTextLayout = NULL;
	ID2D1SolidColorBrush* m_pd2dbrText = NULL;
#endif


public:
	CScene* m_pScene;

	CCamera* pMainCamera = NULL;

	// 화면에 출력할 UI 리스트
	//std::vector<UICamera*> pUI_list;
	//int ui_num = 0;


	//플레이어 객체에 대한 포인터이다.
	CPlayer* m_pPlayer = NULL;

	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다.
	POINT m_ptOldCursorPos;

	bool power_charge = false;
	// 파워 게이지 값 저장변수
	int power_degree = 0;

	float random_time = -1;
	float sum_time = 0;

	// 턴 시간 제한
	float Limit_time = 0.0f;
	
	// 턴 넘김 시간 : 2초
	float Delay_time = 0.0f;

	bool Need_to_change_turn = false;

	CGameObject* m_pSelectedObject = NULL;

	bool Camera_First_Person_View = false;

public:

	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	//프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다).

	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다. 

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//렌더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다.

	void BuildObjects();
	void ReleaseObjects();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.

	//////////
	//핵심 부분
	//////////
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.

	void WaitForGpuComplete();
	//CPU와 GPU를 동기화하는 함수이다.

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.


	void ChangeSwapChainState();

	void MoveToNextFrame();

	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);

	void CreateDirect2DDevice();



};