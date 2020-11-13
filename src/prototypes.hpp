
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

    class QNetworkAccessManager;
    class QSortFilterProxyModel;
    class QNetworkAddressEntry;
    class QCryptographicHash;
    class QStandardItemModel;
    class QNetworkInterface;
    class QTableWidgetItem;
    class QStringListModel;
    class QXmlStreamReader;
    class QNetworkRequest;
    class QSystemTrayIcon;
    class QNetworkReply;
    class QStandardItem;
    class QElapsedTimer;  //Requires Include within Class Headers.
    class QStyleFactory;
    class QApplication;
    class QHostAddress;
    class QInputDialog;
    class QTextStream;
    class QFileDialog;
    class QToolButton;
    class QModelIndex;  //Requires Include within Class Headers.
    class QMainWindow;  //Requires Include within Class Headers.
    class QMessageBox;
    class QCloseEvent;
    class QStringList;
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
    class QVariant;
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
    class QFile;
    class QMenu;
    class QDir;
    class QUrl;

#endif // QT_PROTOTYPES_HPP

#ifndef REMIX_PROTOTYPES_HPP
    #define REMIX_PROTOTYPES_HPP

    //Table-View Classes.
    class UserSortProxyModel;
    class PlrSortProxyModel;
    class TblEventFilter;

    //Widget GUI Classes.
    class SettingsWidget;
    class ReMixTabWidget;
    class PlrListWidget;
    class ReMixWidget;
    class RulesWidget;
    class MOTDWidget;

    //Threaded Classes.
    class WriteThread;
    class UdpThread;

    //GUI and Data Storage Classes.
    class CreateInstance;
    class AppEventFilter;
    class PacketHandler;
    class CampExemption;
    class SelectWorld;
    class PacketForge;
    class ServerInfo;
    class CmdHandler;
    class CmdTable;
    class ChatView;
    class Settings;
    class Comments;
    class RunGuard;
    class RandDev;
    class SendMsg;
    class Player;
    class Server;
    class Helper;
    class Logger;
    class Theme;
    class ReMix;
    class User;
    class UPNP;

    #ifndef REMIX_GLOBALS
        #define REMIX_GLOBALS

        enum Globals
        {
            //Anything above PACKET_FLOOD_LIMIT (1024 packets)
            //will disconnect the User.
            PACKET_FLOOD_TIME = 1000,

            //Users are able to send 1024 packets within PACKET_FLOOD_TIME.
            PACKET_FLOOD_LIMIT = 1024,

            //Minimum hex-SERNUM allowed. This is used to validate a hex-SERNUM.
            //Via: sernum & MIN_HEX_SERNUM
            MIN_HEX_SERNUM = 0x40000000,

            //Maximum time a User may remain inactive with "Disconnect
            //Idle Users" enabled. 10 minutes - 600,000 milliseconds.
            MAX_IDLE_TIME = 600000,

            //Maximum time a User may remain inactive before being marked with
            //an AFK icon. 5 minutes - 300,000 milliseconds.
            MAX_AFK_TIME = 300000,

            //Maximum amount of time in MS (30 seconds) to allow for the
            //Master to respond to a checkin.
            MAX_MASTER_RESPONSE_TIME = 30000,

            //Maximum amount of time in MS (5 Minutes)
            //before re-announcing to the Master Server.
            MAX_MASTER_CHECKIN_TIME = 300000,

            //Minimum amount of time in MS (30 Seconds)
            //before re-announcing to the Master Server.
            MIN_MASTER_CHECK_IN_TIME = 30000,

            //Amount of times an un-authenticated (admin) can use commands.
            MAX_CMD_ATTEMPTS = 3,

            //Amount of Players that are able to connect to the server.
            MAX_PLAYERS = 256,

            //Length of a Remote Admin's password salt.
            SALT_LENGTH = 12,

            //Maximum length annowed for a GameInfo String to be set and sent to
            //players that ping our servers.
            MAX_GAME_NAME_LENGTH = 256,

            //Count of possible logging types that ReMix uses.
            LOG_TYPE_COUNT = 11,

            //Count of Settings Keys Accessed via the
            //Settings/SettingsWidget class.
            SETTINGS_KEY_COUNT = 6,

            //Count of Rules Keys accesed via the Rules/RulesWidget class.
            SETTINGS_SUBKEY_COUNT = 28,

            //Count of Settings Sub-Kets Accessed via the Helper namespace.
            RULES_SUBKEY_COUNT = 16,

            //Count of currently supported Remote-Admin commands.
            GM_COMMAND_COUNT = 22,

            //Count of possible Sub-Commands each GM Command may posess.
            GM_SUBCOMMAND_COUNT = 10,

            //Count of Keys accessed via the User class.
            USER_KEY_COUNT = 13,

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

            //Count of currently accepted UPNP Schemas
            UPNP_SCHEMA_COUNT = 6,

            //Maximum wait time before the Server's UI is updated.
            //0.5 seconds in milliseconds.
            UI_UPDATE_TIME = 500,

            //Valid Count of usable "Themes".
            UI_THEME_COUNT = 2,

            //Valid count of colors the Theme class can use for certain
            //UI and Chat roles.
            UI_THEME_COLORS = 11,

            //Maximum time a User may remain connected after having a
            //"disconnect" initiated on their socket. (250MS|
            MAX_DISCONNECT_TTL = 250,

            //Count of Ban durations available to use when banning a player.
            PUNISH_DURATION_COUNT = 7,
        };

        //Valid Password types.
        enum class PwdTypes: int{ Server = 0, Admin = 1, Invalid = -1 };

        //Valid Remote Administrator Ranks.
        enum class GMRanks: int{ User = 0, GMaster, CoAdmin, Admin, Owner,
                                 Creator = 5, Invalid = -1 };

        //Valid Remote Administrator commands.
        enum class GMCmds: int{ Help = 0, List, MotD, Info, NetStatus, Ban,
                                UnBan, Kick, Mute, UnMute, Message, Login,
                                Register, ShutDown, ReStart, MKAdmin, RMAdmin,
                                CHAdmin, CHRules, CHSettings, Vanish, Version,
                                Camp, Invalid = -1 };
        //Valid Remote Administrator sub-commands.
        enum class GMSubCmds: int{ Zero = 0, One, Two, Three, Four, Five,
                                   Six, Seven = 7, Invalid = -1 };

        //Valid Command Structure Format.
        enum class CmdTblFmt: int { Cmd = 0, SubCommands, SubCommandCount,
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
        enum class DCTypes: int{ IPDC = 0, DupDC, PktDC = 2 };

        //Valid Theme ID's.
        enum class Themes: int{ Light = 0, Dark = 1 };

        //Valid Theme Colors
        enum class Colors: int{ Valid = 0, Invisible, Invalid, OwnerName, Name,
                                OwnerChat, Chat, Gossip, Shout, Emote,
                                GoldenSoul = 10, Default = -1 };

        //Valid columns within the PlrListWidget.
        enum class PlrCols: int{ IPPort = 0, SerNum, Age, Alias, Time,
                                 BytesIn, BytesOut, BioData = 7, ColCount = 9 };

        //Valid columns within the User Dialog.
        enum class UserCols: int{ SerNum = 0, /*Pings,*/ Calls, LastSeen, IPAddr,
                                  Rank, Muted, MuteDate, MuteDuration,
                                  MuteReason, Banned, BanDate, BanDuration,
                                  BanReason = 12, ColCount = 13 };

        //Valid columns within the Logger Dialog.
        enum class LogCols: int{ Date = 0, Source, Type, Message = 3, ColCount = 4 };

        //Valid Log types available to the Logger Class.
        enum class LogTypes: int{ ALL = 0, ADMIN, COMMENT, CLIENT, MASTERMIX, UPNP, PUNISHMENT, MISC, CHAT, QUEST, PKTFORGE, PING };

        //Used for converting time in seconds to a human readable format.
        enum class TimeFormat{ Hours = 0, Minutes = 1, Seconds = 2, Default = -1, HoursDiv = 3600, MinsDiv = 60, SecDiv = 60 };
        enum class MultiplyTime: int{ Hours = 60, Minutes = 60, Seconds = 60, Miliseconds = 1000 };

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

        //Valid Key values for use within the Rules and Settings Classes.
        enum class SKeys: int{ Setting = 0, WrongIP, Messages, Positions, Rules, Logger = 5, KeyCount = 5 };

        //Valid Sub-Key values for use within the Settings Class.
        enum class SSubKeys: int{ Extension = 0, AutoBan, AllowIdle, ReqSerNum, AllowDupe, AllowSSV, BanDupes, MOTD, LogComments, FwdComments,
                                  InformAdminLogin, EchoComments, MinimizeToTray, SaveWindowPositions, IsRunning, WorldDir, PortNumber, IsPublic,
                                  GameName, LogFiles, DarkMode, UseUPNP, CheckForUpdates, DCBlueCodedSerNums, LoggerAutoScroll, OverrideMasterIP,
                                  LoggerAutoClear, HasSvrPassword, SvrPassword, World, SvrUrl, AllPK, MaxPlayers, MaxIdle, MinVersion, PKLadder, NoBleep,
                                  NoCheat, NoEavesdrop, NoMigrate, NoModding, NoPets, NoPK, ArenaPK, AutoRestart, KeyCount };

        //Valid Toggles for the Settings Widget.
        enum class SToggles: int{ AllowDupeIP = 0, BanDupeIP, ReqSerNum, DCBlueCode, DCIdles, AllowSSV, LogComments, FwdComments, EchoComments,
                                  InformAdminLogin, MinimizeToTray, SaveWindowPositions, LogFiles, WorldDir, OverrideMaster = 14 };

        //Valid Toggles for the Rules Widget.
        enum class RToggles: int{ ServerPassword = 0, AutoRestart, WorldName, UrlAddr, AllPK, MaxPlayers, MaxIdle, MinVersion, Ladder, NoBleep, NoCheat,
                                  NoEavesdrop, NoMigrate, NoModding, NoPets, NoPK, ArenaPK = 16 };

        //Valid bases for use in converting strings to integers.
        enum class IntBase: int{ OCT = 8, DEC = 10, HEX = 16 };

        //Valid MessageBox types.
        enum class MessageBox: int{ SingleLine = 0, MultiLine = 1, Invalid = -1 };

        //Valid Units for conversion within the function Helper::sanitizeToFriendlyUnits
        enum class ByteUnits: int{ Byte,       // 1024^0,
                                   KibiByte,   // 1024^1,
                                   MebiByte,   // 1024^2,
                                   GibiByte,   // 1024^3,
                                   TebiByte,   // 1024^4,
                                   PebiByte,   // 1024^5,
                                   ExbiByte,    // 1024^6
                                 };
    #endif  // REMIX_GLOBALS

#endif // REMIX_PROTOTYPES_HPP
