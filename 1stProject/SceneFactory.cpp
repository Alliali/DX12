#include "SceneFactory.h"
#include "TitleScene.h"
#include "MenuScene.h"
#include "Scene.h"

SceneManager* SceneFactory::CreateScene(SceneType type, CPlayer* pPlayer)
{
	switch (type)
	{
	case SceneType::Title:
		return new TitleScene(pPlayer);
	case SceneType::Menu:
		return new MenuScene(pPlayer);
	/*case SceneType::Tutorial:
		return new TutorialScene;
	case SceneType::Level_1:
		return new Level_1Scene;
	case SceneType::Level_2:
		return new Level_2Scene;*/
	case SceneType::Start:
		return new CScene(pPlayer);
	case SceneType::End:
		::PostQuitMessage(0);
	default:
		return nullptr;
	}
}
