
#ifndef QT_PROTOTYPES_HPP
    #define QT_PROTOTYPES_HPP

//Required Qt Includes. --Unable to prototype in most instances.
//    #include <QElapsedTimer>
//    #include <QtConcurrent>
//    #include <QMainWindow>
//    #include <QModelIndex>
//    #include <QTcpServer>
//    #include <QFuture>
//    #include <QObject>
//    #include <QDialog>
//    #include <QTimer>

    #include <QtCore/QtContainerFwd>
    class QNetworkAccessManager;
    class QSortFilterProxyModel;
    class QNetworkAddressEntry;
    class QCryptographicHash;
    class QStandardItemModel;
    class QNetworkInterface;
    class QTableWidgetItem;
    class QStringListModel;
    class QXmlStreamReader;
    class QXmlStreamWriter;
    class QNetworkRequest;
    class QSystemTrayIcon;
    class QTreeWidgetItem;
    class QNetworkReply;
    class QStandardItem;
    class QElapsedTimer;  //Requires Include within Class Headers.
    class QStyleFactory;
    class QApplication;
    class QHostAddress;
    class QInputDialog;
    class QTreeWidget;
    class QTextStream;
    class QFileDialog;
    class QToolButton;
    class QModelIndex;  //Requires Include within Class Headers.
    class QMainWindow;  //Requires Include within Class Headers.
    class QMessageBox;
    class QCloseEvent;
    class QTextCursor;
    class QGridLayout;
    class QByteArray;
    class QScrollBar;
    class QTcpServer;  //Requires Include within Class Headers.
    class QTcpSocket;
    class QUdpSocket;
    class QTableView;
    class QTabWidget;
    class QCollator;  //Requires Include within Class Headers.
    class QHostInfo;
    class QDateTime;
    class QFileInfo;
    class QSettings;
    class QTextEdit;
    class QPalette;
    class QLibrary;
    class QToolTip;
    class QtEndian;
    class QProcess;
    class QLayout;
    class QDialog;  //Requires Include within Class Headers.
    class QString;
    class QObject;  //Requires Include within Class Headers.
    class QTimer;  //Requires Include within Class Headers.
    class QBrush;
    class QDebug;
    class QEvent;
    class QPoint;
    class QIcon;
    class QChar;
    class QMenu;
    class QDir;
    class QUrl;  //Requires Include within Class Headers.

#endif // QT_PROTOTYPES_HPP

