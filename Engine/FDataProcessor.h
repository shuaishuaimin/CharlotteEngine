#pragma once
#include "DXPrimitives.h"

class FDataProcessor
{
public:
	FDataProcessor();

	FDataProcessor(const FDataProcessor& datahandler) = delete;
	FDataProcessor& operator=(const FDataProcessor& datahandler) = delete;
	virtual ~FDataProcessor();

	static void LoadMesh(const std::string& FileName, Charalotte::FMeshPrimitive& MeshInfo);

	static void LoadActors(const std::string& FileName, Charalotte::FActorPrimitive& ActorInfos);
};

