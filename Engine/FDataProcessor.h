#pragma once
#include "WinBaseData.h"

class FDataProcessor
{
public:
	FDataProcessor();

	FDataProcessor(const FDataProcessor& datahandler) = delete;
	FDataProcessor& operator=(const FDataProcessor& datahandler) = delete;
	virtual ~FDataProcessor();

	static void LoadMesh(const std::string& FileName, Charalotte::FMeshInfoForPrint& MeshInfo);

	static void LoadActors(const std::string& FileName, Charalotte::FActorsInfoForPrint& ActorInfos);
};