#ifndef REMIX_PROTOTYPES_HPP
    #define REMIX_PROTOTYPES_HPP

    //Table-View Classes.
    class UserSortProxyModel;
    class PlrSortProxyModel;
    class CmdTableDelegate;
    class ListEventFilter;
    class TblEventFilter;
    class UserDelegate;

    //Widget GUI Classes.
    class SettingsWidget;
    class ReMixTabWidget;
    class PlrListWidget;
    class ReMixWidget;
    class RulesWidget;
    class ColorWidget;
    class MOTDWidget;
    class ChatView;

    //Threaded Classes.
    class MasterMixThread;
    class WriteThread;
    class UdpThread;

    //Packet Handlers.
    class WoSPacketHandler;
    class ToYPacketHandler;
    class W97PacketHandler;

    //UPnP Classes
    class UPnPDiscoverPrivate;
    class UPnPServicePrivate;
    class UPnPDevicePrivate;
    class UPnPWanService;
    class UPnPDiscover;
    class UPnPService;
    class UPnPDevice;
    class UPnPReply;
    class UPnPSoap;
    class UPNP;

    //GUI and Data Storage Classes.
    class CmdTableOverride;
    class CreateInstance;
    class AppEventFilter;
    class PacketHandler;
    class CampExemption;
    class SelectWorld;
    class PacketForge;
    class CmdHandler;
    class CmdTable;
    class Settings;
    class RunGuard;
    class RandDev;
    class SendMsg;
    class Player;
    class Helper;
    class Logger;
    class Server;
    class Theme;
    class ReMix;
    class User;

    #ifndef REMIX_GLOBALS
        #define REMIX_GLOBALS

        enum class Globals: int
        {
            //Anything above PACKET_FLOOD_LIMIT (512 packets)
            //will disconnect the User.
            PACKET_FLOOD_TIME = 1000,

            //Users are able to send 512 packets within PACKET_FLOOD_TIME.
            PACKET_FLOOD_LIMIT = 512,

            //Minimum hex-SERNUM allowed. This is used to validate a hex-SERNUM.
            //Via: sernum & MIN_HEX_SERNUM
            MIN_HEX_SERNUM = 0x40000000,

            //Maximum time a User may remain inactive with "Disconnect
            //Idle Users" enabled. 10 minutes - 600,000 milliseconds.
            MAX_IDLE_TIME = 600000,

            //Maximum time a User may remain inactive before being marked with
            //an AFK icon. 5 minutes - 300,000 milliseconds.
            MAX_AFK_TIME = 300000,

            //Maximum interval for ReMix to refresh the MasterMix Information.
            //3 Hours in milliseconds. 3 * 60 * 60 * 1000
            MASTER_MIX_UPDATE_INTERVAL = 10800000,

            //Maximum amount of time in MS (30 seconds) to allow for the
            //Master to respond to a checkin.
            MAX_MASTER_RESPONSE_TIME = 30000,

            //Maximum amount of time in MS (5 Minutes)
            //before re-announcing to the Master Server.
            MAX_MASTER_CHECKIN_TIME = 300000,

            //Minimum amount of time in MS (30 Seconds)
            //before re-announcing to the Master Server.
            MIN_MASTER_CHECK_IN_TIME = 30000,

            //Minimum amount of time in MS (0.5 Seconds).
            MIN_MASTER_STARTUP_TIME = 500,

            //Amount of times an un-authenticated (admin) can use commands.
            MAX_CMD_ATTEMPTS = 3,

            //Amount of Players that are able to connect to the server.
            //Reduced from 256 to 255 due to Packet Header Slot Limitations.
            MAX_PLAYERS = 255,

            //Length of a Remote Admin's password salt.
            SALT_LENGTH = 12,

            //Maximum length allowed for a GameInfo String to be set and sent to
            //players that ping our servers.
            MAX_GAME_NAME_LENGTH = 256,

            //Maximum length allowed for a SSV FileName Path.
            MAX_SSV_PATH_LENGTH = 255,

            //Time interval for updating the server's Usage array.
            SERVER_USAGE_UPDATE = 600000,

            //Size of the Server Usage array corresponding to 48 hours.
            SERVER_USAGE_48_HOURS = 288,

            //Size of the Server Usage array corresponding to Days.
            SERVER_USAGE_DAYS = 156,

            //Size of the Server Usage array corresponding to Hours.
            SERVER_USAGE_HOURS = 7,

            //Size of the Server Usage array corresponding to Minutes.
            SERVER_USAGE_MINUTES = 3,

            //Maximum Server Count a User can have at any time.
            MAX_SERVER_COUNT = 19,

            //Count of possible Game Names we can use to initialize the server.
            GAME_NAME_COUNT = 3,

            //Maximum live-time for a UPNP PortForward to live.
            //1 Hour in seconds.
            UPNP_TIME_OUT_S = 3600,

            //Maximum live-time for a UPNP PortForward to live.
            //Permanent Lease.
            UPNP_TIME_OUT_PERMA = 0,

            //Maximum idle-time for a UPNP PortForward exist without refreshing.
            //30 Minutes in milliseconds.
            UPNP_TIME_OUT_MS = 1800000,

            //Maximum amount of time ReMix will wait for a UPNP Port Forward Response.
            //5 Seconds in Milliseconds.
            UPNP_RESPONSE_TIME_OUT = 5000,

            //Count of currently accepted UPNP Schemas
            UPNP_SCHEMA_COUNT = 6,

            //Maximum wait time before the Server's UI is updated.
            //0.5 seconds in milliseconds.
            UI_UPDATE_TIME = 500,

            //Maximum time a User may remain connected after having a
            //"disconnect" initiated on their socket. (250MS)
            MAX_DISCONNECT_TTL = 250,

            //Maximum time a User may remain connected with an invalid or '0' SerNum value.
            //5 Minutes in milliseconds.
            MAX_SERNUM_TTL = 300000,

            //Maximum Count that ReMix will allow a User to send a packet with the incorrect Packet ID.
            MAX_PKT_HEADER_EXEMPT = 5,

            //Time before the server sends the keep alive packet for ReMix's designated sernum.
            //30 Seconds in milliseconds.
            MASTER_SERNUM_KEEPALIVE = 30000,

            //The gave period for an Admin's Visible state to remain valid before they must log in.
            //5 minues in milliseconds.
            PLAYER_VISIBLE_LOAD_TIMEOUT = 300000,

            //The maximum level a WoS Client may be.
            MAX_PLAYER_LEVEL = 100,

            //The minimum level a WoS Client may be.
            MIN_PLAYER_LEVEL = 1,

            //The minimum count of Attack packets to detect before assuming Infinite Cursor is active on a WoS Client.
            MIN_INFCURSOR_COUNT = 3,

            //Time before the server sends a ping to a Player.
            //30 Seconds in milliseconds.
            PING_UPDATE_TIME = 30000,

            //The Maximum response time for a Player Ping for the UI to show. 999 milliseconds.
            MAX_PLAYER_PING_TIME = 999,
        };

        //Valid Password types.
        enum class PwdTypes: int{ Server = 0, Admin = 1, Invalid = -1 };

        //Valid columns within the CmdTableOverride Dialog.
        enum class CmdOverrideCols: int{ Command = 0, Rank, Description, Usage, ColCount = 4 };

        //Valid Remote Administrator Ranks.
        enum class GMRanks: int{ User = 0, GMaster, CoAdmin, Admin, Owner,
                                 Creator = 5, Invalid = -1 };

        //Valid Remote Administrator commands.
        enum class GMCmds: int{ Help = 0, List, MotD, Info, Ban,
                                UnBan, Kick, Mute, UnMute, Quarantine, UnQuarantine,
                                Message, LogIn, LogOut,
                                Register, ShutDown, ReStart,
                                CHRules, CHSettings, Vanish, Version, Ping,
                                Camp, Guild, Invalid = -1 };

        //Valid Remote Administrator subcommands.
        enum class GMSubCmds: int{ MotDChange, MotDRemove,
                                   InfoServer, InfoQuarantined, InfoMuted, InfoIP, InfoSoul,
                                   BanIP, BanSoul, BanAll,
                                   UnBanIP, UnBanSoul, UnBanAll,
                                   KickIP, KickSoul, KickAll,
                                   MuteIP, MuteSoul, MuteAll,
                                   UnMuteIP, UnMuteSoul, UnMuteAll,
                                   QuarantineIP, QuarantineSoul,
                                   UnQuarantineIP, UnQuarantineSoul,
                                   MessageIP, MessageSoul, MessageAll,
                                   ShutDownStop, ReStartStop,
                                   VanishHide, VanishShow, VanishStatus,
                                   PingIP, PingSoul,
                                   CampLock, CampUnLock, CampAllowAll, CampAllowCurrent, CampAllow, CampRemove, CampStatus, CampSoul,
                                   Invalid = -1 };

        //Valid Remote Administrator sub-commands.
        enum class GMSubCmdIndexes: int{ Zero = 0, One, Two, Three, Four, Five,
                                         Six, Seven = 7, Invalid = -1 };

        //Valid Command Structure Format.
        enum class CmdTblFmt: int{ Cmd = 0, SubCommands, SubCommandCount,
                                   CmdInfo, CmdSyntax, CmdRank,
                                   CmdIsActive = 6 };

        //Valid SerNum response codes.
        enum class UserListResponse: int{ Q_Response = 0, R_Response = 1 };

        //Valid Game ID's that ReMix can call to.
        enum class Games: int{ WoS = 0, ToY = 1, W97 = 2, Invalid = -1 };

        //Valid methods of banning Users.
        //Valid punishment types.
        enum class PunishTypes: int{ Mute = 0, Ban, SerNum, IP, DV, WV = 5 };

        //Valid forms of diconnecting Users.
        enum class DCTypes: int{ IPDC = 0, DupDC, PktDC, Invalid = 3 };

        //Valid Theme ID's.
        enum class Themes: int{ Light = 0, Dark = 1 };

        //Valid Icons for use in the PlrListWidget SerNum DecorationRole.
        enum class IconRoles: int { Invalid = -1, SoulAFK = 0, SoulAFKWell, SoulGhost, SoulNPK, SoulPK, SoulWell, GSoulGhost,
                                    GSoulNPK, GSoulPK, SoulCheater, SoulModder, SoulMuted = 12, RoleCount = 12 };

        //Valid Theme Colors
        enum class Colors: int{ GossipTxt = 0, ShoutTxt, EmoteTxt, DeathTxt, SpellTxt, DiceAndLevel, PlayerTxt, AdminTxt, AdminMessage, OwnerTxt, CommentTxt,
                                GoldenSoul, WhiteSoul, PlayerName, AdminName, OwnerName, TimeStamp, AdminValid, AdminInvalid, IPValid,
                                IPInvalid, IPQuarantined, IPVanished, PartyJoin, PKChallenge, SoulIncarnated, SoulLeftWorld,
                                PlayerPingGood, PlayerPingMedium, PlayerPingBad = 29, ColorCount = 29, Default = -1 };

        //Valid columns within the PlrListWidget.
        enum class PlrCols: int{ IPPort = 0, PlrPing, SerNum, Name, Age, Alias, Time,
                                 BytesIn, BytesOut, BioData = 10, ColCount = 10 };

        //Valid columns within the User Dialog.
        enum class UserCols: int{ SerNum = 0, /*Pings,*/ Calls, LastSeen, IPAddr,
                                  Rank, Muted, MuteDate, MuteDuration,
                                  MuteReason, Banned, BanDate, BanDuration,
                                  BanReason = 12, ColCount = 13 };

        //Valid columns within the Logger Dialog.
        enum class LogCols: int{ Date = 0, Source, Type, Message = 3, ColCount = 4 };

        //Valid Key values for use within the Logger and WriteThread Classes.
        enum class LKeys: int{ AllLogs = 0, AdminLog, CommentLog, ClientLog, MasterMixLog, UPNPLog, PunishmentLog, MiscLog, ChatLog, SSVLog, PingLog };

        //Used for converting time in seconds to a human readable format.
        enum class TimePeriods: int{ Years = 365*24*60*60, Days = 24*60*60, Hours = 60*60, Minutes = 60, Seconds = 1, Default = 0 };
        enum class TimeFormat: int{ Days = 0, Hours = 1, Minutes = 2, Seconds = 3, Default = -1 };
        enum class TimeDivide: int{ Days = 24, Hours = 3600, Minutes = 60, Seconds = 60, Miliseconds = 1000 };
        enum class TimeMultiply: int{ Hours = 60, Minutes = 60, Seconds = 60, Milliseconds = 1000 };
        enum class TimeInterval: int{ Day = 86400, Hour = 3600, Minute = 60, Second = 1, Millisecond = 1000 };

        //Valid Ban Durations in seconds.
        //1_Day, 7 Days, 30 Days, 6 Months, 1 Year, Permanent
        enum class PunishDurations: int{ Invalid = 0,
                                         THIRTY_SECONDS = 30,
                                         ONE_MINUTE = 60,
                                         TEN_MINUTES = 600,
                                         THIRTY_MINUTES = 1800,
                                         ONE_HOUR = 3600,
                                         ONE_DAY = 86400,
                                         SEVEN_DAYS = 604800,
                                         THIRTY_DAYS = 2592000,
                                         SIX_MONTHS = 15552000,
                                         ONE_YEAR = 31104000,
                                         PERMANENT = 2147483647 };

        //Valid target specifiers for packets. Used by the MIX packet type.
        enum class PktTarget: int{ ALL = 0, PLAYER, SCENE = 2 };

        //Valid Key values for use within the User Class.
        enum class UKeys: int{ Seen = 0, Bio, CurrentIP, LastIP, DV, WV, Rank, Hash, Salt,
                               Muted, MuteDuration, MuteReason,
                               Banned, BanDuration, BanReason,
                               Pings, Calls, InVisible = 17 };

        //Valid Key values for use within the Rules and Settings Classes.
        enum class SKeys: int{ Setting = 0, Messages, Positions, Rules, Logger, Colors, CommandRanks = 6 };

        //Valid Sub-Key values for use within the Settings Class.
        enum class SSubKeys: int{ ServerID = 0, AutoBan, AllowIdle, ReqSerNum, AllowDupe, AllowSSV, BanDupes, CensorIPInfo, MOTD, LogComments, FwdComments,
                                  InformAdminLogin, EchoComments, MinimizeToTray, SaveWindowPositions, IsRunning, WorldDir, PortNumber, IsPublic, GameName,
                                  LogFiles, DarkMode, UseUPNP, CheckForUpdates, DCBlueCodedSerNums, LoggerAutoScroll, OverrideMasterIP, LoggerAutoClear,
                                  OverrideMasterHost, ChatAutoScroll, ChatTimeStamp, HidePlayerView, HideChatView, NetInterface, ServerButtonState,
                                  ServerPlayerChatSize, HasSvrPassword, SvrPassword, WorldName, ToYName, SvrUrl, AllPK, MaxPlayers, MaxAFK, MinVersion,
                                  PKLadder, NoBleep, NoCheat, NoEavesdrop, NoMigrate, NoModding, NoPets, NoPK, ArenaPK, AutoRestart, StrictRules, KeyCount,
                                  PlayerEmulation };

        //Valid Toggles for the Settings Widget.
        enum class SToggles: int{ AllowDupeIP = 0, BanDupeIP, CensorIPInfo, ReqSerNum, DCBlueCode, AllowIdle, PlayerEmulation, AllowSSV, LogComments,
                                  FwdComments, EchoComments, InformAdminLogin, MinimizeToTray, SaveWindowPositions, LogFiles, WorldDir,
                                  OverrideMasterHost, OverrideMaster = 17 };

        //Valid Toggles for the Rules Widget.
        enum class RToggles: int{ StrictRules = 0, ServerPassword, AutoRestart, WorldName, UrlAddr, MaxPlayers, MaxAFK, MinVersion, Ladder, NoBleep,
                                  NoCheat, NoEavesdrop, NoMigrate, NoModding, NoPets, NoPK, AllPK, ArenaPK = 17 };

        //Valid bases for use in converting strings to integers.
        enum class IntBase: int{ OCT = 8, DEC = 10, HEX = 16 };

        //Valid Filler lengths for IntBase::Hex conversions.
        enum class IntFills: int{ Bit = 1,       //Half a Byte
                                  Byte = 2,      //Byte
                                  Word = 4,      //Two Bytes
                                  DblWord = 8,   //Int
                                  QuadWord = 16, //Int64
                                };

        //Valid MessageBox types.
        enum class MessageBox: int{ SingleLine = 0, MultiLine = 1, Invalid = -1 };

        //Valid Units for conversion within the function Helper::sanitizeToFriendlyUnits
        enum class ByteUnits: int{ Byte,     // 1024^0,
                                   KibiByte, // 1024^1,
                                   MebiByte, // 1024^2,
                                   GibiByte, // 1024^3,
                                   TebiByte, // 1024^4,
                                   PebiByte, // 1024^5,
                                   ExbiByte, // 1024^6
                                 };

        //Valid forms of a Chat Packet.
        enum class ChatType: int{ Normal = 0, DiceAndLevelUp, Unk1 = 2, LearnSpell = 3,
                                  Unk2 = 4, Unk3 = 5, PetCmd = 6, SceneMsg = 10, DeathMsg = 11 };

        //Valid SSV Modes. Read/Write.
        enum class SSVMode: int{ Read = 0, Write = 1, Invalid = -1,  };

        //Valid Player Disconnect Types.
        enum class PlrDisconnectType: int{ SerNumChanged = 0, InvalidSerNum, BlueCodeSerNum, SerNumOne, Invalid = -1 };

        //The SerNum ReMix will use for any packets forged or emulated.
        enum class ReMixSerNum: quint32{ SerNum = 0xFFFFFB2E };

        //Valid WoS Packets that ReMix will read information from, modify, or emulate.
        enum class WoSPacketTypes: char{ Incarnation = '3', ServerLeave = '5', Chat = 'C', PKAttack = 'k', PartyState = 'p', PlayerCamp = 'F',
                                         PlayerUnCamp = 'f', PlayerStatus = 's', SkinTransfer = 'x', PetCall = 'K', CharacterInfo = 'H',
                                         BioRequest = 'b', BioResponse = 'B', GuildInfo = 'U', InfoRequest = '4', HeartBeat = '0',
                                         SceneTossedItem = 'I', SceneAttackAction = 'S', ScenePromptAction = 'A',
                                         CampJoin = 'J', CampLeave = 'L', WorldHello = '1', WorldGetOut = '2', };

        //Valid forms of a Skin Transfer Packet.
        enum class IncarnationType: int{ Incarnation = 1, GhostIncarnation = 2, DisIncarnation = 4 };

        //Valid forms of a Skin Transfer Packet.
        enum class SkinTransferType: int{ RequestModeOne = 1, RequestModeTwo = 6, Offer = 2, DataRequest = 3, DataTransfer = 4, ThanksForSkin = 5 };

        //Valid Tab Indexes for the Settings Tab View.
        enum class SettingsTabIndexes: int{ Settings = 0, Rules, Colors, MoTD };

        template <typename T>
        static constexpr auto operator*(T e) noexcept
            -> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
        {
            return static_cast<std::underlying_type_t<T>>(e);
        }

    #endif  // REMIX_GLOBALS

#endif // REMIX_PROTOTYPES_HPP
