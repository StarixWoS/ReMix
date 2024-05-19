#include "wospacketforge.hpp"
#include "packetforge.hpp"
#include "chatview.hpp"
#include "player.hpp"
#include "server.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "campexemption.hpp"
#include <QSharedPointer>
#include <QtCore>

WoSPacketForge::WoSPacketForge()
{

}

void WoSPacketForge::sendMixPacket(QString opCode, QString serNum, QString msg)
{
    if ( msg.contains( "/cmd", Qt::CaseInsensitive ) )
        opCode = "6";

    QString packet = ":MIX" + opCode + "00" + serNum + msg;
    tcpSocket->sendPacket( packet );
}

void WoSPacketForge::sendPacketPrep(char opCode, QString srcSerNum, QString trgSerNum,
                                   QString message, QString subCode)
{
    QString packet;
    QString tempVar;

    //int trgPosition = getSerNumPosition( trgSerNum );
    switch( opCode )
    {
        case '0':
            packet = ":;o0" + srcSerNum + "D";
        break;
        case '1':
            this->sendMixPacket( "3", srcSerNum, "" );
            packet = ":;o1" + srcSerNum + "B";
        break;
        case '2':    //Perhaps we could also use this as a ban mechanism.
            packet = ":;o2" + srcSerNum + "D";
        break;
        case '3':
            tempVar = Helper::intToStr( serverSel->getSoulHours(), IntBase::HEX, IntFills::DblWord );
            if ( us->getIsPKer() )
            {
                tempVar = Helper::intToStr( -serverSel->getSoulHours(), IntBase::HEX, IntFills::DblWord, QChar( 'F' ) );
                if ( tempVar.length() > 8 )
                    tempVar = tempVar.mid( tempVar.length() - 8 );
            }

            packet = ":;o3" + srcSerNum + "D" + us->classID + incarnationType + "00000A97"
                    + tempVar + "0000000000000000"
                    + ui->charName->text() + ","
                    + us->getSkin()
                    + "," + ui->charWorld->text() + ","
                    + Helper::intToStr( static_cast<int>( std::time( nullptr ) ), IntBase::HEX, IntFills::DblWord ).toUpper();
            updatePlayerList( 0 );
        break;
        case '4':
            this->sendMixPacket( "4", trgSerNum, "" );
            packet = ":;o4" + srcSerNum + "D" + trgSerNum;
        break;
        case '5':
            packet = ":;o5" + srcSerNum + "D";
        break;
        case 'B':
            this->sendMixPacket( "4", trgSerNum, "" );
            packet = ":;oB"         + srcSerNum + "D" + trgSerNum
                    + "name=Owner"
                    + ",alias=ReMix"
                    + ",loc="
                    + ",email="
                    + ",guild=ReMix,home="
                    + ",bCRC="      + Helper::intToStr( ui->charBMod->text().toInt(), IntBase::DEC )
                    + ",lCRC="      + Helper::intToStr( ui->charCMod->text().toInt(), IntBase::DEC )
                    + ",travel=0"
                    + ",bSec="      + Helper::intToStr( ui->charBDate->text().toInt(), IntBase::DEC )
                    + ",nSec="      + Helper::intToStr( serverSel->getSoulHours() / 60, IntBase::DEC )
                    + ",nD=0"
                    + ",nK=0"
                    + ",nI=0"
                    + ",hLev=1"
                    + ",hGag=0,mods=0"
                    + ",rating=800"
                    + ",nPK=0"
                    + ",gID=0"
                    + ",bV=2711"
                    + ",gn=0"
                    + ",cc=0";
        break;
        case 'F':
            {
                //:;oF00000FA0D0001000280516C880001
                packet = ":;oF" + srcSerNum + "D" + QString( "%1" ).arg( us->getScene(), 4, 16, QChar( '0' ) ).toUpper()
                         + "0000" + "80516C88" + "0001";

                us->setIsCamped( true, 0 );
                us->setCampHost( us->getSerNum() );
                us->setScene( 0 );

//                MapLinks* links = world->getWorldMapLinks();
//                if ( links != nullptr )
//                {
//                    QString bckgnd = QString( links[ world->getNearestLinkID() ].linkBackground );
//                    scene->setBackgroundImg( bckgnd );

//                    ui->sceneView->installEventFilter( scene );
//                    ui->sceneView->setScene( scene );
//                    scene->addPlayerToScene( us, 0 );
//                }
            }
        break;
        case 'U':
            packet = ":;oU" + srcSerNum + "D" + Helper::intToStr( ui->charGuildID->text().toInt(), IntBase::HEX, IntFills::DblWord )
                    + "0000000000000000000000000000000000000000"
                    + ui->charGuild->text() + ",(null),(null)";
        break;
        case 'H':
            packet = ":;oH" + srcSerNum + "D" + "0000000000000000"
                    + Helper::intToStr( ui->charHalos->text().toInt() * 1000, IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( ui->charBravery->text().toInt(), IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( ui->charAgressiveness->text().toInt(), IntBase::HEX, IntFills::DblWord )
                    + "071DFC29" + "0000000000000000"
                    + Helper::intToStr( ui->charBSoul->text().toInt(), IntBase::HEX, IntFills::DblWord );
        break;
        case 'I':
            //:;oI000008AED271000020001
            //packet = ":;oI" + srcSerNum + "D" + subCode + "0001";
        break;
        case 'J':
            this->sendMixPacket( "1", trgSerNum, "" );
            packet = ":;oJ" + srcSerNum + "D" + trgSerNum
                    + Helper::intToStr( ui->charLevel->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charDef->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charAtk->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charHP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charHP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charMP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charMP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charEXP->text().toInt(), IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( ui->charStr->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charSta->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charAgi->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charDex->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charWis->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charMRatio->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + "0000"
                    + Helper::intToStr( ui->charHRatio->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + "0000000000000000000000000000";

            //us->setCampHost( Helper::serNumToInt( trgSerNum, true ) );
            //us->setIsCamped( true );

//            MapLinks* links = world->getWorldMapLinks();
//            if ( links != nullptr )
//            {
//                QString bckgnd = QString( links[ world->getNearestLinkID() ].linkBackground );
//                scene->setBackgroundImg( bckgnd );

//                ui->sceneView->installEventFilter( scene );
//                ui->sceneView->setScene( scene );
//                scene->addPlayerToScene( us, 0 );
//            }
        break;
        case 'L':
            this->sendMixPacket( "2", trgSerNum, "" );
            packet = ":;oL" + srcSerNum + "D" + trgSerNum;
            //us->setCampHost( 0 );
            //us->setIsCamped( false );

//            ui->sceneView->removeEventFilter( scene );
//            ui->sceneView->setScene( world );
//            world->addPlayerToMap( us, 0 );
        break;
            //case 'S':
            //    packet = ":;oS" + srcSerNum + "D" + "00000000" + "012475CA0000";
            //break;
        case 'V':
            {
                //Prevent Camp-Dragging.
                if ( us->getIsCamped() )
                    return;

                int ldrPos = 0;
                if ( trgSerNum.compare("") != 0 )
                {
                    ldrPos = getSerNumPosition( trgSerNum );
                    if ( ldrPos <= 0 )
                        return;
                }

                int speed = us->moveSpeed;
                int map = us->getMapID();

                if ( us->cmpPartyLeader( plrList[ ldrPos ]->getSerNum() )
                  && subCode == "M" )
                {
                    speed = plrList[ ldrPos ]->moveSpeed;
                    map = plrList[ ldrPos ]->getMapID();
                    us->targetX = plrList[ ldrPos ]->targetX + Helper::genRandNum( 0, 4095 );
                    us->targetY = plrList[ ldrPos ]->targetY + Helper::genRandNum( 0, 4095 );
                }
                packet = ":;oV" + srcSerNum + "D" + subCode
                        + Helper::intToStr( map, IntBase::HEX, IntFills::Word )
                        + Helper::intToStr( us->targetX, IntBase::HEX, IntFills::DblWord )
                        + Helper::intToStr( us->targetY, IntBase::HEX, IntFills::DblWord );
                if ( subCode == "M" )
                    packet += Helper::intToStr( speed, IntBase::HEX, IntFills::DblWord );

                if ( us->mapX != us->targetX
                  || us->mapY != us->targetY )
                {
                    us->setTargetPos( us->targetX, us->targetY, speed );
                }
            }
        break;
        case 'X':
            if ( us->getIsCamped()
              && !us->cmpCampHost( us->getSerNum() ) )
            {
                packet = QString( ":;oX%1D%2%3%4" )
                             .arg( srcSerNum )
                             .arg( Helper::intToStr( us->getCampHost(), IntBase::HEX, IntFills::DblWord ) )
                             .arg( Helper::intToStr( us->sceneX, IntBase::HEX, IntFills::Word ) )
                             .arg( Helper::intToStr( us->sceneY, IntBase::HEX, IntFills::Word ) );
            }
        break;
        case 'b':
            this->sendMixPacket( "4", trgSerNum, "" );
            packet = ":;ob" + srcSerNum + "D" + trgSerNum;
        break;
        case 'f':
            packet = ":;of" + srcSerNum + "D";
            us->setIsCamped( false );
            us->setCampHost( 0 );

//            ui->sceneView->removeEventFilter( scene );
//            ui->sceneView->setScene( world );
//            world->addPlayerToMap( us, 0 );
        break;
        case 'k':
//            tcpSocket->sendMixPacket( "4", trgSerNum, "" );
//            packet  = ":;ok"  + srcSerNum + "D" + trgSerNum + "000000000000000000000000";
//            sendPacketSwitch( "F", us->serNum_S, "", "", "" );
        break;
        case 'p':
        {
            int leader = Helper::serNumToInt( trgSerNum, true );

            packet = ":;op" + srcSerNum + "D";
            if ( us->cmpPartyLeader( leader )
              && !plrList [ 0 ]->cmpSerNum( leader ) )
            {
                packet += "00000000" + trgSerNum;
                leader = 0;
            }
            else if ( plrList [ 0 ]->cmpSerNum( leader ) )
                packet += "0000000000000000";
            else
                packet += trgSerNum + "00000000";

            packet += "0000000000000000";
            us->setPartyLeader( leader );
        }
        break;
        case 's':
            packet = ":;os" + srcSerNum + "D00000000"
                    + Helper::intToStr( ui->charLevel->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charDef->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charAtk->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charHP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charHP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charMP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charMP->text().toInt(), IntBase::HEX, IntFills::Word )
                    + Helper::intToStr( ui->charEXP->text().toInt(), IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( ui->charStr->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charSta->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charAgi->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charDex->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charWis->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charMRatio->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + "00000000000000000000"
                    + Helper::intToStr( us->getIsAFK(), IntBase::HEX, IntFills::Byte )
                    + Helper::intToStr( ui->charHRatio->text().toInt(), IntBase::HEX, IntFills::Byte )
                    + "000000000000"
                    + Helper::intToStr( ui->charPKRating->text().toInt() * 1000, IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( serverSel->getSoulHours() / 60, IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( ui->charBDate->text().toInt(), IntBase::HEX, IntFills::DblWord )
                    + Helper::intToStr( ui->charCVersion->text().toInt(), IntBase::HEX, IntFills::Word );
        break;
        case 'w':
            tcpSocket->sendMixPacket( "4", trgSerNum );
            packet = ":;ow" + srcSerNum + "D" + subCode + trgSerNum
                    + "0000000000000000" + message;
            parseChatEffect( packet );
        break;
        case 'y':
            tcpSocket->sendMixPacket( "4", trgSerNum );
            packet = ":;oy" + srcSerNum + "D" + trgSerNum + message + "00000004" + subCode;
        break;
    }
    if ( !packet.isEmpty() )
        tcpSocket->sendPacket( packet );
}
//case 'A':
//  v91 = wParam;
//  if ( a2 )
//  {
//    switch ( a2 )
//    {
//      case 1:
//        sprintf(
//          v7,
//          "%c%04X%04X%04X%04X%04X",
//          66,
//          (unsigned __int16)a3,
//          (unsigned __int16)dword_4E4874->Players[a3].plrFatigue,
//          (unsigned __int16)dword_4E4874->Players[a3].unk_217,
//          (unsigned __int16)dword_4E4874->Players[a3].unk_218,
//          (unsigned __int16)a4);
//        break;
//      case 2:
//        p_unk_30 = (unsigned int)&dword_4E4874->Players[a3].unk_30;
//        p_plrDefense = (int)&dword_4E4874->Players[a3].plrDefense;
//        v10 = sub_49B70F(p_plrDefense);
//        sub_42CB22(a3, a4, v10);
//        v88 = (int *)(p_unk_30 + 680);
//        v66 = v85;
//        v59 = (unsigned __int16)*(_DWORD *)(p_unk_30 + 684);
//        v55 = (unsigned __int16)sub_49B70F(p_unk_30 + 1520);
//        v51 = (unsigned __int16)*v88;
//        v11 = sub_49B70F(p_plrDefense);
//        sprintf(
//          v7,
//          "%c%04X%04X%04X%04X%04X%04X%04X%08X%s",
//          67,
//          (unsigned __int16)a3,
//          (unsigned __int16)a4,
//          v11,
//          v51,
//          v55,
//          v59,
//          (unsigned __int16)a5,
//          v66,
//          (const char *)Source);
//        sub_44A389(a3, 1);
//        if ( *(_DWORD *)(p_unk_30 + 4) == wParam )
//        {
//          sub_417A21();
//          v12 = playerArray;
//          v12->plrCurHP = sub_49B70F(p_plrDefense);
//          v12->plrMaxHP = *v88;
//        }
//        break;
//      case 3:
//        sprintf(v7, "%c%04X%08X%04X", 68, (unsigned __int16)a3, a4, (unsigned __int16)a5);
//        sub_43ABF5(a3, a4, a5);
//        break;
//      case 4:
//        sprintf(v82, aIMSendingMyCli);
//        nullsub_3(v82);
//        sprintf(v7, "%c%04X", 69, (unsigned __int16)a3);
//        break;
//      case 5:
//        sprintf(v82, aIMSendingMyCli_0);
//        nullsub_3(v82);
//        sprintf(v7, "%c%04X", 70, (unsigned __int16)a3);
//        break;
//    }
//  }
//  else
//  {
//    dword_D28490 = GetTickCount();
//    sprintf(v7, "%c", 65);
//  }
//  goto LABEL_119;
//case 'F':
//  sprintf(
//    v82,
//    "I am starting a scene (#%d, campfire %d) as the server at 0x%08X (crc 0x%08X)",
//    playerArray->sceneID,
//    a2,
//    playerArray->plrSernum,
//    dword_4FA95C);
//  nullsub_3(v82);
//  sprintf(
//    v7,
//    "%04X%04X%08X%04X",
//    (unsigned __int16)playerArray->sceneID,
//    (unsigned __int16)a2,
//    dword_4FA95C,
//    (unsigned __int16)dword_4E70C8);
//  goto LABEL_119;
//case 'G':
//  sprintf(v82, "I am giving my clients soemthing (%s)", (const char *)a6);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(v7, "%s,%08X", a6, a2);
//  goto LABEL_119;
//case 'I':
//  v14 = sub_48AE32(wParam, v86);
//  if ( !v14 )
//    return 0;
//  sprintf(
//    v7,
//    "%04X%04X%04X",
//    (unsigned __int16)*(_DWORD *)(v14 + 752),
//    (unsigned __int16)*(_DWORD *)(v14 + 748),
//    a2 & 0xFFF);
//  goto LABEL_119;
//case 'J':
//  sprintf(v82, "I am joining the scene at 0x%08X! (how unlike me!)", a2);
//  nullsub_3(v82);
//  SRNSendMIX(1, a2, ValueName, 0);
//  v15 = playerArray;
//  sub_417A21();
//  plrEqdWep = v15->plrEqdWep;
//  v71 = (unsigned __int16)v15->equipArray[13];
//  v67 = (unsigned __int16)v15->equipArray[12];
//  v63 = (unsigned __int16)v15->equipArray[11];
//  v60 = (unsigned __int16)v15->equipArray[10];
//  v56 = (unsigned __int16)v15->equipArray[1];
//  v52 = (unsigned __int16)v15->equipArray[0];
//  p_unk_30 = v15->plrHandRatio;
//  p_unk_30 = (unsigned __int8)p_unk_30;
//  sprintf(
//    v7,
//    "%08X%04X%04X%04X%04X%04X%04X%04X%08X%02X%02X%02X%02X%02X%02X%02X%02X%04X%04X%04X%04X%04X%04X%04X",
//    a2,
//    (unsigned __int16)v15->plrLevelPos,
//    (unsigned __int16)v15->plrDefPos,
//    (unsigned __int16)v15->plrAtkPos,
//    (unsigned __int16)v15->plrMaxHP,
//    (unsigned __int16)v15->plrMaxHP,
//    (unsigned __int16)v15->plrMaxMP,
//    (unsigned __int16)v15->plrMaxMP,
//    v15->plrExpPos,
//    (unsigned __int8)v15->plrStrength,
//    (unsigned __int8)v15->plrStamina,
//    (unsigned __int8)v15->plrAgility,
//    (unsigned __int8)v15->plrDexterity,
//    (unsigned __int8)v15->plrWisdom,
//    (unsigned __int8)v15->plrMagicRatio,
//    (unsigned __int8)plrEqdWep,
//    (unsigned __int8)p_unk_30,
//    (unsigned __int16)plrEqdWep,
//    v52,
//    v56,
//    v60,
//    v63,
//    v67,
//    v71);
//  goto LABEL_119;
//case 'K':
//  if ( !a2 && sub_414F90(Source) )
//    return 0;
//  sprintf(
//    v7,
//    "%02X%04X%04X%04X%04X%04X%04X%04X%04X%08X%02X%02X%02X%02X%02X%08X",
//    (unsigned __int8)a2,
//    (unsigned __int16)Source->petData[0].petMonID,
//    (unsigned __int16)Source->petData[0].petLvlPos,
//    (unsigned __int16)Source->petData[0].petCurrentHP,
//    (unsigned __int16)Source->petData[0].petMaxHP,
//    (unsigned __int16)Source->petData[0].petCurrentMP,
//    (unsigned __int16)Source->petData[0].petMaxMP,
//    (unsigned __int16)Source->petData[0].petAtkRating,
//    (unsigned __int16)Source->petData[0].petDefRating,
//    Source->petData[0].petEXPPos,
//    (unsigned __int8)Source->petData[0].petStrength,
//    (unsigned __int8)Source->petData[0].petStamina,
//    (unsigned __int8)Source->petData[0].petWisdom,
//    (unsigned __int8)Source->petData[0].petDexterity,
//    (unsigned __int8)Source->petData[0].petAgility,
//    Source->petData[0].unk_05);
//  v17 = playerArray->sceneHostSernum;
//  if ( v17 )
//    SRNSendMIX(4, v17, ValueName, 0);
//  goto LABEL_119;
//case 'L':
//  sprintf(v82, "I am running out on the scene at 0x%08X! (how typical!)", a2);
//  nullsub_3(v82);
//  if ( !playerArray->plrPartyLeader )
//    SRNSendMIX(2, 0, ValueName, 0);
//  sprintf(v7, "%08X", a2);
//  goto LABEL_119;
//case 'O':
//  sprintf(v82, "I am telling my clients to display the SHOP button (%s)", (const char *)a6);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(v7, "%s", (const char *)a6);
//  goto LABEL_119;
//case 'R':
//  sprintf(v7, "%04X%04X%04X%08X", (unsigned __int16)a2, (unsigned __int16)a3, (unsigned __int16)a4, dword_502830);
//  goto LABEL_119;
//case 'S':
//  v18 = (_DWORD *)sub_48AE32(wParam, v86);
//  if ( !v18 )
//    return 0;
//  sprintf(
//    v7,
//    "%04X%04X%08X%04X",
//    (unsigned __int16)v18[186],
//    (unsigned __int16)v18[187],
//    v18[228],
//    (unsigned __int16)v18[229]);
//  goto LABEL_119;
//case 'T':
//  sprintf(v82, "I am taking something from my clients (%s)", (const char *)a6);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(v7, "%s,%08X", a6, a2);
//  goto LABEL_119;
//case 'W':
//  sprintf(v82, "I am telling my clients to play wave file '%s' in mode %d", (const char *)a6, a2);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(v7, "%04X%s", (unsigned __int16)a2, a6);
//  goto LABEL_119;
//case 'X':
//  sprintf(v82, "I am telling my scene server 0x%08X that I am moving now", a2);
//  nullsub_3(v82);
//  sprintf(v7, "%08X%04X%04X", a2, (unsigned __int16)a3, (unsigned __int16)a4);
//  SRNSendMIX(4, a2, ValueName, 0);
//  goto LABEL_119;
//case 'a':
//  sprintf(v82, "I am telling my clients to set monster %d to pose %d", a3, a2);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(
//    v7,
//    "%04X%04X%04X%04X%s",
//    (unsigned __int16)a3,
//    (unsigned __int16)a2,
//    (unsigned __int16)a4,
//    (unsigned __int16)a5,
//    (const char *)a6);
//  goto LABEL_119;
//case 'd':
//  sprintf(v7, "%08X%08X%08X%08X%s,%s", a2, a3, a4, a5, (const char *)a6, (const char *)Source);
//  switch ( a3 )
//  {
//    case -2:
//      v91 = wParam;
//      break;
//    case -1:
//      a3 = playerArray->sceneHostSernum;
//      break;
//    case 0:
//      break;
//    default:
//      SRNSendMIX(4, a3, ValueName, 0);
//      break;
//  }
//  goto LABEL_119;
//case 'f':
//  sprintf(v82, "I am stopping a scene as the server at 0x%08X", playerArray->plrSernum);
//  nullsub_3(v82);
//  goto LABEL_119;
//case 'g':
//  sprintf(v82, "I am telling my clients to display the GAME button (%s)", (const char *)a6);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(v7, "%s", (const char *)a6);
//  goto LABEL_119;
//case 'k':
//  sprintf(v7, "%08X%08X%08X%08X%s", a2, a3, a4, a5, (const char *)a6);
//  SRNSendMIX(4, a2, ValueName, 0);
//  goto LABEL_119;
//case 'm':
//  v19 = (plrScene *)&dword_4E4874->Players[a2].unk_30;
//  sprintf(
//    v82,
//    "I am updating monster state for my scene clients (%d:%s)",
//    a2,
//    (const char *)&dword_4E4874->Players[a2].unk_32);
//  nullsub_3(v82);
//  unk_150 = v19->unk_150;
//  v91 = wParam;
//  v21 = -(v19->unk_162 == 0);
//  p_plrDefense = (unk_150 & 0xF) << 12;
//  v22 = v19->unk_253 == 0;
//  p_unk_30 = (16 * (unsigned __int8)v19->unk_157) | (v21 + 1);
//  if ( !v22 )
//    LOBYTE(p_unk_30) = p_unk_30 | 2;
//  if ( v19->unk_146 == v19->unk_148 && v19->unk_147 == v19->unk_149 )
//    LOBYTE(p_unk_30) = p_unk_30 | 4;
//  if ( v19->skinIsMirrored )
//    LOBYTE(p_unk_30) = p_unk_30 | 8;
//  if ( v19->plrSernum == -1 && v19->petOwnerSerNum == wParam )
//    sub_414962(v19);
//  sub_490CE4();
//  unk_276 = (unsigned __int8)v19->unk_276;
//  unk_274 = (unsigned __int8)v19->unk_274;
//  petOwnerSerNum = v19->petOwnerSerNum;
//  monsterID = (unsigned __int16)v19->monsterID;
//  magicPercent = (unsigned __int8)v19->magicPercent;
//  plrExp = v19->plrExp;
//  atkWeaponID = (unsigned __int16)v19->atkWeaponID;
//  v47 = (unsigned __int16)p_unk_30;
//  v46 = v19->skinFrame_1 + 'A';
//  unk_262 = (unsigned __int8)v19->unk_262;
//  handPercent = (unsigned __int8)v19->handPercent;
//  plrWisdom = (unsigned __int8)v19->plrWisdom;
//  plrDexterity = (unsigned __int8)v19->plrDexterity;
//  plrAgility = (unsigned __int8)v19->plrAgility;
//  plrStamina = (unsigned __int8)v19->plrStamina;
//  plrStrength = (unsigned __int8)v19->plrStrength;
//  plrMP = (unsigned __int16)v19->plrMP;
//  v37 = (unsigned __int16)sub_49B70F(&v19->plrCurrentMP);
//  plrHP = (unsigned __int16)v19->plrHP;
//  v35 = (unsigned __int16)sub_49B70F(&v19->plrCurrentHP);
//  v34 = (unsigned __int16)sub_49B70F(&v19->plrAttack);
//  v33 = (unsigned __int16)sub_49B70F(&v19->plrDefense);
//  v23 = sub_49B70F(&v19->plrLevel);
//  sprintf(
//    v7,
//    "%c%c%04X%04X%08X%04X%04X%04X%04X%04X%04X%04X%02X%02X%02X%02X%02X%02X%02X%c%04X%04X%s,%s,%08X%02X%04X%08X%02X%02X%02X",
//    a2 + 'A',
//    v19->incarType + 'A',
//    (unsigned __int16)v19->unk_148,
//    (unsigned __int16)v19->unk_149,
//    v19->plrSernum,
//    p_plrDefense | v23 & 0xFFF,
//    v33,
//    v34,
//    v35,
//    plrHP,
//    v37,
//    plrMP,
//    plrStrength,
//    plrStamina,
//    plrAgility,
//    plrDexterity,
//    plrWisdom,
//    handPercent,
//    unk_262,
//    v46,
//    v47,
//    atkWeaponID,
//    v19->plrSkinName,
//    v19->plrName,
//    plrExp,
//    magicPercent,
//    monsterID,
//    petOwnerSerNum,
//    unk_274,
//    unk_276,
//    v74);
//  goto LABEL_119;
//case 's':
//  v25 = playerArray;
//  playerArray->plrIsAFK = dword_4E48D0;
//  p_unk_30 = dword_4E48D0 != 0;
//  if ( dword_4EF134 )
//    LOBYTE(p_unk_30) = p_unk_30 | 2;
//  if ( !dword_4E4834 )
//    LOBYTE(p_unk_30) = p_unk_30 | 4;
//  p_unk_30 |= 8 * (v25->plrGender & 3);
//  if ( v25->plrCampLockState )
//    LOBYTE(p_unk_30) = p_unk_30 | 0x20;
//  if ( a2 )
//    LOBYTE(p_unk_30) = p_unk_30 | 0x40;
//  if ( sub_467312(29) )
//    LOBYTE(p_unk_30) = p_unk_30 | 0x80;
//  sub_417A21();
//  plrGameBirthVer = v25->plrGameBirthVer;
//  plrBirthDate = v25->plrBirthDate;
//  v65 = v25->plrPlayTime / 60;
//  v26 = sub_43A9BE(v25);
//  sprintf(
//    v7,
//    "%08X%04X%04X%04X%04X%04X%04X%04X%08X%02X%02X%02X%02X%02X%02X%02X%08X%08X%02X%02X%02X%04X%08X%08X%08X%08X%04X",
//    v25->sceneHostSernum,
//    (unsigned __int16)v25->plrLevelPos,
//    (unsigned __int16)v25->plrDefPos,
//    (unsigned __int16)v25->plrAtkPos,
//    (unsigned __int16)v25->plrCurHP,
//    (unsigned __int16)v25->plrMaxHP,
//    (unsigned __int16)v25->plrCurMP,
//    (unsigned __int16)v25->plrMaxMP,
//    v25->plrExpPos,
//    (unsigned __int8)v25->plrStrength,
//    (unsigned __int8)v25->plrStamina,
//    (unsigned __int8)v25->plrAgility,
//    (unsigned __int8)v25->plrDexterity,
//    (unsigned __int8)v25->plrWisdom,
//    (unsigned __int8)v25->plrMagicRatio,
//    (unsigned __int8)v25->plrModderPos,
//    v25->plrLastBless,
//    v25->plrLastCurse,
//    v25->plrCheatNeg,
//    p_unk_30,
//    (unsigned __int8)v25->plrHandRatio,
//    (unsigned __int16)v25->plrEqdWep,
//    v25->unk_25,
//    v26,
//    v65,
//    plrBirthDate,
//    plrGameBirthVer);
//  sub_439F7A(2, aIToldThem);
//  goto LABEL_119;
//case 't':
//  sprintf(v82, "I am telling my clients to play a new sound theme (%d)", a2);
//  nullsub_3(v82);
//  v91 = wParam;
//  sprintf(v7, "%04X", (unsigned __int16)a2);
//  goto LABEL_119;
//case 'u':
//  sprintf(v82, "I am telling my clients to play a new midi file (%s)", a6);
//  v91 = wParam;
//  sprintf(v7, "%s,%04X", a6, (unsigned __int16)a2);
//  goto LABEL_119;
//case 'w':
//  if ( sub_43A4FD() )
//    return 0;
//  SRNSendMIX(4, a3, ValueName, 0);
//  v90 = 1;
//  sprintf(v7, "%02X%08X%08X%08X%s", (unsigned __int8)a2, a3, a4, a5, (const char *)a6);
//  goto LABEL_119;
//case 'z':
//  sprintf(v82, "I am telling my clients to leave party unless they meet these conditions (%s)", a6);
//  v91 = wParam;
//  sprintf(v7, "%s,%04X", a6, (unsigned __int16)a2);
//  goto LABEL_119;
