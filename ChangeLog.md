TODO:
  * Change the IP re-selection to allow the User to select the active network interface and select the working IP address.
  * Implement other remote administrator commands and sub-commands.

Version 2.5.1:
    Change:
      * Implemented Mute durations.


    Bugfixes:
      * User Camp packets are no longer spammed when a new User connects to a server with Camped Users.
        * Camp packets are now only sent to the newly connecting User.




Version 2.5.0:
    Change:
      * Implement the "/info" command.
        * Sends basic information about the server's up time, users connected, and admins connected.
      * Implement packet deobfuscation code from the PacketForge module within ReMix itself.
      * Added the ability to utilize ReMote Administrator commands from within the regular chat box (bypassing /admin) on GameTypes supporting commands.
        * Commands entered in this way must begin with "  `  " (tilde). e.g. `ban soul 4000 30m Testing the new command!
        * All chat beginning with `(tilde) will be ommitted from further packet processing and/or re-sending.
      * The "/help" command now sends both command description and usage and no longer has a "format" sub command.


    Bugfixes:
      * Prevent Clients from forging a MasterMix response packet.
      * Prevent setting the GameInfo string within a ServerInstance's name. e.g. "ReMix Server [world=Evergreen]" is no longer possible.
      * Prevent comments from Users that have not yet sent their SerNum.
        * This is related to another bugfix from version [2.4.7].
      * Mark certain functions and constructors as overrides or defaults.
      * Properly cast certain UI events to their correct type.
      * Correctly inform Remote Administrators when they attempt to use a command they lack access to.




Version 2.4.9:
    Buildsystem:
      * Convert the ReMix project from Qbs to CMake.




Version 2.4.8:
    Change:
      * ReMix will now store and re-send packets with the OpCode( "F", Camp Initiated ).
        * This only affects the Well of Souls[WoS].


    Bugfixes:
      * ReMix will now attempt to correct a User's BIO data when they have pinged with a SerNum that does not match their current SerNum.
        * This is an attempt to correct an oversight left by a BugFix in version [2.4.7].




Version 2.4.7:
    Change:
      * Added the ability for ReMote Administrators to set a punishment duration when banning a User.
        * Example usage: /ban soul 4000 hours 24 *Reason


    Bugfixes:
      * Bans will now properly be removed when a banned User pings the server.
        * ReMote Administrators may now enact punishments on other Remote Administrators of a lesser rank.
      * Remove a SerNum check regarding UDP information due to ping/SerNum change collisions within the same household.




Version 2.4.6:
    Change:
      * Player Chat is now logged to file.
      * Player Sernums will now be colored Gold when using a Golden Soul.
        * Remote Administrators will remain as Green while authenticated and Muted Users will remain as Red.
      * Added Punishment Durations. Ranging from one day to one year, and permanent.
        * The durations can currently only be selected when manually Banning a User.
        * Remote Administrator bans are currently limited to 30 days from the time of banning.
        * Mutes are currently unaffected and are therefore permanent until reconnecting to the server.
      * Bans originating from ReMix versions prior to 2.4.6 will automatically be removed.
        * The Ban information is logged to file so that the Server Host may manually re-ban the User if so desired.
      * Remote Administrators must now authenticate with the server regardless of any previously set setting.
        * The Setting is no longer available, and is thus ignored.


    Bugfixes:
      * ReMix Server Instances will now only ping the MasterMix every 30 seconds after the initial check-in.
        * This is changed to every 5 minutes once a MasterMix response is received.
      * ReMix Server Instances will now correctly check-in with the "Warpath[W97]" MasterMix every 5 minutes.
      * ReMix will now correctly shift to a valid Server Instance when closing another.
        * Previously the Server Instance would retain UI elements pertaining to the closed Server Instance.
      * Users attempting to use commands outside of their rank status will now properly be warned and once again be given a command attempt countdown.
      * Unauthenticated Remote Administrators can now properly use all commands available to the rank "GmRanks::User".
        * Previously a Remote Administrator would be unable to use the command "/login".
      * Revoking and Reinstating a Remote Administrator from the Player List is now correctly handled.
        * Previously a Remote Administrator that had been demoted would be requested to re-register a password.
        * Previously a demoted Remote Administrator could attempt to login with their previous password. (This would not succeeed.)
      * Banning a User from the Player List will now properly request a Punishment Duration.




