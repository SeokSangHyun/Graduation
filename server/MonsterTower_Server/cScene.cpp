#include "stdafx.h"
#include "cScene.h"
#include "cZone.h"
#include "cObject.h"
#include "cSector.h"
#include "cMyNamespace.h"

cScene::cScene()
{
}

cScene::~cScene()
{
}

void cScene::UpdateZone(const UINT id)
{
	cObject* object = objects[id];
	if (object->GetScene() == STAGE_ONE)
		object->SetZone(&stageOne);
	if (object->GetScene() == STAGE_TWO)
		object->SetZone(&stageTwo);
	if (object->GetScene() == STAGE_THREE)
		object->SetZone(&stageThree);
	object->GetCurrZone()->UpdateSector(id);
}
