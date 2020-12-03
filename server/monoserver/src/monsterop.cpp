/*
 * =====================================================================================
 *
 *       Filename: monsterop.cpp
 *        Created: 05/03/2016 21:49:38
 *    Description: 
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#include <algorithm>
#include "player.hpp"
#include "monster.hpp"
#include "sysconst.hpp"
#include "actorpod.hpp"
#include "mathf.hpp"
#include "monoserver.hpp"
#include "taodog.hpp"
#include "dbcomid.hpp"

extern MonoServer *g_monoServer;
void Monster::on_MPK_METRONOME(const MessagePack &)
{
    update();
}

void Monster::on_MPK_MISS(const MessagePack &rstMPK)
{
    AMMiss stAMM;
    std::memcpy(&stAMM, rstMPK.Data(), sizeof(stAMM));

    if(stAMM.UID != UID()){
        return;
    }

    foreachInViewCO([this, stAMM](const COLocation &rstLocation)
    {
        if(uidf::getUIDType(rstLocation.UID) == UID_PLY){
            m_actorPod->forward(rstLocation.UID, {MPK_MISS, stAMM});
        }
    });
}

void Monster::on_MPK_QUERYCORECORD(const MessagePack &rstMPK)
{
    AMQueryCORecord stAMQCOR;
    std::memcpy(&stAMQCOR, rstMPK.Data(), sizeof(stAMQCOR));

    reportCO(stAMQCOR.UID);
}

void Monster::on_MPK_EXP(const MessagePack &rstMPK)
{
    if(masterUID()){
        m_actorPod->forward(masterUID(), {rstMPK.Type(), rstMPK.Data(), rstMPK.DataLen()});
    }
}

void Monster::on_MPK_ACTION(const MessagePack &rstMPK)
{
    AMAction stAMA;
    std::memcpy(&stAMA, rstMPK.Data(), sizeof(stAMA));

    if(stAMA.UID == UID()){
        return;
    }

    if(stAMA.MapID != MapID()){
        RemoveTarget(stAMA.UID);
        RemoveInViewCO(stAMA.UID);
        return;
    }

    int nX   = -1;
    int nY   = -1;
    int nDir = -1;

    switch(stAMA.Action){
        case ACTION_STAND:
        case ACTION_ATTACK:
        case ACTION_HITTED:
            {
                nX   = stAMA.X;
                nY   = stAMA.Y;
                nDir = stAMA.Direction;
                break;
            }
        case ACTION_MOVE:
        case ACTION_SPELL:
        case ACTION_SPAWN:
            {
                nX = stAMA.X;
                nY = stAMA.Y;
                break;
            }
        case ACTION_DIE:
            {
                RemoveTarget(stAMA.UID);
                RemoveInViewCO(stAMA.UID);
                return;
            }
        default:
            {
                return;
            }
    }

    switch(stAMA.Action){
        case ACTION_SPAWN:
        case ACTION_SPACEMOVE2:
            {
                dispatchAction(stAMA.UID, ActionStand(X(), Y(), Direction()));
                break;
            }
        default:
            {
                break;
            }
    }

    AddInViewCO(stAMA.UID, stAMA.MapID, nX, nY, nDir);
}

void Monster::on_MPK_NOTIFYNEWCO(const MessagePack &rstMPK)
{
    const auto stAMNNCO = rstMPK.conv<AMNotifyNewCO>();
    switch(GetState(STATE_DEAD)){
        case 0:
            {
                // should make an valid action node and send it
                // currently just dispatch through map

                dispatchAction(ActionStand(X(), Y(), Direction()));
                break;
            }
        default:
            {
                AMNotifyDead stAMND;
                std::memset(&stAMND, 0, sizeof(stAMND));

                stAMND.UID = UID();
                m_actorPod->forward(stAMNNCO.UID, {MPK_NOTIFYDEAD, stAMND});
                break;
            }
    }
}

void Monster::on_MPK_ATTACK(const MessagePack &rstMPK)
{
    AMAttack stAMAK;
    std::memcpy(&stAMAK, rstMPK.Data(), sizeof(stAMAK));

    switch(GetState(STATE_DEAD)){
        case 0:
            {
                if(monsterID() == DBCOM_MONSTERID(u8"神兽")){
                    dynamic_cast<TaoDog *>(this)->setTransf(true);
                }

                if(mathf::LDistance2(X(), Y(), stAMAK.X, stAMAK.Y) > 2){
                    switch(uidf::getUIDType(stAMAK.UID)){
                        case UID_MON:
                        case UID_PLY:
                            {
                                AMMiss stAMM;
                                std::memset(&stAMM, 0, sizeof(stAMM));

                                stAMM.UID = stAMAK.UID;
                                m_actorPod->forward(stAMAK.UID, {MPK_MISS, stAMM});
                                return;
                            }
                        default:
                            {
                                return;
                            }
                    }
                }

                addOffenderDamage(stAMAK.UID, stAMAK.Damage);
                dispatchAction(ActionHitted(X(), Y(), Direction()));
                StruckDamage({stAMAK.UID, stAMAK.Type, stAMAK.Damage, stAMAK.Element, stAMAK.Effect});
                return;
            }
        default:
            {
                AMNotifyDead stAMND;
                std::memset(&stAMND, 0, sizeof(stAMND));

                stAMND.UID = UID();
                m_actorPod->forward(stAMAK.UID, {MPK_NOTIFYDEAD, stAMND});
                return;
            }
    }
}

void Monster::on_MPK_MAPSWITCH(const MessagePack &)
{
}

void Monster::on_MPK_QUERYLOCATION(const MessagePack &rstMPK)
{
    AMLocation stAML;
    std::memset(&stAML, 0, sizeof(stAML));

    stAML.UID       = UID();
    stAML.MapID     = MapID();
    stAML.X         = X();
    stAML.Y         = Y();
    stAML.Direction = Direction();

    m_actorPod->forward(rstMPK.from(), {MPK_LOCATION, stAML}, rstMPK.ID());
}

void Monster::on_MPK_UPDATEHP(const MessagePack &)
{
}

void Monster::on_MPK_BADACTORPOD(const MessagePack &)
{
}

void Monster::on_MPK_DEADFADEOUT(const MessagePack &rstMPK)
{
    AMDeadFadeOut stAMDFO;
    std::memcpy(&stAMDFO, rstMPK.Data(), sizeof(stAMDFO));

    RemoveTarget(stAMDFO.UID);
    RemoveInViewCO(stAMDFO.UID);
}

void Monster::on_MPK_NOTIFYDEAD(const MessagePack &rstMPK)
{
    AMNotifyDead stAMND;
    std::memcpy(&stAMND, rstMPK.Data(), sizeof(stAMND));

    RemoveTarget(stAMND.UID);
    RemoveInViewCO(stAMND.UID);
}

void Monster::on_MPK_OFFLINE(const MessagePack &rstMPK)
{
    AMOffline stAMO;
    std::memcpy(&stAMO, rstMPK.Data(), sizeof(stAMO));

    if(true
            && stAMO.UID
            && stAMO.UID == masterUID()){
        goDie();
    }
}

void Monster::on_MPK_CHECKMASTER(const MessagePack &rstMPK)
{
    m_actorPod->forward(rstMPK.from(), MPK_OK, rstMPK.ID());
}

void Monster::on_MPK_QUERYMASTER(const MessagePack &rstMPK)
{
    AMUID stAMUID;
    std::memset(&stAMUID, 0, sizeof(stAMUID));

    stAMUID.UID = masterUID() ? masterUID() : UID();
    m_actorPod->forward(rstMPK.from(), {MPK_UID, stAMUID}, rstMPK.ID());
}

void Monster::on_MPK_QUERYFINALMASTER(const MessagePack &rstMPK)
{
    QueryFinalMaster(UID(), [this, rstMPK](uint64_t nFMasterUID)
    {
        AMUID stAMUID;
        std::memset(&stAMUID, 0, sizeof(stAMUID));

        stAMUID.UID = nFMasterUID;
        m_actorPod->forward(rstMPK.from(), {MPK_UID, stAMUID}, rstMPK.ID());
    });
}

void Monster::on_MPK_QUERYFRIENDTYPE(const MessagePack &rstMPK)
{
    AMQueryFriendType stAMQFT;
    std::memcpy(&stAMQFT, rstMPK.Data(), sizeof(stAMQFT));

    checkFriend(stAMQFT.UID, [this, rstMPK](int nFriendType)
    {
        AMFriendType stAMFT;
        std::memset(&stAMFT, 0, sizeof(stAMFT));

        stAMFT.Type = nFriendType;
        m_actorPod->forward(rstMPK.from(), {MPK_FRIENDTYPE, stAMFT}, rstMPK.ID());
    });
}

void Monster::on_MPK_QUERYNAMECOLOR(const MessagePack &rstMPK)
{
    AMNameColor stAMNC;
    std::memset(&stAMNC, 0, sizeof(stAMNC));

    stAMNC.Color = 'W';
    m_actorPod->forward(rstMPK.from(), {MPK_NAMECOLOR, stAMNC}, rstMPK.ID());
}

void Monster::on_MPK_MASTERKILL(const MessagePack &rstMPK)
{
    if(masterUID() && (rstMPK.from() == masterUID())){
        goDie();
    }
}

void Monster::on_MPK_MASTERHITTED(const MessagePack &rstMPK)
{
    if(masterUID() && (rstMPK.from() == masterUID())){
        if(checkMonsterName(u8"神兽")){
            dynamic_cast<TaoDog *>(this)->setTransf(true);
            return;
        }

        if(checkMonsterName(u8"变异骷髅")){
            return;
        }
    }
}
