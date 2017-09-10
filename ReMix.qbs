import qbs

CppApplication
{
    consoleApplication: false
    property string ReMixVersionDisplay: '2.1.9'
    property string ReMixVersionMajor: '2'
    property string ReMixVersionMinor: '1'
    property string ReMixVersionPatch: '9'
    property string ReMixVersion: ReMixVersionMajor + '.'
                                + ReMixVersionMinor + '.'
                                + ReMixVersionPatch

    Depends { name: "cpp" }
    Depends { name: "Qt";
        submodules: ["core", "gui", "widgets", "network", "concurrent",] }

    //destinationDirectory: "libDir/"
    name: "ReMix-" + ReMixVersionDisplay + "-" + qbs.buildVariant

    cpp.includePaths: ["resources", "ui", "ui/widgets",
                       "src", "src/tblview", "src/widgets"]

    cpp.defines: ["RC_VERSION=" + ReMixVersion.replace(/\./g, ",") + ",0",
                  "RC_VERSION_STRING=" + ReMixVersionDisplay,
                  "REMIX_VERSION=" + "\"" + ReMixVersionDisplay + "\"",]
            .concat( base )

    cpp.cxxLanguageVersion: "c++14"
    cpp.debugInformation: qbs.buildVariant == "debug" ? true : false
    cpp.optimization: qbs.buildVariant == "debug" ? "small" : "fast"
    cpp.cxxFlags: ["-Os", "-s",]

    Group
    {
        name: "HPP"

        fileTags: ["hpp"]
        files: [ "src/tblview/usersortproxymodel.hpp",
                 "src/tblview/plrsortproxymodel.hpp",
                 "src/tblview/tbleventfilter.hpp",
                 "src/widgets/settingswidget.hpp",
                 "src/widgets/remixtabwidget.hpp",
                 "src/widgets/plrlistwidget.hpp",
                 "src/widgets/userdelegate.hpp",
                 "src/widgets/remixwidget.hpp",
                 "src/widgets/ruleswidget.hpp",
                 "src/widgets/motdwidget.hpp",
                 "src/createinstance.hpp",
                 "src/appeventfilter.hpp",
                 "src/packethandler.hpp",
                 "src/packetforge.hpp",
                 "src/selectworld.hpp",
                 "src/themechange.hpp",
                 "src/cmdhandler.hpp",
                 "src/serverinfo.hpp",
                 "src/prototypes.hpp",
                 "src/includes.hpp",
                 "src/chatview.hpp",
                 "src/comments.hpp",
                 "src/settings.hpp",
                 "src/randdev.hpp",
                 "src/sendmsg.hpp",
                 "src/server.hpp",
                 "src/player.hpp",
                 "src/helper.hpp",
                 "src/rules.hpp",
                 "src/remix.hpp",
                 "src/user.hpp",
                 "src/upnp.hpp",
                ]
    }

    Group
    {
        name: "CPP"

        fileTags: ["cpp"]
        files: [
            "src/tblview/usersortproxymodel.cpp",
            "src/tblview/plrsortproxymodel.cpp",
            "src/tblview/tbleventfilter.cpp",
            "src/widgets/settingswidget.cpp",
            "src/widgets/remixtabwidget.cpp",
            "src/widgets/plrlistwidget.cpp",
            "src/widgets/userdelegate.cpp",
            "src/widgets/remixwidget.cpp",
            "src/widgets/ruleswidget.cpp",
            "src/widgets/motdwidget.cpp",
            "src/createinstance.cpp",
            "src/appeventfilter.cpp",
            "src/packethandler.cpp",
            "src/packetforge.cpp",
            "src/selectworld.cpp",
            "src/cmdhandler.cpp",
            "src/serverinfo.cpp",
            "src/chatview.cpp",
            "src/comments.cpp",
            "src/settings.cpp",
            "src/randdev.cpp",
            "src/sendmsg.cpp",
            "src/server.cpp",
            "src/player.cpp",
            "src/helper.cpp",
            "src/rules.cpp",
            "src/remix.cpp",
            "src/user.cpp",
            "src/upnp.cpp",
            "src/main.cpp",
        ]
    }

    Group
    {
        name: "UI"

        fileTags: ["ui"]
        files: [
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
        ]
    }

    Group
    {
        name: "RC"
        files: [ "resources/icons.qrc",
                 "resources/ReMix.rc",
                ]
    }
}
