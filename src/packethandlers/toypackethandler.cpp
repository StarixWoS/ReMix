#include "toypackethandler.hpp"
#include "packetforge.hpp"
#include "chatview.hpp"
#include "player.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "campexemption.hpp"
#include <QSharedPointer>
#include <QtCore>


ToYPacketHandler::ToYPacketHandler()
{

}

ToYPacketHandler::~ToYPacketHandler()
{

}

ToYPacketHandler* ToYPacketHandler::getInstance()
{
    static ToYPacketHandler* instance;
    if ( instance == nullptr )
        instance = new ToYPacketHandler();

    return instance;
}

bool ToYPacketHandler::handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr)
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
        pkt = pkt.left( pkt.length() - 4 );
        switch ( pkt.at( 3 ).toLatin1() )
        {
            case 'B':
                {
                    emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                    emit this->insertChatMsgSignal( "*** ", Colors::SoulLeftWorld, false );
                    emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                    emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                    emit this->insertChatMsgSignal( "has left! ***", Colors::SoulLeftWorld, false );
                }
            break;
            case 'C':
                {
                    plr->setIsAFK( false );
                    retn = chatView->parseChatEffect( pkt );
                }
            break;
            case 'N':
                {
                    QStringList varList{ pkt.mid( 39 ).split( "," ) };
                    QString plrName{ varList.at( 0 ) };

                    if ( !plrName.isEmpty() )
                        plr->setPlrName( plrName );

                    //Check that the User is actually incarnating.
                    int type{ pkt.at( 14 ).toLatin1() - 0x41 };
                    if ( type >= 1 && !Helper::cmpStrings( plr->getPlrName(), "" ) )
                    {
                        bool isIncarnated{ plr->getIsIncarnated() };

                        plr->setIsGhosting( false );
                        plr->setIsIncarnated( true );
                        if ( Settings::getSetting( SKeys::Rules, SSubKeys::StrictRules, server->getServerName() ).toBool() )
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

                        if ( !isIncarnated )
                        {
                            emit this->insertChatMsgSignal( ChatView::getTimeStr(), Colors::TimeStamp, true );
                            emit this->insertChatMsgSignal( "*** ", Colors::SoulIncarnated, false );
                            emit this->insertChatMsgSignal( plr->getPlrName(), nameColor, false );
                            emit this->insertChatMsgSignal( " [ " % plr->getSernum_s() % " ] ", serNumColor, false );
                            emit this->insertChatMsgSignal( "has entered! ", Colors::SoulIncarnated, false );
                            emit this->insertChatMsgSignal( "***", Colors::SoulIncarnated, false );
                        }
                    }
                }
            break;
            case 'm':
                //Arcadia Moderator Connected.
                //Verify Information.
            break;

        }
    }
    return retn;
}
