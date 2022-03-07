#include "stdafx.h"
#include "FWinGame.h"

void FWinGame::RegisterEvent()
{
	FWinInputSystem::GetInstance().RegisterKeyInput('A', [](const FGameTimer& gt) {
		float KeySe = 0.3f;
		auto Trans = FScene::GetInstance().GetCameraTrans();
		Trans.Translation.y -= KeySe;
		FScene::GetInstance().GetCamera()->TransformCamera(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinInputSystem::GetInstance().RegisterKeyInput('D', [](const FGameTimer& gt) {
		float KeySe = 0.3f;
		auto Trans = FScene::GetInstance().GetCameraTrans();
		Trans.Translation.y += KeySe;
		FScene::GetInstance().GetCamera()->TransformCamera(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinInputSystem::GetInstance().RegisterKeyInput('W', [](const FGameTimer& gt) {
		float KeySe = 0.3f;
		auto Trans = FScene::GetInstance().GetCameraTrans();
		Trans.Translation.x += KeySe;
		FScene::GetInstance().GetCamera()->TransformCamera(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinInputSystem::GetInstance().RegisterKeyInput('S', [](const FGameTimer& gt) {
		float KeySe = 0.3f;
		auto Trans = FScene::GetInstance().GetCameraTrans();
		Trans.Translation.x -= KeySe;
		FScene::GetInstance().GetCamera()->TransformCamera(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinInputSystem::GetInstance().RegisterKeyInput('Q', [](const FGameTimer& gt) {
		float KeySe = 0.3f;
		auto Trans = FScene::GetInstance().GetCameraTrans();
		Trans.Translation.z -= KeySe;
		FScene::GetInstance().GetCamera()->TransformCamera(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinInputSystem::GetInstance().RegisterKeyInput('E', [](const FGameTimer& gt) {
		float KeySe = 0.3f;
		auto Trans = FScene::GetInstance().GetCameraTrans();
		Trans.Translation.z += KeySe;
		FScene::GetInstance().GetCamera()->TransformCamera(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinInputSystem::GetInstance().RegisterKeyInput('O', [](const FGameTimer& gt) {
		glm::vec4 Location = glm::vec4(-5000.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 Target = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 Up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		FScene::GetInstance().GetCamera()->BackCameraLocation(Location, Target, Up);
		});
}