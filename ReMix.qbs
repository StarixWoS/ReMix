import qbs

CppApplication
{
    consoleApplication: false
    property string ReMixVersionDisplay: '2.1.7'
    property string ReMixVersionMajor: '2'
    property string ReMixVersionMinor: '1'
    property string ReMixVersionPatch: '7'
    property string ReMixVersion: ReMixVersionMajor + '.'
                                + ReMixVersionMinor + '.'
                                + ReMixVersionPatch

    Depends { name: "cpp" }
    Depends { name: "Qt";
        submodules: ["core", "gui", "widgets", "network", "concurrent",] }

    //destinationDirectory: "libDir/"
    name: "ReMix-" + ReMixVersionDisplay + "-" + qbs.buildVariant

    cpp.includePaths: ["resources", "ui", "ui/widgets", "src", "src/tblview", "src/widgets"]

    cpp.defines: ["RC_VERSION=" + ReMixVersion.replace(/\./g, ",") + ",0",
                  "RC_VERSION_STRING=" + ReMixVersionDisplay,
                  "REMIX_VERSION=" + "\"" + ReMixVersionDisplay + "\"",].concat( base )

    cpp.cxxLanguageVersion: "c++14"
    cpp.debugInformation: false
    cpp.optimization: "fast"
    cpp.cxxFlags: "-s"

    files: [
        "src/prototypes.hpp",
        "src/includes.hpp",
        "src/tblview/usersortproxymodel.cpp",
        "src/tblview/usersortproxymodel.hpp",
        "src/tblview/plrsortproxymodel.cpp",
        "src/tblview/plrsortproxymodel.hpp",
        "src/tblview/tbleventfilter.cpp",
        "src/tblview/tbleventfilter.hpp",
        "src/widgets/settingswidget.cpp",
        "src/widgets/settingswidget.hpp",
        "src/widgets/remixtabwidget.cpp",
        "src/widgets/remixtabwidget.hpp",
        "src/widgets/plrlistwidget.cpp",
        "src/widgets/plrlistwidget.hpp",
        "src/widgets/userdelegate.cpp",
        "src/widgets/userdelegate.hpp",
        "src/widgets/remixwidget.cpp",
        "src/widgets/remixwidget.hpp",
        "src/widgets/ruleswidget.cpp",
        "src/widgets/ruleswidget.hpp",
        "src/widgets/motdwidget.cpp",
        "src/widgets/motdwidget.hpp",
        "src/createinstance.cpp",
        "src/createinstance.hpp",
        "src/appeventfilter.cpp",
        "src/appeventfilter.hpp",
        "src/packethandler.cpp",
        "src/packethandler.hpp",
        "src/packetforge.cpp",
        "src/packetforge.hpp",
        "src/selectworld.cpp",
        "src/selectworld.hpp",
        "src/cmdhandler.cpp",
        "src/cmdhandler.hpp",
        "src/serverinfo.cpp",
        "src/serverinfo.hpp",
        "src/chatview.cpp",
        "src/chatview.hpp",
        "src/comments.cpp",
        "src/comments.hpp",
        "src/settings.cpp",
        "src/settings.hpp",
        "src/randdev.cpp",
        "src/randdev.hpp",
        "src/sendmsg.cpp",
        "src/sendmsg.hpp",
        "src/server.cpp",
        "src/server.hpp",
        "src/player.cpp",
        "src/player.hpp",
        "src/helper.cpp",
        "src/helper.hpp",
        "src/rules.cpp",
        "src/rules.hpp",
        "src/remix.cpp",
        "src/remix.hpp",
        "src/user.cpp",
        "src/user.hpp",
        "src/upnp.cpp",
        "src/upnp.hpp",
        "src/main.cpp",

        "ui/widgets/settingswidget.ui",
        "ui/widgets/plrlistwidget.ui",
        "ui/widgets/remixwidget.ui",
        "ui/widgets/ruleswidget.ui",
        "ui/widgets/motdwidget.ui",
        "ui/createinstance.ui",
        "ui/selectworld.ui",
        "ui/chatview.ui",
        "ui/comments.ui",
        "ui/settings.ui",
        "ui/sendmsg.ui",
        "ui/remix.ui",
        "ui/user.ui",

        "resources/icons.qrc",
        "resources/ReMix.rc",
    ]
}
