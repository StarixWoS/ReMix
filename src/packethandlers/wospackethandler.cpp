#include "wospackethandler.hpp"
#include "packetforge.hpp"
#include "chatview.hpp"
#include "player.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "campexemption.hpp"
#include <QSharedPointer>
#include <QtCore>

WoSPacketHandler::~WoSPacketHandler()
{

}

WoSPacketHandler* WoSPacketHandler::getInstance()
{
    static WoSPacketHandler* instance;
    if ( instance == nullptr )
        instance = new WoSPacketHandler();

    return instance;
}

bool WoSPacketHandler::handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr)
{
    if ( server == nullptr )
        return false;

    if ( chatView == nullptr )
        return false;

    if ( plr == nullptr )
        return false;

    Colors serNumColor{ Colors::WhiteSoul };
    Colors nameColor{ Colors::PlayerName };
    if ( plr->getAdminRank() >= GMRanks::GMaster )
    {
        nameColor = Colors::AdminName;
        if ( plr->getAdminRank() > GMRanks::Admin )
            nameColor = Colors::OwnerName;
    }

    if ( plr->getIsGoldenSerNum() )
        serNumColor = Colors::GoldenSoul;

    QString pkt{ PacketForge::getInstance()->decryptPacket( packet ) };
    bool retn{ true };

    if ( !pkt.isEmpty() )
    {
        QString trgSerNum{ pkt.left( 21 ).mid( 13 ) };
        QSharedPointer<Player> targetPlayer{ server->getPlayer( trgSerNum ) };

        Colors targetNameColor{ Colors::PlayerName };
        Colors targetSerNumColor{ Colors::WhiteSoul };
        if ( targetPlayer != nullptr )
        {
            targetNameColor = Colors::AdminName;
            if ( targetPlayer->getAdminRank() > GMRanks::Admin )
                targetNameColor = Colors::OwnerName;

            if ( targetPlayer->getIsGoldenSerNum() )
                targetSerNumColor = Colors::GoldenSoul;
        }

        switch ( pkt.at( 3 ).toLatin1() )
        {
            case '3':
                {
                    QStringList varList{ pkt.mid( 47 ).split( "," ) };
                    QString plrName{ varList.at( 0 ) };

                    if ( !plrName.isEmpty() )
                        plr->setPlrName( plrName );

                    //Check that the User is actually incarnating.
                    int type{ pkt.at( 14 ).toLatin1() - 0x41 };
                    if ( type >= 1
                      && !Helper::cmpStrings( plr->getPlrName(), "" ) )
                    {
                        bool isIncarnated{ plr->getIsIncarnated() };
                        plr->setIsGhosting( false );

                        QString msg{ "" };
                        switch ( type )
                        {
                            case 1:
                                {
                                    plr->setIsIncarnated( true );
                                    msg = "has incarnated into this world! ";
                                }
                            break;
                            case 2:
                                {
                                    plr->setIsIncarnated( true );
                                    plr->setIsGhosting( true );
                                    msg = "walks the land as an apparition! ";
                                }
                            break;
                            case 4:
                                {
                                    if ( isIncarnated )
                                    {
                                        isIncarnated = false;
                                        plr->setIsIncarnated( false );
                                        msg = "has returned to the Well of Souls! ";
                                    }
                                }
                            break;
                            default:
                                plr->setIsIncarnated( false );
                            break;
                        }

                        //To Do: Enforce Game Version.
                        if ( plr->getIsIncarnated()
                          && Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
                        {
                            const QString minVersion{ Settings::getSetting( SKeys::Rules, SSubKeys::MinVersion, server->getServerName() ).toString() };
                            if ( !minVersion.isEmpty() )
                            {
                                const qint32 plrVersion{ Helper::strToInt( pkt.mid( 15 ).left( 8 ) ) };
                                if ( Helper::strToInt( minVersion ) != plrVersion )
                                {
                                    static const QString dcMsg{ "You have been disconnected due to the Rule \"minV\" being *Strictly Enforced*." };
                                    server->sendMasterMessage( dcMsg, plr, false );
                                    plr->setDisconnected( true, DCTypes::PktDC );
                                }
                            }
                        }

                        plr->setIsPK( Helper::strToInt( pkt.mid( 23 ).left( 8 ) ) <= 0 ? true : false );

                        if ( !msg.isEmpty()
                          && !isIncarnated )
                        {
                            emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                            emit this->insertChatMsgSignal( "*** ", Colors::SoulIncarnated, false );
                            emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                            emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                            emit this->insertChatMsgSignal( msg, Colors::SoulIncarnated, false );
                            emit this->insertChatMsgSignal( "***", Colors::SoulIncarnated, false );
                        }
                    }

                    if ( type >= 1 && type != 4 )
                    {
                        for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
                        {
                            QSharedPointer<Player> tmpPlr{ server->getPlayer( i ) };
                            if ( tmpPlr != nullptr
                              && plr != tmpPlr )
                            {
                                if ( tmpPlr->getIsVisible() ) //Do not force Invisible Users to send camp packets.
                                {
                                    if ( plr->getSceneHost() != tmpPlr->getSernum_i()
                                      || plr->getSceneHost() <= 0 )
                                    {
                                        if ( !tmpPlr->getCampPacket().isEmpty()
                                          && tmpPlr->getTargetType() == PktTarget::ALL )
                                        {
                                            tmpPlr->setTargetSerNum( plr->getSernum_i() );
                                            tmpPlr->setTargetType( PktTarget::PLAYER );
                                            tmpPlr->forceSendCampPacket();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            break;
            case '5': //Player Leaves Server. //Arcadia uses packet 'B'.
                {
                    emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                    emit this->insertChatMsgSignal( "*** ", Colors::SoulLeftWorld, false );
                    emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                    emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                    emit this->insertChatMsgSignal( "has left this world! ***", Colors::SoulLeftWorld, false );
                }
            break;
            case 'C':
                {
                    plr->setIsAFK( false );
                    retn = chatView->parseChatEffect( pkt );
                }
            break;
            case 'k': //PK Attack.
                {
                    //To Do: Enforce the "No Player Killing" rule.
                    if ( Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
                    {
                        static const QString dcMsg{ "You have been disconnected due to the Rule \"noPK\" being *Strictly Enforced*." };
                        server->sendMasterMessage( dcMsg, plr, false );
                        plr->setDisconnected( true, DCTypes::PktDC );
                    }

                    if ( targetPlayer != nullptr )
                    {
                        emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                        emit this->insertChatMsgSignal( "*** ", Colors::PKChallenge, false );
                        emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                        emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                        emit this->insertChatMsgSignal( "has challenged ", Colors::PKChallenge, false );
                        emit this->insertChatMsgSignal( targetPlayer->getPlrName(), targetNameColor, false );
                        emit this->insertChatMsgSignal( " [ " % targetPlayer->getSernum_s() % " ] ", targetSerNumColor, false );
                        emit this->insertChatMsgSignal( "to a PK fight! ***", Colors::PKChallenge, false );
                    }
                }
            break;
            case 'p':
                {
                    bool isJoining = true;

                    qint32 leader( Helper::strToInt( trgSerNum ) );
                    if ( leader == 0 )
                    {
                        leader = Helper::strToInt( packet.left( 29 ).mid( 21 ) );
                        if ( leader == 0 )
                            break;

                        isJoining = false;
                    }

                    QSharedPointer<Player> partyLeader{ server->getPlayer( Helper::intToStr( leader, IntBase::HEX, IntFills::DblWord ) ) };
                    if ( partyLeader != nullptr )
                    {
                        if ( isJoining
                          && Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
                        {
                            //The Target Party is locked.
                            if ( partyLeader->getIsPartyLocked() )
                            {
                                static const QString dcMsg{ "You have been disconnected due to violating a User's \"Locked Party\" policy." };
                                server->sendMasterMessage( dcMsg, plr, false );
                                plr->setDisconnected( true, DCTypes::PktDC );
                            }
                        }
                        else
                        {
                            if ( partyLeader->getAdminRank() >= GMRanks::GMaster )
                            {
                                targetNameColor = Colors::AdminName;
                                if ( partyLeader->getAdminRank() > GMRanks::Admin )
                                    targetNameColor = Colors::OwnerName;

                                if ( partyLeader->getIsGoldenSerNum() )
                                    targetSerNumColor = Colors::GoldenSoul;
                            }

                            emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                            emit this->insertChatMsgSignal( "*** ", Colors::PartyJoin, false );
                            emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                            emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                            emit this->insertChatMsgSignal( ( isJoining ? "joins " : "leaves " ), Colors::PartyJoin, false );

                            emit this->insertChatMsgSignal( partyLeader->getPlrName() % "'s", targetNameColor, false );
                            emit this->insertChatMsgSignal( " [ " % partyLeader->getSernum_s() % " ] ", targetSerNumColor, false );
                            emit this->insertChatMsgSignal( "party. ***", Colors::PartyJoin, false );
                        }
                    }
                }
            break;
            case 'F':
                {  //Save the User's camp packet. --Send to newly connecting Users.
                    if ( plr->getCampPacket().isEmpty() )
                    {
                        qint32 sceneID{ Helper::strToInt( pkt.left( 17 ).mid( 13 ) ) };
                        if ( sceneID >= 1 ) //If is 0 then it is the well scene and we can ignore the 'camp' packet.
                        {
                            plr->setCampPacket( packet );
                            plr->setCampCreatedTime( QDateTime::currentDateTime().toSecsSinceEpoch() );
                        }
                    }
                }
            break;
            case 'f':
                {  //User un-camp. Remove camp packet.
                    if ( !plr->getCampPacket().isEmpty() )
                    {
                        plr->setCampPacket( "" );
                        plr->setCampCreatedTime( 0 );
                    }
                }
            break;
            case 's': //Parse Player Level and AFK status.
                {
                    const qint32 status{ Helper::strToInt( pkt.mid( 89 ).left( 2 ) ) };

                    plr->setPlrCheatCount( Helper::strToInt( pkt.mid( 87 ).left( 2 ) ) );
                    plr->setPlrModCount( Helper::strToInt( pkt.mid( 69 ).left( 2 ) ) );
                    plr->setPlrLevel( Helper::strToInt( pkt.mid( 21 ).left( 4 ) ) );
                    plr->setIsPartyLocked( status & 2 );
                    plr->setIsAFK( status & 1 );
                }
            break;
            case 'K':  //If pet level exceess the Player's level then discard the packet.
                {
                    if ( Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
                    {
                        retn = false;   //Silently ignore the Pet Packet.
                        break;
                    }

                    static QString dcMsg{ "You may not call a pet stronger than yourself within a camp (scene) hosted by another Player!" };
                    qint32 petLevel{ Helper::strToInt( pkt.mid( 19 ).left( 4 ) ) };

                    if ( plr->getPlrLevel() >= 1
                      && petLevel >= plr->getPlrLevel() )
                    {
                        server->sendMasterMessage( dcMsg, plr, false );
                        retn = false;
                    }
                }
            break;
            case 'H':
                {
                    if ( plr->getIsIncarnated() )
                    {
                        if ( Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
                        {
                            const bool migration{ Settings::getSetting( SKeys::Rules, SSubKeys::NoMigrate, server->getServerName() ).toBool() };
                            if ( migration )
                            {
                                const qint32 birthSerNum{ Helper::strToInt( pkt.mid( 77 ) ) };
                                if ( plr->getSernum_i() != birthSerNum )
                                {
                                    static const QString dcMsg{ "You have been disconnected due to the Rule \"noMigrate\" being *Strictly Enforced*." };
                                    server->sendMasterMessage( dcMsg, plr, false );
                                    plr->setDisconnected( true, DCTypes::PktDC );
                                }
                            }
                        }
                    }
                }
            break;
            case 'J':
                {
                    QSharedPointer<Player> tmpPlr{ nullptr };
                    for ( int i = 0; i < server->getMaxPlayerCount(); ++i )
                    {
                        tmpPlr = server->getPlayer( i );
                        if ( tmpPlr != nullptr )
                        {
                            if ( Helper::cmpStrings( tmpPlr->getSernumHex_s(), trgSerNum ) )
                                break;
                        }
                        tmpPlr = nullptr;
                    }

                    if ( tmpPlr != nullptr )
                    {
                        QString message{ "The Camp Hosted by [ %1 ] is currently locked and you may not enter!" };
                        if ( !CampExemption::getInstance()->getIsWhitelisted( tmpPlr->getSernumHex_s(), plr->getSernumHex_s() ) )
                        {   //The Player is not exempted from further checking.
                            if ( tmpPlr->getIsCampLocked() )
                            {
                                retn = false;
                            }
                            else if ( tmpPlr->getIsCampOptOut() )
                            {
                                //The Camp was created before the Player connected. Mark it as old.
                                if (( tmpPlr->getCampCreatedTime() - plr->getPlrConnectedTime() ) < 0 )
                                {
                                    message = "The Camp Hosted by [ %1 ] is considered \"Old\" to your client and you can not enter!";
                                    retn = false;
                                }
                                else
                                    retn = true;
                            }
                        }

                        if ( !retn )
                        {
                            message = message.arg( tmpPlr->getPlrName() );
                            server->sendMasterMessage( message, plr, false );
                        }
                    }
                }
            break;
        }
    }
    return retn;
}
