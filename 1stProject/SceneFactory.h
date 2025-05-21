#pragma once

#include "stdafx.h"


class SceneManager;
class CPlayer;

class SceneFactory
{
public:
	static SceneManager* CreateScene(SceneType type, CPlayer* pPlayer);
};

