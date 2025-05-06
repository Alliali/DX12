#include "SceneFactory.h"
#include "TitleScene.h"
#include "Scene.h"

SceneManager* SceneFactory::CreateScene(SceneType type, CPlayer* pPlayer)
{
	switch (type)
	{
	case SceneType::Title:
		return new TitleScene(pPlayer);
	/*case SceneType::Menu:
		return new MenuScene;
	case SceneType::Tutorial:
		return new TutorialScene;
	case SceneType::Level_1:
		return new Level_1Scene;
	case SceneType::Level_2:
		return new Level_2Scene;*/
	case SceneType::Scene:
		return new CScene(pPlayer);
	/*case SceneType::Exit:
		return new ExitScene;*/
	default:
		return nullptr;
	}
}
