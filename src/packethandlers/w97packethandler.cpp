#include "w97packethandler.hpp"
#include "packetforge.hpp"
#include "chatview.hpp"
#include "player.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "campexemption.hpp"
#include <QSharedPointer>
#include <QtCore>


W97PacketHandler::W97PacketHandler()
{

}

W97PacketHandler::~W97PacketHandler()
{

}

W97PacketHandler* W97PacketHandler::getInstance()
{
    static W97PacketHandler* instance;
    if ( instance == nullptr )
        instance = new W97PacketHandler();

    return instance;
}

bool W97PacketHandler::handlePacket(QSharedPointer<Server> server, ChatView* chatView, const QByteArray& packet, QSharedPointer<Player> plr)
{
    if ( server == nullptr )
        return false;

    if ( chatView == nullptr )
        return false;

    if ( plr == nullptr )
        return false;

    bool retn{ false };
    QString pkt{ packet };
    pkt = PacketForge::getInstance()->decryptPacket( packet );

    if ( !pkt.isEmpty() )
    {
        pkt = pkt.trimmed();

        //Warpath denotes Chat Packets with opCode 'D' at position '7'.
        if ( pkt.at( 7 ) == 'D' )
        {
            //Remove the checksum.
            pkt = pkt.left( pkt.length() - 2 );
            retn = chatView->parseChatEffect( pkt );

            plr->setIsAFK( false );
        }
        else if ( pkt.at( 7 ) == '4' )
        {
            QString plrName{ pkt.mid( 20 ) };
            plrName = plrName.left( plrName.length() - 2 );
            if ( !plrName.isEmpty() )
                plr->setPlrName( plrName );
        }
    }
    return retn;
}
