#pragma once

#include "FWinsApp.h"
#include "FMathHelper.h"
#include "FUploadBuffer.h"
#include "BaseData.h"
#include "FCamera.h"

class FGameProcess : public FWinsApp
{
public:
	FGameProcess(HINSTANCE hInstance);
	// ban copy constructor and assignment
	FGameProcess(const FGameProcess& dm) = delete;
	FGameProcess operator= (const FGameProcess& dm) = delete;
	~FGameProcess();

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

	// Help function
	void ConstructProjectionMatrix();

	// render pipeline
	void BuildDescriptorHeaps();
	void BulidConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayOut();
	void CalcVerticesAndIndices(const std::string& GeometryName = "", const Charalotte::FTransform& Transform = Charalotte::FTransform());
	void BuildMeshGeometrys();
	void BuildEnviroument(const std::string& MapName = "");
	void BuildPSO();

	void TestFunc();

private:

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<Charalotte::ObjectConstants>> mObjectCB = nullptr;

	std::unique_ptr<MeshGeometry> mMeshGeo = nullptr;
	std::vector<MeshGeometry> MeshGeoArray;
	std::unordered_map<std::string, SubmeshGeometry> NameMeshDir;
	std::unordered_map<std::string, uint32_t> RepeatName;

	std::vector<DirectX::XMFLOAT4> TestColors;

	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

	DirectX::XMFLOAT4X4 mWorld = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mView = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = FMathHelper::Identity4x4();

	std::unique_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;
	// this num we can change
	//float mTheta = 1.5f * DirectX::XM_PI;	//??
	//float mPhi = DirectX::XM_PIDIV2;
	float mTheta = 1.5f * DirectX::XM_PI;	//??
	float mPhi = DirectX::XM_PIDIV2;
	//DirectX::XM_PIDIV4
	float mRadius = 50.0f;

	float mSensitivity = 0.25f;

	std::vector<Charalotte::Vertex> vertices;
	std::vector<int16_t> indices;
	POINT mLastMousePos;
	int ColorIndex;

	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> MeshInfoDir;
};