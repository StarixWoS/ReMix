
#ifndef QT_PROTOTYPES_HPP
    #define QT_PROTOTYPES_HPP

//Required Qt Includes. --Unable to prototype in most instances.
//    #include <QElapsedTimer>
//    #include <QMainWindow>
//    #include <QModelIndex>
//    #include <QTcpServer>
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

    //GUI and Data Storage Classes.
    class CreateInstance;
    class AppEventFilter;
    class PacketHandler;
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
    class Theme;
    class Rules;
    class ReMix;
    class User;
    class UPNP;

    #ifndef REMIX_GLOBALS
        #define REMIX_GLOBALS

        enum Globals
        {
            //Anything above PACKET_FLOOD_LIMIT (2000MS)
            //will disconnect/ban the User.
            PACKET_FLOOD_TIME = 2000,

            //Users are able to send 100 packets within PACKET_FLOOD_TIME.
            PACKET_FLOOD_LIMIT = 100,

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

            //Minimum amount of time in MS (2 Seconds)
            //before re-announcing to the Master Server.
            MIN_MASTER_CHECK_IN_TIME = 2000,

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
            LOG_TYPE_COUNT = 8,

            //Count of Settings Keys Accessed via the
            //Settings/SettingsWidget class.
            SETTINGS_KEY_COUNT = 5,

            //Count of Rules Keys accesed via the Rules/RulesWidget class.
            SETTINGS_SUBKEY_COUNT = 27,

            //Count of Settings Sub-Kets Accessed via the Helper namespace.
            RULES_SUBKEY_COUNT = 16,

            //Count of currently supported Remote-Admin commands.
            GM_COMMAND_COUNT = 22,

            //Count of possible Sub-Commands each GM Command may posess.
            GM_SUBCOMMAND_COUNT = 4,

            //Count of Keys accessed via the User class.
            USER_KEY_COUNT = 12,

            //Time interval for updating the server's Usage array.
            SERVER_USAGE_UPDATE = 600000,

            //Size of the Server Usage array corresponding to 48 hours.
            SERVER_USAGE_48_HOURS = 288,

            //Maximum Server Count a User can have at any time.
            MAX_SERVER_COUNT = 19,

            //Count of possible Game Names we can use to initialize the server.
            GAME_NAME_COUNT = 3,

            //Maximum live-time for a UPNP PortForward to live.
            //1 Hour in seconds.
            UPNP_TIME_OUT_S = 3600,

            //Maximum idle-time for a UPNP PortForward exist without refreshing.
            //30 Minutes in milliseconds.
            UPNP_TIME_OUT_MS = 1800000,

            //Count of currently accepted UPNP Schemas
            UPNP_SCHEMA_COUNT = 6,

            //Maximum time before the Baud information
            //must be displayed to the User. 5 seconds in milliseconds.
            BAUD_UPDATE_TIME = 5000,

            //Maximum wait time before the Server's UI is updated.
            //0.5 seconds in milliseconds.
            UI_UPDATE_TIME = 500,

            //Valid Count of usable "Themes".
            UI_THEME_COUNT = 2,

            //Valid count of colors the Theme class can use for certain
            //UI and Chat roles.
            UI_THEME_COLORS = 10,
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
                                Invalid = -1 };

        //Valid Command Structure Format.
        enum class CmdTblFmt: int { Cmd = 0, SubCommands, SubCommandCount,
                                    CmdInfo, CmdSyntax, CmdRank,
                                    CmdIsActive = 4 };

        //Valid SerNum response codes.
        enum UserListResponse{ Q_Response = 0, R_Response = 1 };

        //Valid Game ID's that ReMix can call to.
        enum class Games: int{ WoS = 0, ToY = 1, W97 = 2, Invalid = -1 };

        //Valid methods ov banning Users.
        enum class BanTypes: int{ SerNum = 0, IP, DV, WV = 3 };

        //Valid forms of diconnecting Users.
        enum class DCTypes: int{ IPDC = 0, DupDC, PktDC = 2 };

        //Valid Theme ID's.
        enum class Themes: int{ Light = 0, Dark = 1 };

        //Valid Theme Colors
        enum class Colors: int{ Valid = 0, Invisible, Invalid, OwnerName, Name,
                                OwnerChat, Chat, Gossip, Shout, Emote = 9,
                                Default = -1 };

        //Valid columns within the PlrListWidget.
        enum class PlrCols: int{ IPPort = 0, SerNum, Age, Alias, Time,
                                 BytesIn, BytesOut, BioData = 7, ColCount = 9 };

        enum class UserCols: int{ SerNum = 0, Pings, Calls, LastSeen, IPAddr,
                                  Rank, Banned, BanDate, BanReason = 8,
                                  ColCount = 9 };

        //Used for converting time in seconds to a human readable format.
        enum class TimeFormat{ Hours = 0, Minutes, Seconds, Default = -1,
                               HoursDiv = 3600, MinsDiv = 60, SecDiv = 60 };

    #endif  // REMIX_GLOBALS

#endif // REMIX_PROTOTYPES_HPP
