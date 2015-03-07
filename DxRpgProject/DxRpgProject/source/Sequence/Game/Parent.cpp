#include "Sequence/Game/Parent.h"
#include "Sequence/Game/Map.h"
#include "Sequence/Game/Battle.h"
#include "Map/MapMainCharacter.h"
#include "Map/MapCityCharacter.h"
#include "Sequence/Parent.h"

namespace Sequence
{
namespace Game
{
Parent::Parent()
    : nextSequence_(NextNone), child_(NULL), mapStage_(0)
{
    scs_ = new ::SharedCharacterStatus();
    initializeParameter();
    amc_[MainCharIndex] = new ::Map::MapMainCharacter();
    for (int n = MainCharMax; n < MainCharMax + CompCharMax; n++)
    {
        amc_[n] = new ::Map::MapCityCharacter();
    }
    child_ = new Map(this, mapStage_);
}


Parent::~Parent()
{
    for (int i = 0; i < MainCharMax + CompCharMax; i++)
    {
        if (amc_[i] != NULL)
        {
            delete amc_[i];
            amc_[i] = NULL;
        }
    }
    if (child_ != NULL)
    {
        delete child_;
    }
    delete scs_;
}

void Parent::update(GrandParent* grandParent)
{
    child_->update(this);
    switch (nextSequence_)
    {
    case NextMapMain:       
        if (child_ != NULL)
        {
            delete child_;
        }
        child_ = new Map(this, mapStage_);     
        break;
    case NextBattle:
        if (child_ != NULL)
        {
            delete child_;
        }
        child_ = new Battle(scs_);
        break;
    case NextRestart:
        grandParent->moveTo(GrandParent::NextMap);
        break;
    case NextNone:
        break;
    default:
        exit(EXIT_FAILURE);
        break;
    }
    nextSequence_ = NextNone;
}

void Parent::moveTo(NextSequence nextSequence)
{
    nextSequence_ = nextSequence;
}

::Map::AbstractMapCharacter *Parent::getCharacter(int index)
{
    return amc_[index];
}
void Parent::initializeParameter()
{
#if _DEBUG
    scs_->hp_ = 1352;
#else
    scs_->hp_ = 1352;
#endif
    scs_->hpMax_ = 1352;
    scs_->mp_ = 97;
    scs_->mpMax_ = 102;
    scs_->power_ = 20;
    scs_->defense_ = 20;
    scs_->strength_ = 20;
    scs_->magic_ =  20;
    scs_->weapon_ = 20;
    scs_->equip_ = 20;
    scs_->level_ = 10;
}
/*
int Parent::getMapStage() const
{
    return mapStage_;
}
*/
void Parent::setMapStage(int mapStage)
{
    mapStage_ = mapStage;
}

}
}