Version 2.4.5:
    Change:
      * Moved Server specific information from the main ReMix UI to a ToolTip that is shown when hovering over a Server tab instance.
      * Added various log messages to further inform the Server Host as to what is happening in the background.
      * The ChatView will now attempt to use the ingame name of a Player as opposed to just their sernum or alias.


    Bugfixes:
      * The Server Host will now be informed when the PacketForge Module is or is not able to be loaded.
      * Packets sent to a Server Instance using the game type "Arcadia" will now properly decrypt packets when the PacketForge module is loaded.
      * ReMix will now properly select the correct MasterMix server to correspond to the GameType selected via the CreateInstance dialog.
      * ReMix will no longer attempt to continuously check-in with the "Warpath[W97]" MasterMix server.
        * The "Warpath[W97]" MasterMix does not support responding to the Server Check-In packets.
          * This is due to the MasterMix version being much older and not really compatible.




Version 2.4.4:
    Change:
      * The threshold for disconnecting a player has been increased to 1024 packets within 1,000MS ( 1 Second ).
        * The previous threshold was 512 packets within 2,000MS ( 2 Seconds ).
      * Players will no longer be automatically banned for packet flooding. They will however be disconnected.
        * This is due to WoS scenes, and the ability to unintentionally fill a scene with 148 entities.
      * The Option within the Settings UI related to "Banning Deviants" has been removed, and will no longer have an effect within the Settings class.
        * This is related to the Packet Flooding change.


    Bugfixes:
      * Now correctly check if an IP address is Banned when the User's BIO Information is obtained.
        * Previously a User with Banned Information could connect to a ReMix server with only their SerNum changed.
      * When comparing and searching connected Players for Banned Information we will now skip the currently connecting SerNum for all ban times excluding SerNum bans.
        * Previously some Banned Information was not checked.




Version 2.4.3:

    Bugfixes:
      * Fixed a crash when recieving comments from players.
      * Duplicate SerNums will now correctly be disconnected with prejudice.




Version 2.4.2:
    Change:
      * Added a port randomizing button to the server creation dialog in an effort to be more intuitive.
      * Added more logging information from the UPNP class to the Logger UI.

    Bugfixes:
      * Fixed an oversight related to UPNP where the Modem or Router would only support certain lease times.




Version 2.4.1:
    Change:
      * Add a new Dialog to enable in-application viewing of Log events.
        * Note: Not all events of note are yet logged to this Dialog. Only events logged to file are currently shown.
      * Add the early base layout for a future WorldShuffler feature.
        * This feature will eventually allow the Host to host a server that will change the selected Game/ToY/World without intervention from the Host.

    Bugfixes:
      * Converted many variable types to their proper (assumed) types.
        * Note: Many of these variables were being incorrectly used and otherwise being truncated to a different type alltogether,
      * Converted many uses of '0' to use the proper value of 'nullptr'.




Version 2.4.0:
    Change:
      * Add more profanity words to the chat filter.

    Bugfixes:
      * Changed the packet flooding threshold to 521 packets within 2,000 milliseconds (2 seconds) up from 100.




Version 2.3.9:
    Change:
      * Added a third sub-command to the command "/vanish".
        * The added sub-command is "status" and the usage is "/vanish status" and will output the current visibility of the Admin for other users.
      * The "/shutdown" and "/restart" commands now have an aditional sub-command to stop an in-progress shutdown or restart.
        * The new sub-command is "stop" with the usage "/shutdown stop" or "/restart stop".
      * Add icons next to a User's SerNum to show if they have spoken within the last 5 minutes.
        * There are two states. One shows the User 'talking' and the other shows the User as being AFK.

    Buildsystem:
      * Update the Appveyor project file.

    Bugfixes:
      * The "/vanish" command now correctly functions as an on/off toggle when no sub-commands are used in conjunction with it.
        * Previously the command would show that the Admin was invisible while it was not correct.




