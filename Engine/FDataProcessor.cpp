#include "stdafx.h"
#include "FDataProcessor.h"

using std::ifstream;
using std::ios;
using std::cout;
using std::string;

FDataProcessor::FDataProcessor()
{

}

FDataProcessor::~FDataProcessor()
{

}

void FDataProcessor::LoadMesh(const std::string& FileName, Charalotte::FMeshPrimitive& MeshInfo)
{
	Charalotte::FMeshPrimitive& TempMeshInfo = MeshInfo;
	ifstream file("BinaryMeshFiles/" + FileName, ios::in | ios::binary);
	if (!file)
	{	
		return;
	}
	int MeshNameLength;
	file.read((char*)&MeshNameLength, sizeof(int));

	string MeshName;
	MeshName.resize(MeshNameLength);
	file.read((char*)MeshName.data(), MeshNameLength);
	TempMeshInfo.MeshName = MeshName;

	int LODNum = 0;
	file.read((char*)&LODNum, sizeof(int));
	TempMeshInfo.LODNums = LODNum;

	int LODInfosSize = 0;
	file.read((char*)&LODInfosSize, sizeof(int));

	if (LODNum != LODInfosSize)
	{
		return;
	}

	// Due to the file format that we define,
	// we use this rule to get our file data
	for (int index = 0; index < LODInfosSize; index++)
	{
		Charalotte::FLODInfo lodinfo;
		file.read((char*)&(lodinfo.VerticesNum), sizeof(int));
		file.read((char*)&(lodinfo.TrianglesNum), sizeof(int));
		file.read((char*)&(lodinfo.IndicesNum), sizeof(int));

		int VerticesLocationnum = 0;
		file.read((char*)&VerticesLocationnum, sizeof(int));
		for (int VerIndex = 0; VerIndex < VerticesLocationnum; VerIndex++)
		{
			Charalotte::FVector VertexLocation;
			file.read((char*)&(VertexLocation.x), sizeof(float));
			file.read((char*)&(VertexLocation.y), sizeof(float));
			file.read((char*)&(VertexLocation.z), sizeof(float));
			lodinfo.VerticesLocation.push_back(VertexLocation);
		}
		int IndicesNum = 0;
		file.read((char*)&IndicesNum, sizeof(int));
		for (int InIndex = 0; InIndex < IndicesNum; InIndex++)
		{
			int VertexIndex;
			file.read((char*)&VertexIndex, sizeof(int));
			lodinfo.Indices.push_back(VertexIndex);
		}

		int NormalNum = 0;
		file.read((char*)&NormalNum, sizeof(int));
		for (int NoIndex = 0; NoIndex < NormalNum; NoIndex++)
		{
			Charalotte::FVector4 NormalVector;
			file.read((char*)&(NormalVector.x), sizeof(float));
			file.read((char*)&(NormalVector.y), sizeof(float));
			file.read((char*)&(NormalVector.z), sizeof(float));
			file.read((char*)&(NormalVector.w), sizeof(float));
			lodinfo.VerticesNormal.push_back(NormalVector);
		}
		int UVNums = 0;
		file.read((char*)&UVNums, sizeof(int));
		for (int Index = 0; Index < UVNums; Index++)
		{
			Charalotte::FVector2D TempUV;
			file.read((char*)&(TempUV.X), sizeof(float));
			file.read((char*)&(TempUV.Y), sizeof(float));
			lodinfo.UVs.push_back(TempUV);
		}

		TempMeshInfo.LodInfos.push_back(lodinfo);
		//	TempMeshInfo.LodInfos 
	}
}

void FDataProcessor::LoadActors(const std::string& FileName, Charalotte::FActorPrimitive& ActorInfos)
{
	Charalotte::FActorPrimitive& TempActorInfos = ActorInfos;

	int ActorsNum;
	ifstream file("BinaryActorFiles/" + FileName, ios::in | ios::binary);
	if (!file)
	{
		return;
	}
	file.read((char*)&ActorsNum, sizeof(int32_t));
	for (int i = 0; i < ActorsNum; i++)
	{
		Charalotte::FActorInfo ActorInfo;
		file.read((char*)&(ActorInfo.Transform.Rotation.X), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Rotation.Y), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Rotation.Z), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Rotation.W), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Translation.x), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Translation.y), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Translation.z), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Scale3D.x), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Scale3D.y), sizeof(float));
		file.read((char*)&(ActorInfo.Transform.Scale3D.z), sizeof(float));

		int AssetNameLength = 0;
		file.read((char*)&AssetNameLength, sizeof(int32_t));
		ActorInfo.MeshPrimitiveName.resize(AssetNameLength);
		file.read((char*)(ActorInfo.MeshPrimitiveName.data()), AssetNameLength);
		TempActorInfos.ActorsInfo.push_back(ActorInfo);
	}
}

