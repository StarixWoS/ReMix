
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
    class QDebug;
    class QEvent;
    class QPoint;
    class QChar;
    class QFile;
    class QMenu;
    class QDir;
    class QUrl;

#endif // QT_PROTOTYPES_HPP

#ifndef REMIX_PROTOTYPES_HPP
    #define REMIX_PROTOTYPES_HPP

    namespace Helper
    {}

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
    class PacketHandler;
    class SelectWorld;
    class PacketForge;
    class ServerInfo;
    class CmdHandler;
    class ChatView;
    class Settings;
    class Comments;
    class RandDev;
    class SendMsg;
    class Player;
    class Server;
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

            //Count of Settings Keys Accessed via the
            //Settings/SettingsWidget class.
            SETTINGS_KEY_COUNT = 5,

            //Count of Rules Keys accesed via the Rules/RulesWidget class.
            SETTINGS_SUBKEY_COUNT = 22,

            //Count of Settings Sub-Kets Accessed via the Helper namespace.
            RULES_SUBKEY_COUNT = 16,

            //Count of currently supported Remote-Admin commands.
            ADMIN_COMMAND_COUNT = 18,

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
            //30 Minutes in seconds.
            UPNP_TIME_OUT_S = 1800,

            //Maximum live-time for a UPNP PortForward to live.
            //30 Minutes in milliseconds.
            UPNP_TIME_OUT_MS = 1800000,
        };

        enum Games{ WoS = 1, ToY = 2, W97 = 3, Invalid = 4 };
        enum UserListResponse{ Q_Response = 0, R_Response = 1 };
        enum Themes{ LIGHT = 0, DARK = 1 };

    #endif  // REMIX_GLOBALS

#endif // REMIX_PROTOTYPES_HPP