Version 2.3.8:
    Change:
      * Create and implemented a Command Table class (CmdTable).
      * Restructured the Remote Administrator commands, and added functionality for several "Helper" commands.
        * Commands added include: Help, List, MotD.
          * Help shows the description or syntax of a command.
          * List shows all commands available to the user.
          * MotD allows the User to change the servers message of the day.
        * Various sub-commands have also been added but are not yet completely implemented.
          * Commands with new subcommands include Shutdown, Restart, and Vanish.
      * The server logs are now stored in the path "logs/logType/[yy-mm-dd]/logType.txt".
        * This allows for the User to more quickly browse to a certain log type for a specific day.
          * This also makes it more obvious to the User if the log exists for a certain day.
	  
    Bugfixes:
      * The World Selection UDP command will now check that the User setting the World information at least has a current connection to the server.
      * The Random Device class (RandDev) once again properly generates a sufficiently random number and no longer re-seeds upon every use.




Version 2.3.7:
    Change:
      * When creating a server the Create Instance dialog how has a checkbox to enable the ability to toggle port randomization.
        * Randomization only occurs when the User inputs '0' or makes the input field blank.
      * Add a new Setting toggle to disallow Users using a BlueCoded SerNum.
        * The Setting is not enabled by default and unless enabled the following SerNums will remain valid and able to connect.
          * 1004, 1024, 1043, 1046, 1052, 1054, 1055, 1062, 1068, 1072, 1099, 1112, 1120, 1123, 1125, 1138, 1163, 1166, 1170, 1172, 1173, 1189,
          * 1204, 1210, 1217, 1275, 1292, 1307, 1308, 1312, 1332, 1338, 1367, 1369, 1370, 1520, 1547, 1551, 1565, 1600, 1607, 1611, 1656, 1675,
          * 1681, 1695, 1706, 1715, 1751, 1754, 1840, 1965, 2003, 2058, 2062, 2144, 2205, 2217, 2264, 2268, 2359, 1008, 1017, 1051, 1054, 1082,
          * 1099, 1104, 1105, 1181, 1199, 1222, 1279, 1343, 1358, 1388, 1456, 1528, 1677, 1773, 1777, 1778, 1780, 1796, 1799, 2156, 2167, 2241,
          * 2248, 2362, 2421, 1098, 1220, 1264, 1342, 1361, 1823, 2302, 2488, 2585, 2372, 1492, 1576, 1100, 1347, 1050, 1015, 1666, 1745, 2043,
          * 1200, 2628, 1016, 1739, 1853, 2708, 2757, 1498, 2448, 2801, 1031, 1265, 1414, 1420, 1429, 1214, 1489, 1707, 2543, 1101, 1283, 1604,
          * 1428, 2707, 1023, 1069, 1071, 1132, 1286, 1854, 2910, 1005, 2682, 1348, 2615, 2617, 1884, 1169, 1540, 1645, 1939, 1179, 3053, 1803,
          * 2377, 1000, 1021, 1500, 1501, 1515, 1547, 1803, 2377, 3111, 3202, 3191, 3149, 3,

    Bugfixes:
      * When creating a server the Create Instance dialog will no longer automatically generate ports when the User removes all text from the input field.
      * When blocking an IP address from use ReMix will now properly refresh the network configuration and select another IP address.
      * The Remote Administrator commands "Kick", "Ban", and "Disconnect" will no longer affect the Command Issuer or other Remote Administrators.




Version 2.3.6:
    Change:
      * Disabled update checking.
        * The feature didn't appear to work for most Users.
        * Maybe one day someone will add it in, but I doubt it will be useful.
      * Add a basic chat filter for the ReMix chat viewer.
        * The feature relies on the server host having the rule "No Cursing" enabled.
      * The RandDev class is now a static class and the device member can be accessed globally.

    Bugfixes:
      * Removed two emit and slot functions related to Remote Administrator authentication.
        * This improves the speed at which the User receives the notification.
      * Change how the User's sernum is shown to the server host in various dialogs.
        * This primarily improves consitency between the various dialogs.




