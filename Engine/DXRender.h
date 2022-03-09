#pragma once

#include "FWinRender.h"
#include "FMathHelper.h"
#include "FUploadBuffer.h"
#include "WinBaseData.h"
#include "FCamera.h"
#include "FWinEventRegisterSystem.h"
#include "FScene.h"

struct FActorAsset
{
	std::shared_ptr<Charalotte::MeshGeometry> MeshAsset = nullptr;
	Charalotte::FTransform Transform;
	std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>> ObjectCB = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;

	glm::mat4 VPTrans = glm::mat4(1.0f);
	glm::mat4 MTrans = glm::mat4(1.0f);
};

class DXRender : public FWinRender
{
public:
	DXRender(HINSTANCE hInstance);
	// ban copy constructor and assignment
	DXRender(const DXRender& dm) = delete;
	DXRender operator= (const DXRender& dm) = delete;
	virtual ~DXRender();

	virtual bool Initialize()override;

	void LoadingMapDataFromAssetSystem(const std::string& MapName);

	void Destory(){};

private:
	virtual void OnResize()override;
	virtual void Update()override;
	virtual void Draw()override;
	
	// render pipeline
	void BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap);
	void BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
		std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>>& ObjectCb);
	void BuilMeshAsset(const std::string& MapName);
	void BuildActors(const std::string& MapName);
	void BuildRootSignature();
	void BuildShadersAndInputLayOut();
	void CalcVerticesAndIndices(const std::string& GeometryName = "", const Charalotte::FTransform& Transform = Charalotte::FTransform());
	void BuildMeshGeometrys();

	void BuildPSO();

private:

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	std::vector<std::shared_ptr<FActorAsset>> ActorArray;

	std::vector<std::shared_ptr<Charalotte::MeshGeometry>> MeshGeoArray;

	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

	POINT mLastMousePos;
	Charalotte::FActorsInfoForPrint ActorInfos;

};