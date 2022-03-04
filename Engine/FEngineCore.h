#pragma once

#include "FWinsApp.h"
#include "FMathHelper.h"
#include "FUploadBuffer.h"
#include "BaseData.h"
#include "FCamera.h"

struct FActorAsset
{
	std::shared_ptr<MeshGeometry> MeshAsset = nullptr;
	Charalotte::FTransform Transform;
	std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>> ObjectCB = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;

	glm::mat4 VPTrans = glm::mat4(1.0f);
	glm::mat4 MTrans = glm::mat4(1.0f);
};

class FEngineCore : public FWinsApp
{
public:
	FEngineCore(HINSTANCE hInstance);
	// ban copy constructor and assignment
	FEngineCore(const FEngineCore& dm) = delete;
	FEngineCore operator= (const FEngineCore& dm) = delete;
	~FEngineCore();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const FGameTimer& gt)override;
	virtual void Draw(const FGameTimer& gt)override;

	// transform
	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;
	virtual void OnKeyBoardInput(const FGameTimer& gt)override;

	// render pipeline
	void BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap);
	void BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
		std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>>& ObjectCb);
	void LoadMeshs(const std::string& GeometryName);
	void LoadActors(const Charalotte::FActorsInfoForPrint& Actor);
	void BuildRootSignature();
	void BuildShadersAndInputLayOut();
	void CalcVerticesAndIndices(const std::string& GeometryName = "", const Charalotte::FTransform& Transform = Charalotte::FTransform());
	void BuildMeshGeometrys();
	void BuildEnviroument(const std::string& MapName = "");
	void BuildPSO();

private:

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	std::vector<std::shared_ptr<FActorAsset>> ActorArray;

	std::vector<std::shared_ptr<MeshGeometry>> MeshGeoArray;

	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

	// camera data
	std::unique_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;

	POINT mLastMousePos;
	Charalotte::FActorsInfoForPrint ActorInfos;

	// store mesh data to prevent repeating read the same file
	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> MeshInfoDir;
};