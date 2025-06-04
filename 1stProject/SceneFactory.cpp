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
	case SceneType::Tutorial:
		return new MenuScene(pPlayer);
	case SceneType::Level_1:
		return new CScene(pPlayer);
	case SceneType::Level_2:
		return new CScene(pPlayer);
	case SceneType::Start:
		return new CScene(pPlayer);
	case SceneType::End:
		return nullptr;
	default:
		return nullptr;
	}
}