Version 2.3.5:
  Change:
    * Add a set of colors to the Theme class that are utilized in certain UI and chat elements.
    * Add global variables to indicate how many Themes ReMix can use, and how many custom UI colors the Theme class can use.
    * Add an enumerator class for use with the new theme color array used within the Theme class.
    * Add an implementation file for the Theme class.
    * Convert a few basic enumerators into a strongly typed enumerator class.
    * Enable the remote administrator command '/vanish' for remote administrators to appear as 'disappeared' or to sit on a server unnoticed.
      * The command also prevents Users from seeing Vanished administrators on the User-list.
    * The Settings class nolonger has control of the RulesWidget or the MOTDWidget classes.
      * Each class now stored the objects currently in use, and can be fetched or deleted through static functions.
      * These changes were made for the possibility of implementing the ability for remote administrators to change a server's rule set or message of the day.
    * The WorldSelection dialog, when used, will now allow the User to not select a World.
      * If the User previously had a world selected, a confirmation dialog will appear to confirm the removal.
      * If no world was previously selected by the User, the 'Ok' button will function as though the User had clicked 'Cancel'.
    * Add an enumerator class for the PlrListWidget class' table columns.
    * Convert the User enumerator to a enumerator class, and renameed it to 'UserCols'.
    * Add an enumerator for use in converting a numeric time to a user readable format. 'Hours:Minutes:Seconds'.
    * Add two Helper function to convert seconds into a time format using the TimeFormat enumerator.
      * Used for the Server UpTime, and the User connection times.

  Bugfixes:
    * When logging SerNum mismatches to file, we should be logging the encrypted packet instead of the unencrypted packet.
      * This is to ensure that any packet using the ':SR' format aren't being encrypted, and that we will know the packet at issue in it's original state.
    * The Settings and User Information dialogs will now close once the last server has been exited.
      * The previous behavior left these two dialogs open for view and interaction while the Create Server dialog was shown.
    * Prevent the string "world=" within the GameInfo string sent to Users when the Server information is requested.
      * This usually doesn't appear outside of "ReBreather" chat clients.
      * It's better to patch it in ReMix than bothering with the "ReBreather" sorce, though.
    * Properly remove the World rule when the option is unselected.
    * When renaming a Server instance, the process will no longer toggle all rules in a disabled state.
      * This was unintended and otherwise caused clutter that left the important rules 'hidden' to Users.

  Note:
    * These changes to the Theme Class are intended to preceed the implementation of a Theme-Color selector.




Version 2.3.4:
  Change:
    * Consolidate information regarding log file names into the Helper class for ease of use.
    * All logs will now be seperated into subdirectories based on the current date to more easily browse to events on a certain day.
    * All disconnects are now logged to file if the option to log files is enabled.
    * Move various enumerators related to Remote Administrator commands into the prototypes header.

  Bugfixes:
    * Properly validate settings before logging certain information to file.
      * Before, the setting "Settings::getLogFiles()" was not being checked when certain events occured.
    * Setting a SerNum no longer calls the validation function.
      * The call was redundant, as validation occurs before setting in any case.
    * All disconnects will now properly increment the UI indicator.
    * The function to log files now checks if the file should be logged.




Version 2.3.3:
  Change:
    * Re-enable the remote administrator commands to shutdown and restart a server.
      * When the last server is shutdown via a remote command, the ReMix instance will also close.
      * When the last server is restarted via a remote admin command, the Create Instance dialog will be skipped, and the server will be recreated using the previous settings.

  Bugfixes:
    * Servers when closed will now correctly have their indices shifted to retain the proper name and position within the server map.
      * The previous behavior would cause a server to take on the name and settings dialog of another active server.
    * Correctly convert the server usage information to strings.
      * The previous behavior of using the raw data would eventually cause a segfault.




