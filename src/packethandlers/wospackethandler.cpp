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

QHash<QSharedPointer<Server>, WoSPacketHandler*> WoSPacketHandler::handlerInstanceMap;

WoSPacketHandler::WoSPacketHandler(QSharedPointer<Server> server)
{
    QObject::connect( this, &WoSPacketHandler::insertChatMsgSignal, ChatView::getInstance( server ), &ChatView::insertChatMsgSlot, Qt::UniqueConnection );
}

WoSPacketHandler::~WoSPacketHandler()
{
    this->disconnect();
}

WoSPacketHandler* WoSPacketHandler::getInstance(QSharedPointer<Server> server)
{
    WoSPacketHandler* instance{ handlerInstanceMap.value( server, nullptr ) };
    if ( instance == nullptr )
    {
        instance = new WoSPacketHandler( server );
        if ( instance != nullptr )
            handlerInstanceMap.insert( server, instance );
    }
    return instance;
}

void WoSPacketHandler::deleteInstance(QSharedPointer<Server> server)
{
    WoSPacketHandler* instance{ handlerInstanceMap.take( server ) };
    if ( instance != nullptr )
    {
        instance->disconnect();
        instance->setParent( nullptr );
        instance->deleteLater();
    }
}

bool WoSPacketHandler::handlePacket(QSharedPointer<Server> server, ChatView* chatView, QByteArray& packet, QSharedPointer<Player> plr)
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
    bool emulatePlayer{ Settings::getSetting( SKeys::Setting, SSubKeys::PlayerEmulation ).toBool() };
    bool retn{ true };

    if ( !pkt.isEmpty() )
    {
        QString trgSerNum{ pkt.left( 21 ).mid( 13 ) };
        qint32 trgSerNumInt{ Helper::strToInt( trgSerNum, IntBase::HEX ) };
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
            case *WoSPacketTypes::WorldGetOut: //We're going to be using this as a Player PING!
                {
                    plr->setPlrPingResponseTime( QDateTime::currentMSecsSinceEpoch() );
                }
            break;
            case *WoSPacketTypes::SceneTossedItem:  //Details for these two packets don't matter. Only their existence.
            case *WoSPacketTypes::SceneAttackAction:
                {
                    retn = false;
                    if ( plr->getSceneCursorState() )
                    {
                        plr->setSceneCursorState( false );
                        retn = true;
                    }
                    else
                    {
                        const QString infCursor{ "Infinite Cursor Exploit Detected, Ignoring Action. User [ %1 : %2 ]." };
                        server->sendMasterMessage( infCursor.arg( plr->getSernum_s(), plr->getPlrName() ), plr, true );
                    }
                }
            break;
            case *WoSPacketTypes::ScenePromptAction:
                {
                    if ( pkt.at( 13 ).toLatin1() == 'A' )
                    {
                        QSharedPointer<Player> tmpPlr{ nullptr };
                        for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
                        {
                            if ( tmpPlayer != nullptr )
                            {
                                tmpPlr = tmpPlayer;
                                if ( plr->getSernum_i() == tmpPlr->getSceneHost() )
                                {
                                    tmpPlr->setSceneCursorState( true );
                                    //qDebug() << "Scene" << plr << "giving cursor to" << tmpPlr;
                                }
                            }
                            tmpPlr = nullptr;
                        }
                    }
                    retn = true;
                }
            break;
            case *WoSPacketTypes::Incarnation:
                {
                    QStringList varList{ pkt.mid( 47 ).split( "," ) };
                    QString plrName{ varList.at( 0 ) };

                    if ( !plrName.isEmpty() )
                    {
                        if ( emulatePlayer )
                        {
                            //Prevent Users from impersonating the Owner Character.
                            if ( Helper::cmpStrings( plrName, "Owner" ) )
                            {
                                pkt = pkt.replace( "Owner", "Owner Impersonator" );
                                plrName = "Owner Impersonator";
                                packet = PacketForge::getInstance()->encryptPacket( pkt.toLatin1(), plr->getPktHeaderSlot(), Games::WoS );
                            }
                        }
                        plr->setPlrName( plrName );
                    }

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
                            case *IncarnationType::Incarnation:
                                {
                                    plr->setIsIncarnated( true );
                                    msg = "has incarnated into this world! ";
                                }
                            break;
                            case *IncarnationType::GhostIncarnation:
                                {
                                    plr->setIsIncarnated( true );
                                    plr->setIsGhosting( true );
                                    msg = "walks the land as an apparition! ";
                                }
                            break;
                            case *IncarnationType::DisIncarnation:
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
                                    static const QString dcMsg{ "You have been disconnected due to the Rule \"minV=%1\" being *Strictly Enforced*." };
                                    server->sendMasterMessage( dcMsg.arg( minVersion ), plr, false );
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
                        for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
                        {
                            if ( tmpPlayer != nullptr
                              && plr != tmpPlayer )
                            {
                                if ( tmpPlayer->getIsVisible()
                                  && !tmpPlayer->getIsMuted() ) //Do not force Invisible, or Muted Users to send camp packets.
                                {
                                    if ( plr->getSernum_i() != tmpPlayer->getSceneHost()
                                      || plr->getSceneHost() <= 0 )
                                    {
                                        if ( !tmpPlayer->getCampPacket().isEmpty()
                                          && tmpPlayer->getTargetType() == PktTarget::ALL )
                                        {
                                            tmpPlayer->setTargetSerNum( plr->getSernum_i() );
                                            tmpPlayer->setTargetType( PktTarget::PLAYER );
                                            tmpPlayer->forceSendCampPacket();
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if ( emulatePlayer )
                    {
                        this->forgePacket( server, plr, WoSPacketTypes::Incarnation );
                        this->forgePacket( server, plr, WoSPacketTypes::GuildInfo );
                        this->forgePacket( server, plr, WoSPacketTypes::CharacterInfo );
                    }

                    if ( plr->getIsIncarnated() )
                        server->sendPingToPlayer( plr );

                    retn = true;
                }
            break;
            case *WoSPacketTypes::ServerLeave: //Player Leaves Server. //Arcadia uses packet 'B'.
                {
                    emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                    emit this->insertChatMsgSignal( "*** ", Colors::SoulLeftWorld, false );
                    emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                    emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                    emit this->insertChatMsgSignal( "has left this world! ***", Colors::SoulLeftWorld, false );
                }
            break;
            case *WoSPacketTypes::Chat:
                {
                    plr->setIsAFK( false );
                    retn = chatView->parseChatEffect( pkt );
                }
            break;
            case *WoSPacketTypes::PKAttack: //PK Attack.
                {
                    //To Do: Enforce the "No Player Killing" rule.
                    if ( Settings::getSetting( SKeys::Rules, SSubKeys::NoPK, server->getServerName() ).toBool()
                      && Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
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
            case *WoSPacketTypes::PartyState:
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
            case *WoSPacketTypes::PlayerCamp:
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
            case *WoSPacketTypes::PlayerUnCamp:
                {  //User un-camp. Remove camp packet.
                    if ( !plr->getCampPacket().isEmpty() )
                    {
                        plr->setCampPacket( "" );
                        plr->setCampCreatedTime( 0 );
                        //plr->removeSceneHosted( nullptr, true );
                    }
                }
            break;
            case *WoSPacketTypes::PlayerStatus: //Parse Player Level and AFK status.
                {
                    /* //Example of a Level 999 hacker. 0x3E7 = 999
                     * 087210.406: :;os00000FA0D0000000003E703E803020967096707A207A200C0AB9CFFFFFFFFFF140100000000000000000000
                     *                 14000000000000000C35000000314838F442570A356102
                     * 087210.437: :;os00000FA0D0000000003E703E803020967096707A207A200C0AB9CFFFFFFFFFF140100000000000000000000
                     *                 14000000000000000C35000000314838F442570A356102
                    */
                    const qint32 status{ Helper::strToInt( pkt.mid( 89 ).left( 2 ) ) };

                    plr->setPlrCheatCount( Helper::strToInt( pkt.mid( 87 ).left( 2 ) ) );
                    plr->setPlrModCount( Helper::strToInt( pkt.mid( 69 ).left( 2 ) ) );
                    plr->setPlrLevel( Helper::strToInt( pkt.mid( 21 ).left( 4 ) ) );
                    plr->setIsPartyLocked( status & 2 );
                    plr->setIsAFK( status & 1 );
                }
            break;
            case *WoSPacketTypes::SkinTransfer: //Skin Transfers.
                {
                    targetPlayer = server->getPlayer( pkt.mid( 15 ).left( 8 ) );

                    QString logMessage{ "" };
                    QString skinName{ "" };

                    qint32 transferType{ Helper::strToInt( pkt.left( 15 ).mid( 13 ), IntBase::HEX ) };

                    bool toLog{ true };

                    switch ( transferType )
                    {
                        case *SkinTransferType::RequestModeOne: //Mode One is never used. Perhaps from an older WoS version?
                        case *SkinTransferType::RequestModeTwo: //Mode Two is the standard Skin Request.
                            {
                                bool isSkinRequest{ false };
                                if ( pkt.contains( ".bmp," ) ) //Contains IP Information.
                                {
                                    QString forgedPacket{ pkt.left( pkt.indexOf( "," ) ) };
                                            forgedPacket = PacketForge::getInstance()->encryptPacket( forgedPacket.toLatin1(),
                                                                                                      plr->getPktHeaderSlot(), server->getGameId() );
                                    skinName = pkt.left( pkt.indexOf( "," ) ).mid( 39 );

                                    if ( plr->getIsVisible()
                                      && !plr->getIsMuted() )
                                    {
                                        if ( !forgedPacket.isEmpty() )
                                            forgedPacket.append( "\n" );

                                        if ( plr->getSvrPwdReceived()
                                          || !plr->getSvrPwdRequested() )
                                        {
                                            emit this->sendPacketToPlayerSignal( plr, *plr->getTargetType(), plr->getTargetSerNum(),
                                                                                 plr->getTargetScene(), forgedPacket.toLatin1() );

                                            //Reset the User's target information.
                                            plr->setTargetType( PktTarget::ALL );
                                            plr->setTargetSerNum( 0 );
                                            plr->setTargetScene( 0 );
                                        }
                                    }
                                    retn = false; //The original packet will not be processed.
                                    isSkinRequest = true;
                                }
                                else //ReMix will forward the packet as normal, as we only wish to modify packets with IP data.
                                    retn = true;

                                if ( isSkinRequest )
                                    logMessage = "User [ %1 ] is requesting the skin [ %2 ] from User [ %3 ].";
                            }
                        break;
                        case *SkinTransferType::Offer:           //User is Offering another User a Skin.
                            {
                                skinName = pkt.left( pkt.indexOf( "," ) ).mid( 39 );
                                logMessage = "User [ %1 ] is offering, or accepting a transfer request for the Skin [ %2 ]; To User [ %3 ].";

                                retn = true;
                            }
                        break;
                        case *SkinTransferType::ThanksForSkin:   //User is saying thanks for a skin.
                        case *SkinTransferType::DataRequest:     //Data Transfer, Request Data from Position.
                        case *SkinTransferType::DataTransfer:    //Data Transfer.
                        default:
                            toLog = false;
                            retn = true;
                        break;
                    }

                    if ( toLog )
                    {
                        if ( targetPlayer != nullptr )
                        {
                            logMessage = logMessage.arg( plr->getSernum_s() )
                                                   .arg( skinName )
                                                   .arg( targetPlayer->getSernum_s() );
                        }
                    }
                }
            break;
            case *WoSPacketTypes::PetCall:  //If pet level exceess the Player's level then discard the packet.
                {
                    if ( Settings::getSetting( SKeys::Rules, SSubKeys::NoPets, server->getServerName() ).toBool()
                      && Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
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
            case *WoSPacketTypes::CharacterInfo:
                {
                    if ( plr->getIsIncarnated() )
                    {
                        if ( Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool()
                          && Settings::getSetting( SKeys::Rules, SSubKeys::NoMigrate, server->getServerName() ).toBool() )
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
            break;
            case *WoSPacketTypes::CampJoin:
                {
                    /*
                     * //Examples of a level 999 hacker. 0x3E7 = 999
                     * 086667.921: :;oJ00000FA0D458C4C5A03E7000000002C912C911096109600C0AD8A060D0206030A005A000000000000000000000000000021D9
                     * 086898.265: :;oJ00000FA0D458C4C5A03E703E803020967096707A207A200C0AB9CFFFFFFFFFF140014000000F800B500BE00BF00C003E55B9B
                     *
                     * //Normal User.
                     * 086667.921: :;oJ00000FA0D458C4C5A006403E8030C0DDD0DDD0505050500C97F05FFFFFFFFFF05BA5F00BA00F800B500BE00BF00C003E52142
                    */
                    plr->setPlrLevel( Helper::strToInt( pkt.mid( 21 ).left( 4 ) ) );

                    QSharedPointer<Player> tmpPlr{ nullptr };
                    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
                    {
                        if ( tmpPlayer != nullptr )
                        {
                            tmpPlr = tmpPlayer;
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
                        //else //Player added to the scene.
                        //    tmpPlr->addSceneHosted( plr );
                    }
                }
            break;
//            case *WoSPacketTypes::CampLeave:
//                {
//                    QSharedPointer<Player> tmpPlr{ nullptr };
//                    for ( QSharedPointer<Player> tmpPlayer : server->getPlayerVector() )
//                    {
//                        if ( tmpPlayer != nullptr )
//                        {
//                            tmpPlr = tmpPlayer;
//                            if ( Helper::cmpStrings( tmpPlr->getSernumHex_s(), trgSerNum ) )
//                                break;
//                        }
//                        tmpPlr = nullptr;
//                    }

//                    if ( tmpPlr != nullptr )
//                        tmpPlr->removeSceneHosted( plr );
//                }
//            break;
            case *WoSPacketTypes::InfoRequest:
                {
                    if ( trgSerNumInt == static_cast<qint32>( ReMixSerNum::SerNum )
                      && emulatePlayer )
                    {
                        this->forgePacket( server, plr, WoSPacketTypes::Incarnation );
                        this->forgePacket( server, plr, WoSPacketTypes::GuildInfo );
                        this->forgePacket( server, plr, WoSPacketTypes::CharacterInfo );
                    }
                }
            break;
        }
    }
    return retn;
}

void WoSPacketHandler::forgePacket(QSharedPointer<Server> server, QSharedPointer<Player> plr, const WoSPacketTypes& type)
{
    if ( server == nullptr )
        return;

    if ( plr == nullptr )
        return;

    QString packet{ "" };
    static QString serverSernum{ Helper::serNumToHexStr( Helper::intToStr( *ReMixSerNum::SerNum, IntBase::HEX, IntFills::DblWord ) ) };
    switch ( type )
    {
        case WoSPacketTypes::WorldHello: //We're going to be using this as a Player PING!
            {

            }
        break;
        case WoSPacketTypes::Incarnation:
            {
                packet = ":;o3" % serverSernum % "DEB00000A97"
                       % Helper::intToStr( server->getUpTime() / 60, IntBase::HEX, IntFills::DblWord )
                       % "0000000000000000Owner,Adventurer"
                       % "," + server->getGameWorld() % ","
                       % Helper::intToStr( static_cast<int>( std::time( nullptr ) ), IntBase::HEX, IntFills::DblWord );
            }
        break;
        case WoSPacketTypes::CharacterInfo:
            {
                packet = ":;oH" % serverSernum % "D" % "0000000000000000000000000000000000000000071DFC29" % "0000000000000000"
                       % serverSernum;
            }
        break;
        case WoSPacketTypes::GuildInfo:
            {
                packet = ":;oU" + serverSernum + "D000000010000000000000000000000000000000000000000ReMix,(null),(null)";
            }
        break;
        default:
        break;
    }

    if ( !packet.isEmpty() )
    {
        packet = PacketForge::getInstance()->encryptPacket( packet.toLatin1(), 0, server->getGameId() );
        emit this->sendPacketToPlayerSignal( nullptr, *PktTarget::PLAYER, plr->getSernum_i(), 0, packet.toLatin1() );

        server->startMasterSerNumKeepAliveTimer();
    }
}