Version 2.3.2:
  Change:
    * Permanently show the designated GameInfo( World Name ) as selected within the RulesWidget class.
      * Instead of disabling the shown GameInfo( World Name ) when all users are disconnected, we now show it on the server list at all times.

  Bugfixes:
    * Correctly format and present the GameInfo( World Name ) on server lists.
    * When the GameInfo( World Name ) is changed within the RulesWidget class, emit a signal to update the information within the ServerInfo class.
    * Correctly convert integers to strings within the UPNP class for logging to file.
    * When selecting the option to toggle the update checker the stored state is now checked to prevent an update check when the option is already toggled.




Version 2.3.1:
  Bugfixes:
    * Properly remove Users from the online User list when they are banned, kicked, or otherwise disconnected.




Version 2.3.0:
  Bugfixes:
    * Fixed a crash related to accessing a nonexistint row within the SettingsWidget table.




Version 2.2.9:
  Change:
    * Remove references to a User's port number from Log files.
    * Add setting toggle to enable checking for ReMix updates.
    * Add a basic ChangeLog file to the repository.

Import: 
  Bugfixes:
    * When changing a server's name, the input dialog will now properly have the old server's name pre-entered.
      * If accepting a name change with the old name, a warning message will be shown to the User.
	* ReMix will nolonger automatically attempt to update when the setting is not toggled.

  Buildsystem:
    * Merge the QtSparkle Update Checking library into the repository.




Version 2.2.8:
  Bugfixes:
    * Sanitize 'QString.arg()' format strings to (hopefully) prevent any formatting bugs from messages containing '%' format operands.
    * A ban date is now again properly added to the User dialog when a user is banned.




Version 2.2.7:
  Bugfixes:
    * ReMix will no longer attempt to disable and remove UPNP Port Forwards if it was never enabled for the server being closed.
    * ReMix will now ignore any GameInfo Strings being sent by Users if one has already been set via another User or from the Rules dialog.
    
  Buildsystem:
    * Add PacketForge.dll to AppVeyor deploy.




Version 2.2.6:
  Feature:
    * Add toggles to enable or disable UPNP instead of being enabled by default.

  Change:
    * A Server Instances UPNP status will now be saved to settings.




Version 2.2.5:
  Change:
    * Rename the ThemeChange class to Theme.
    * The Theme class now handles changing the font style and will now return the current theme value as required.
    * Add the RunGuard and Theme classes into the prototypes header.
    * Inform the Server Owner of the inability to fetch a server's Comments dialog.

  Bugfixes:
    * Change the value for an invalid Game within the Games enum. Games::Invalid now has a value of '-1'.
    * The PacketForge class will now again ignore both "SR?" and "SR!" packets and will no longer attempt to decrypt them.

  Buildsystem:
    * Debugging information will now be stripped from release mode.
    * Revert the project file to a state that does not build ReMix in a Debug mode.




Version 2.2.4:
  Change:
    * Add new Helper function "getTimeAsString" to assist with converting the current date to a string.
    * Mark many class member functions to be const and prevent unwanted editing of member variables.

  Bugfixes:
    * Helper::logToFile() will now again correctly append the timestamp and newlines.
    * Information is again correctly displayed in the User Dialog.



	
Version 2.2.3:
  Change:
    * Refactor many Class member functions to recieve const references for incoming variables.

  Bugfixes:
    * Attempt to properly set a GameID and send the information to the MasterMix.

  Buildsystem:
    * Remove Tab-Spacing from the embedded AppVeyor project file.




Version 2.2.2:
  Change:
    * Restructure how file includes are handled.

  ReadMe:
    * Add a simple ReadMe to the project.

  Buildsystem:
    * Automatically set the Version number based on the Commit-Tag in artifacts.
    * Embed the AppVeyor build status into the ReadMe file.
    * Embed a copy of the AppVeyor project file into the repository.




Version 2.2.1:
  Buildsystem:
    * Begin using the AppVeyor build system.




Version 2.1.16:
  Changes:
    * Reduce verbosity in string comparisons.



