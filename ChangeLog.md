TODO:
  * Implement other remote administrator commands and sub-commands.
  * TODO: Give more meaningful names to the settings stored within the preferences.ini -- Break away from Dan's short-hand naming scheme.
    * This would break settings compatibility for the added benefit of being able to manually edit the preferences while ReMix is closed.
  * Add a Settings Tab to customize the ChatView settings.

Version 3.1.4:
    Change:
      * 





    Bugfixes:
      * Fixed an issue where the CreateInstance dialog would not appear when starting ReMix when compiled with Qt6.7.
      * Fixed an issue with the Light/Dark theme when compiled with Qt6.7.
      * Fixed an issue where ReMix would keep a User "online" when the Network Socket had failed or otherwise disconnected.





Version 3.1.3:
    Change:
      * 





    Bugfixes:
      * Fixed an issue that made "Owner Character Emulation" nonfunctional.
      * Fixed an issue with the MasterMixThread related to closing a Server Instance where more than one Server was connected to the same GameID.
        * e.g. ServerA and ServerB connected to "WoS". Closing ServerA would prevent ServerB from recieving updated MasterInformation for "WoS" as the MasterMixThread would have disconnected from "WoS".





Version 3.1.2:
    Change:
      * Added a new Dialog "Admin Command Overrides" that will allow Server Hosts to override the ranks of Remote Admin Commands and SubCommands.
        * Once a Command's rank has been changed, ReMix will default to using the "CommandRanks" list stored within the "preferences.ini".
      * Expanded functionality to the "help" command that will allow more detailed syntax information fot Commands and SubCommands.
        * e.g. the command "/help message all" will now have a description specifically for the SubCommand "all".
          * "Command Description: Messages *ALL* Users."
          * "Command Usage: /msg all *Message"
      * The command "list" now has it's output filtered based on the command ranks available to the Remote-Admin.
      * Remote-Admins will now be informed when they attempt to use a SubCommand that they lack the rank to access.
        * e.g. the command "/motd remove" when the rank is insufficient will give the error:
          * "Error: You do not have access to the command [ remove ]. Please refrain from attempting to use commands that you lack access to!"
      * The default ranks for Remote Admin Commands and Sub Commands are now synchronized between the classes "CmdTable" and "CmdTableOverride".
      * The UPnP Class has been completely re-written.
        * The rewritten UPnP class now properly scans all UPnP devices and services, and seeks out any valid "InternetGateway" services.
        * While the log output will be less verbose, the feature will hopefully succeed in creating a port forward for more Users.






    Bugfixes:
      * Fixed an issue with the "camp" command when using "camp allow soul ####" would be treated as a Remote Admin command.





Version 3.1.1:
    Change:
      * Users detected violating the "noMod" and "noCheat" rules while the setting "Strictly Enforce Rules" is enabled will no longer be disconnected.
        * These users will now be Quarantined and may only interact with other Quarantined Users.
      * Added a UI control within the PlrListWidget to facilitate a manual override of a User's Quarantined State.
      * Added Remote-Admin commands to facilitate quarantining and unquarantining Users.
        * Usage: "/quarantine soul 4000 Good behavior" and "/unquarantine soul 4000 Good behavior"
      * When Remote-Admins authenticate they will now recieve information on how many Users are Muted and Quarantined.
      * Expanded the Remote-Admin command "Info" to faciliate viewing of Muted and Quarantined Users.
        * The added sub-commands are "muted" and "quarantined". Usage: "`info muted" and "`info quarantined"
          * This is added so that Remote-Admins can "see" which Users have a punishment applied and can remove them if desired.
      * The Remote-Admin command "`info server" will now display the Peak User Count, Muted User Count, and Quarantined User Count.
        * Full output: "Server Info: Up Time [ 00d:00h:00m:27s ], MasterMix Ping [ 106 ms, Avg: 101.5 ms, Trend: 84.13 ms ], Users [ Current 1 / Peak 1 ], Muted [ 0 ], Quarantined [ 0 ], Admins [ 1 ]."






    Bugfixes:
      * 





Version 3.1.0:
    Change:
      * Properly inform Users when a Mute punishment is applied or removed.
        * This also applies to loading punishments from storage when the User connects.
      * Messages sent to Users will utilize the ":SR@M" packet for the "SERVER MESSAGE" output.
      * Rows within the Log View can now be de-selected by clicking a previously selected row.
      * ReMix no longer considers a user leaving as an IP Disconnect which is considered an administrative action.
      * ReMix will now "rename" characters that attempt to impersonate the "Owner" character.
        * The name is not permanent, and only modifies the name packet sent to other Users.
        * The name is changed to "Owner Impersonator".
        * This will not happen if the "Owner Character Emulation" setting is disabled.
      * Usage of the "Owner" character emulation is now an opt-in setting via the Settings UI.
        * If this setting is disabled, the "Server Message" toggle within the "Chat View" UI is removed.
      * Admins that disconnect while in the "Vanished State" will now have that state saved to file.
        * Upon reconnecting, the "Vanish State" will be restored from file and the Admin will have a 5 minute grace period to authenticate with the server before becoming visible.
      * Added a command for Remote Admins to de-authenticate with the server.
        * "/logout"
      * Simplified the collation of commands available to a User/Admin sent via the "/list" command.
      * De-coupled the settings for disconnecting Idle and AFK Users.
        * The Idle disconnect is not configurable and should only matter when a connection is inactive.
        * The AFK Disconnect *is* configurable, and only matters *after* a User is detected going AFK via a packet identifier.
          * In doing this, the setting is now properly named "maxAFK" (previously, "maxIdle") which the Syn-Games support.






    Bugfixes:
      * Fixed an issue where server admins in an invisible state would cause ReMix to send an incorrect online User count in response to a ping.
      * Fixed an issue where ReMix would log and inform a User of an Un-Mute twice when manually un-muting the User from the User Information UI.
      * Fixed an issue where manually Muting a user from the User Information UI would not generage a log-event within the Punishment log.
      * Fix: The LogView will once again automatically scroll to the bottom when the UI Toggle is checked.
      * Fixed an issue where ReMix would continue sending Master Check-In's while doing a Master Mix Information Refresh.
        * Servers now properly have their check-in timers stopped until the refresh is completed.
        * This is to prevent checking in to a MasterIP that is no longer valid whilst obtaining the valid MasterIP.





Version 3.0.9:
    Change:
      * 





    Bugfixes:
      * Fixed an issue related to Server Side Variables (SSV) where a maliciously formatted packet could allow directory traveral on the Host Machine.
        * This at most could lead to reading and writing to certain ".ini" files, if the attacker could get a grasp on the Host's Filesystem.
        * We now sanitize the SSV Path to ensure that this cannot happen.
        * We now ensure that the SSV is being set for the World that the Server Instance is actually using.
      * Fixed an issue Users could be disconnected when the Rule "enforceRules" was enabled while the corresponding Rule being enforced was not checked.





Version 3.0.8:
    Change:
      * Chat messages sent to Users via the Chat View will now appear as normal "Player" chat.
        * Chat messages can now be toggled between a Red (urgent) message, or a normal *player* message via a CheckBox toggle within the ChatView.
      * ReMix now emulates a "Player" for the WoS Game Type.
        * A character named "Owner" will appear to Users connected to WoS.
        * The emulated character cannot be interacted with.
      * Changed how the LogView handles filtering incoming logs.
        * Hopefully this change will allow the logView to be accessed and filtered with a lower latency.
          * The old method for certain would cause UI latency issues once ReMix had been online for an extended period.
      * Simplified the RunGuard class and reduced the scope of what it tries to do.
        * This should allow for the RunGuard to once again operate on Linux and OSX.
      * Corrected the order of the "User List" pings.
        * The request is now correctly logged before the response.
      




    Bugfixes:
      * Fixed an issue with the "vanish" command.
        * The PlayerCount sent to Ping Requests would be set to erroneous values when an admin in vanish mode would disconnect from ReMix.
      * Fixed an issue where Admins in vanish mode were unable to communicate via the ingame chat boxes.
      * Fixed an issue related to Punishment Durations and erroneously logging a completed penalty.
      * Fixed an issue where Mute Durations being removed while a User was connected would not be logged.





Version 3.0.7.2:
    Bugfixes:
      * Attempted a fix with UPNP where the Control URL as provided by a Router/Modem is invalid.
        * An example of an erroneous ControlURL: <controlURL>control?WFAWLANConfig</controlURL>
		  * When such an example is found, the URL will be formatted as such: http://192.168.1.1:1990/control?WFAWLANConfig (RouterIP:RouterPort/ControlURL)




Version 3.0.7:
    Change:
      * When using the UI splitters to hide the Server Buttons (Settings, User Information, Log View, ect) ReMix will now save that state (per server).
      * When using the UI splitters to resize the Chat View and Player View ReMix will now save that state (per server).
      * Partially cleaned up the UPNP log output when validating Control Schemas.
      




    Bugfixes:
      * Attempted a fix with UPNP where the Control URL as provided by a Router/Modem is invalid.
        * As I lack the hardware to test this, I can only assume that the Control URL is being properly formatted within the fallback method.
        * An example of an erroneous ControlURL: <controlURL>control?WFAWLANConfig</controlURL>
		  * When such an example is found, the URL will be formatted as such: http://192.168.1.1:1990/uuid:84f2572c-59bd-aa7f-1747-bd89603cb789/control?WFAWLANConfig (RouterIP:RouterPort/RouterUUID/ControlURL)
          * As stated, this may be incorrect and the proper formatting could actually be: http://192.168.1.1:1990/control?WFAWLANConfig (RouterIP:RouterPort/ControlURL)




    Buildsystem:
      * Add support for sub versions to be derived from the RePo Tag Name.
        * e.g. ReMix verison 3,0,7,1 would be the first release of verison 3,0,7. with the second release being 3,0,7,2.




Version 3.0.6:
    Bugfixes:
      * Fixed an issue with the ChatView UI where messages and information would be written into more than the intended view.
        * e.g. Server A's messages would appear in both Server A's and Server B's ChatView UI.
      * Fixed an issue with Remote Admin Commands.





Version 3.0.5:
    Change:
      * Reduce spammy chat for connected Users when using the ReMix ChatView to send messages.
        * Users using actual game clients will no longer receive the "MESSAGE FROM SERVER" tags when sending messages to Users via ReMix.
        * ReMix chat is sent via the actual chat packet (C) using the sernum (negative)-1234 in hex 0xFFFFFB2E.
          * As an exception, messages sent via Remote Admin Commands currently still prompt with the "MESSAGE FROM SERVER" tags and may be changed in the future.
      * As ReMix now uses the sernum 0xFFFFFB2E for chat packets, we now also send heartbeat packets (0) for the sernum to prevent Users from seeing it "disappear".
      * Reduce verbosity in code when accessing the underlying type of enum values.
        * static_cast<int>( Globals::VALUE ) reduced to *Globals::VALUE.
      * ReMix now removes IP information from (WoS) Skin Transfer (x) packets.
        * This *should force WoS to fallback to using the Mix to communicate the skin data to other Users and allow the feature to work for more people.
      * The packet MIX6 is now fully and completely deprecated. WoS will no longer acknowledge or respond to Mix commands starting with "/cmd".
      * Packet forging is now fully implemented for both WoS and ToY game types.
      




    Bugfixes:
      * Fixed an issue where right-clicking a server-instance would not properly allow the instance to be closed or renamed.
      * Fixed an issue where re-opening a previously closed server instance would cause ReMix to freeze until forcefully closed.
      * Fixed an issue where Server threads (UdpThread) would not close or exit properly.
        * We now forcefully end it instead of waiting for a obj.deleteLater().
      * Fixed an issue where starting a server-instance using the GameType(WoS, ToY, W97) of another instance would force all servers on that GameType to ping the MasterMix.
        * Server instances will now only re-ping the MasterMix if the provided information for that GameType has changed.
      * Fixed an issue on Windows11 using >Qt6.4 where ReMix would no longer properly switch between the dark and light themes.
      * Fixed an issue with the ChatView where only the first server instance created would have it's text colors changed based on the dark and light themes.
        * All other servers created would use the light theme text color regardless of theme picked.
      * Fixed an issue where ReMix would no longer download the MasterMix data (synreal.ini) when using >Qt6.4.
      * Fixed an issue where ReMix would no longer ping the MasterMix when using >Qt6.4.





Version 3.0.4:
    Change:
      * Renamed a Helper function to correct name-case. serNumtoInt -> serNumToInt.
      * Expanded the CommandTable to support aliasing command names.
        * Aliased command names will appear within the "/list" printout as completely seperate commands.
      * Users will no longer have access to the help or usage information of commands that they lack access to use.
        * The User will now receive a message informing them of this.
      * The "Shutdown" command has had it's rank limitation elevated from "Admin" to "Owner".
      * Sorting the "Last Seen" column within the User dialog is now sped up significantly for large lists.
        * We are now sorting via the actual time-code integer as-is instead of converting the human readable string into it's original time-code.
      * Slightly changed how Admin Ranks are applied to Users from the UI, and should over-all be more consistent.
      




    Bugfixes:
      * Fixed an issue where ReMix would crash when attempting to parse a zero-width (empty) packet.
      * Fixed an issue where Muted Users on WoS server instances would have their camp packets forwarded to other Users.
      * Fixed an issue where closing a server instance would crash ReMix.
        * This was caused by improperly closing QThread objects and deleting the parent object before the thread had actually closed.





Version 3.0.3:
    Change:
      * Increased Compatibility with the SynGame Arcadia[ ToY ].
        * ReMix will now correctly identify on the MasterMix server list which minigame is being played.
      * Split the PacketHandler's parsing of packets into dedicated sub-classes for each suppoerted SynGame.
      * ReMix will now disconnect any User that has not sent a valid, non-zero SerNum within 5 Minutes of the Player object being created.





    Bugfixes:
      * Fixed an issue where Server Instances with the "Auto Restart" rule enabled would not correctly restart.
      * Fixed an issue where a User would remain connected in an invalid state as SerNum 0.
        * This would cause any new Users to be disconnected as a "Duplicate SerNum".
      * Fixed an issue within the WoS and ToY packet handlers related to sending and ignoring packets.





Version 3.0.2:
    Change:
      * Properly use Qt's "QInputDialog::getInt" when obtaining integers within the Rules Settings tab.
      * Log when a Player Object or Server Object is deconstructed within the MISC log file.
      * Log when the MaxterMixThread disconnects from a GameType signal/slot.
      * Added more Icons for use within the PlrListWidget.
        * We now set the various Icon images based on more Soul states.
      * We now detect when a User is a Player Killer.
        * This may be useful information if the Server Instance is using the "No Pking" Rule
        * To DO: Strictly enforce the "No PKing" rule within ReMix. Do not trust Users to adhere to the rule.
      * Added a new Rule that enables ReMix to Strictly Enforce rules that are usually trusted to be followed by it's connected Users.
        * When enabled ReMix will Disconnect Users when they attempt to join a "Locked" party.
          * This isn't really a rule, but it's something that can be enforced when Strict Rules is enabled.
        * When enabled alongside the "noPet" rule, ReMix will disregard any packets calling a Pet.
          * This does not disconnect the User, and may be subject to change.
        * Enabling this Rule will allow ReMix to disconnect Users breaking the following rules.
          * noPK, allPK, noCheat, noMod, noMigrate, and minV (version).
      * Added two more Icons for use within the PlrListWidget.
        * This include the Cheater, Modder, and Muted icons.
          * The Muted icon is currently not used.
      * Renamed the Rules within the Rules Widget UI to be somewhat more descriptive in their function.




    Bugfixes:
      * Corrected the spacing between a User's sernum and emote chat text.
      * Attempt to negate a crash within the QPixmapCleanup code (Qt runs this automatically.) by removing the AFK/NPK icons from the Player class.
        * The icon images are now only accessed via the PlrListWidget class.
      * Reduced the Max Players count from 256 to 255.
        * This is due to the Packet Slot Positions being unable to exceed 255 when converted to Hex (0xFF).
      * Fixed another issue where disabling and enabling the "Public" toggle would not correctly restart the MasterMix check-in timers.
      * Fixed an issue where Server Instances with the "Auto Restart" rule enabled would not correctly restart.
      * Fixed an issue where a User would be shown as returning to a lobby despite having never fully connected.




Version 3.0.1:
    Change:
      * Expanded the information sent in response to the Remote Admin command "/info server".
      * Properly parse and customize more chat packet types.




    Bugfixes:
      * Remote Administrator commands are no longer parsed if the command delimiter is followed by a space.




Version 3.0.0:
    Change:
      * Initial branch from develop_stable.
      * Converted the Player and Server class objects to Smart Pointers (QSharedPointer<Server> and QSharedPointer<Player>).
        * Corrected some logic related to the usage of Smart Pointers in order to increase reliablity and stability.
      * Reduced the complexity of the MasterMixThread class.
      * Added overloads for "User::addMute" and "User::addBan" for use in manual actions.
      * Messages sent to specific Users via the SendMsg dialog/class will now be reflected within the Chat View.
      * Explicitly map enum values to their string counterparts.
      * Converted the UserKeys enum to a global Class Enum.
      * Messages sent via the Remote Administrator command "/msg" are now shown within the Chat View.
      * Properly disconnect Game Types within the MasterMixThread Class.
      * Added color customizations for Admin Chat, Admin Names, and Admin Messages (Sent via the `msg command).
      * Expanded the information sent in response to the Remote Admin command "/info server".
      * Properly parse and customize more chat packet types.

      



    Bugfixes:
      * Fixed a possible crash related to the Server class deconstructing while the UdpThread continued to run.
      * Fixed a possible crash related to the MasterMixThread class.
        * The crash was related to a dangling pointer to an invalid Server instance.
      * Fixed an issue when closing a Server Instance related to the AutoRestart rule.
        * We now prompt the User if they wish to remove ther AutoRestart rule before closing the Server instance.
        * If the User Accepts the Rule Removal and Declines the Server closure the AutoRestart rule remains activated.
      * Fixed an issue where disabling and enabling the "Public" toggle would not correctly restart the MasterMix check-in timers.
      * Fixed an issue where the MasterMixThread class would have multiple connections for individual GameTypes.
        * This resulted in the Information being parsed more often than required.
        * The MasterMixThread class now maps which game types have been used.
        * Moniter which GameTypes are still in an active state.
      * Remote Administrator commands are no longer parsed if the command delimiter is followed by a space.




Version 2.8.0:
    Change:
      * Refined the ColorView widget UI to be more refined.
      * Users can now change Player ranks (Remote Administrator roles) via the PlayerList View's context menu.
        * Previously it was only possible to completely remove the Player rank.
      * Correctly prevent the sysop SerNum[ 0x4637B6ED ] from connecting to ReMix.
        * This SerNum will be unable to connect unless it is connecting from the MasterMix IP address.
      * Deprecated the command syntax [ /cmd *cmd ].
        * The currently supported syntaxes are now: [ `command ] and [ /command ].
          * [ /command ] can only be used within the WoS[ /admin ] dialog.
          * [ `command ] can be used via normal chat in all game types, and within the WoS[ /admin ] dialog.
      * The Max Player rule is now correctly enforced.
      * Reduced the complexity of Inserting and Updating data within the PlayerView widget.




    Ported Changes:
      * develop_unstable, ReMix 3.0.0
        * Reduced the complexity of the MasterMixThread class.
        * Fixed a possible crash related to the MasterMixThread class.
          * The crash was related to a dangling pointer to an invalid Server instance.



    Bugfixes:
      * Fixed a memory leak related to the WorldSelect dialog.
        * The world select dialog is only used when a World Directory has been set within the Settings.
      * Fixed a possible crash related to logging User BIO data to file via the User class.
        * This has been changed to a signal/slot in order to be thread-safe.
      * Fixed an issue where Invisible Remote Administrators were unable to use the alternative command syntax.
      * Fixed an issue where Invisible Remote Adminsitrators when camped would be forced to send a Camp packet to a newly connected Player.
      * Fixed an issue where ReMix would send an invalid SR slot to Warpath Players.
      * Fixed the usage description for the command [ /version ].
      * Fixed an issue where Warpath chat was not correctly handled within the ChatView.




Version 2.7.9:
    Change:
      * Removed the Comments Dialog.
        * Comments send to ReMix will now appear within the ChatView Widget.
      * Added a TimeStamp to the ChatView, prepended to any chat or comments.
      * Added a toggle to the ChatView Widget to autoscroll text as it's sent to the text view.
      * Added the Ability to hide the PlayerList or ChatView widgets.
        * Only one widget may be hidden at a time per Server Instance.
        * The Hidden/Shown state is saved as a Server specific setting.
      * Removed UDP Packet handling from the PacketHandler Class and into the UDPThread Class.
        * The UDP Thread Class signals the parsed data to the Server Class for further proccessing of Server specific information.
      * Reduced Code Bloat related to copying Server Settings/Rules in a NameChange event.
        * In the process it has become easier to mark which settings/rules are server specific.
      * Properly parse the '3', '5', 'k', and 'p' packets for chat related information.
        * The '3' Packet Contains incarnation/disincarnation/and ghost messaging.
        * The '5' Packet signals a player is leaving the server.
        * The 'k' Packet contains PK Attack information.
        * The 'p' packet handles Party join/leave information.
      * Removed files from that are no longer used.
      * Reduced code bloat related to swapping the ChatView colors when changing the Theme from Normal/Dark.
      * Added a Day field to the PlayerOnline and Server Instance timecodes.
        * Previously we only tracked how many hours, minutes, and seconds a User or Instance has been active.
        * e.g. 751:32:14 will now read as 31d:07h:32m:14s
      * Repurposed the [ Blocked IP ] function into one where the User could select an IP Interface for ReMix to use.
        * This is Server specific and is not saved as a setting.
        * This is accessed by clicking the Blue IP address within the main ReMix UI.
        * The initial IP Addressed used is selected via the Create Instance dialog.
      * Added a Color Selection UI accessed from the Settings dialog to enable customization of CHat/UI colors used to display information.
        * The customized colors are saved within the "preferences.ini" under the Key [ Colors ];
      * Added a button used to clear the Chat View.
      * Converted the RandDev Class to use the "QRandomGenerator" classes instead of the std library.
      * Added a new setting to allow censoring the IP Addresses visible within the Main ReMix UI.
        * IP addresses within the User or Logger dialogs are not censored.
        * This setting is to allow taking screenshots of the Main UI without inadvertently sharing another persons IP (Not that it's important.).
      



    Bugfixes:
      * Fixed an issue where right-clicking a Server Tab would popup the incorrect context menu, or no menu at all in some cases.
        * This was due to the QTabBar::tabAt() function having no knowledge of the NewTab Button.
        * Reimplemented the tabAt() function to account for this buttton's size within the ReMixTabWidget Class.
      * Fixed an issue where applying the Normal/Dark themes would not correctly theme the LogView UI.
        * The workaround for this was to have the Theme Class signal the Theme change directly to the LogView Class and applying the pallette directly.
      * Fixed an issue where Window Positions were not being saved.
      * Fixed an issue where ReMix would send a MasterMix ping even when the Server was not public.
        * This was due to how ReMix handles connecting players and updating the Player Count to the Master Mix.
        * We now properly ensure that the Server is Public before posting information to the Master.
      * Fixed an issue where changing a Servers name would not copy certain settings and rules.
      * Fixed an issue where Users initializing a custom command would have a TimeStamp with no message inserted into the ChatView.
      * Fixed an issue where changing the Theme to Normal/Dark would not swap colors correctly.
      * Fixed an issue where ReMix would freeze when canceling the CreateInstance dialog while no Server instance is active.
        * The corrected behaviour is that ReMix will now properly close.
      * Fixed an issue when Reading an SSV for a User would not have the returned value sent to the LogView.
      * Fixed an issue where an Automatic Ban would cause UI elements to appear linked to an invalid Player Object.
      * Fixed an issue where manually re-selecting an IP Interface would send multiple MasterMix checkings.
      * Fixed an issue where when the UPNP feature is enabled ReMix would not send a MasterMix checkin.
        * The UPNP feature now has a 5second timeout to allow for sending the MasterMix checkin regardless of a UPNP success.




Version 2.7.8:
    Change:
      * Enabled changing the Light/Dark theme without requiring the User to restart ReMix.
      * The New Tab button is now themed more appropriately with the selected Theme type.
      * Added a context menu to the ReMixTabWidget.
        * When right-clicking a Tab the User will be given the option to Rename or Close a server instance.
        * Double-clicking a tab will no longer prompt the User to rename the Tab.
      * Merged the Server Class into the ServerInfo Class, and renamed ServerInfo to Server.
        * Reduced confusion over which "Server" object was being used.
      * Removed all direct uses of the PlrListWidget object from the Player and ReMixWidget classes.
        * The UI is now updated via signals and slots, but more safe.
        * Neither class should have ever been accessing UI elements directly.
      * Converted the ChatView dialog into a Widget.
        * The Chat View is now shown alongside the Player View.
        * The User is able to now hide the Player View or the Chat View.
          * One view must remain option. When hiding one View the other Button is disabled.
      * Moved packet related code from the ChatView Class into the PacketHandler Class.
        * The Chat View should never have had this code within.
      * The PlrListWidget, PacketHandler, CmdHandler, and the ChatView are now accessed via "Class::getInstance" function calls.
        * This is to reduce the amount of naked pointers being shared and stored within ReMix.
      * The Theme class now Signals to the ChatView class when changing theme types.
        * This allows us to dynamically change the text colors of chat previously submitted to the ChatView.
      



    Bugfixes:
      * Fixed an issue where an empty Context Menu could appear when right-clicking the PlayerList Widget.




Version 2.7.7:
    Change:
      * Converted all remaining Global variables to use an enum Class type.
      * Converted an if/else case within ( Player::validateSerNum ) to a switch case.
      * Converted the use of booleans within ( PacketHandler::handleSSVReadWrite ) to a typed Enum Class.
      * Removed old/dead/deprecated functions and variables from various Classes.
      * Correctly notify users using the Game Type "Warpath" [ W97 ] that the Master Mix will not respond to pings.
      * Normalized User Sernums shown within Message Dialogs when accessing the PlrListWidget context menus.
        * Previously the dialogs would show either a Hex String, or an integet string. e.g. "00000FA0" or "SOUL 4000".
      * Added a new implementation ( SortUtil ) for naturally sorting strings with integers.
        * The previous implementation within the Helper Class ( Helper::naturalSort ) has been removed.
      



    Bugfixes:
      * Fixed an issue where the Test for the rule "Max Players" would show a default of 0.
        * The default for this rule should have been showing as "256".
      * Fixed an issue where the Create Instance Dialog would not show when starting ReMix.
      * Fixed an issue with the Send Message dialog ( Accessed via right-clicking a Player within the PlrListWidget ).
        * Previously when sending a message to one User, the title would be locked with that Users SerNum.
        * Any subsequent messages to other Users would incorrectly show the initializing Users SerNum.
      * Fixed an issue where the GameInfoString would have a null byte appended to the end.
        * This caused the GameInfoString to be appended to the Server's name within the Server List.
      * Fixed an issue within the function ( Helper::intToStr ) introduced in version [ 2.7.5 ].
        * The change made was utilizing undefined logic within C++.
      * Fixed an issue within the CommandHandler class where the "Camp" command would not detect the targeting of the User initializing the command.
        * e.g. User [ SOUL 4000 ] could successfully use the command [ /cmd camp allow soul 4000 ].




Version 2.7.6:
    Change:
      * Prepared the code for use with C++ 20 and Qt6.
        * Fixed a few instances where the QMutexLocker class was deducing type. QMutexLocker is now QMutexLocker<QMutex>.
      * Expanded the remote command "Camp" to allow removal of an allowed SoulID.
        * The "Camp" command will now provide a list of all allowed SoulID's for the User invoking the command.
      * Expanded the features of the CampExemptions Class to allow storage of Allowed SoulID's and the User's Camp state.
        * Users will now have their *FULL* Camp state loaded when they connect to a ReMix server by default.
        * Note: Previously the User Class stored the Camp Lock State with the key "sceneOptOut". This is now invalid and no longer a compatible setting.
      



    Bugfixes:
      * Fixed an issue where A user would appear as having connected for an absurd amount of time.
        * Related to the deprecation of "QDateTime::currentDateTimeUtc().toTime_T()" within Qt.
      * Fixed an issue where incoming TCP packets were being improperly filtered.
        * Clumsy copy & paste.
      * Fixed an issue where invoking the Remote Command "/camp allowall" would not disable the setting invoked via "/camp allowcurrent".
        * Previously the command would only remove the setting invoked via the command "/camp lock".
      * Fixed an issue where ReMix would be opened alongside a Console/Debug window.
      * Fixed an issue where the MasterMixThread would download the MasterMix Info "synreal.ini" resulting in ReMix instances not obtaining the MasterMix information.
        * This was due to the file not being properly synced to the QSettings Object used to access the data.




Version 2.7.5:
    Change:
      * Prepared the code for use with C++ 20 and Qt6.



    Bugfixes:
      * ReMix now properly sends an empty User List when requested. 
      * Fixed an issue on Linux where the synreal.ini file was being incorrectly accessed.
        * The file was created as "synReal.ini" and was being accessed as "synReal.ini".
      * Fixed an issue on linux where the MasterMix for the WoS GameType would return no data.
        * This was another issue related to case sensitivity. e.g. "WoS" and "WOS" are treated differently.
      * Fixed an issue where ReMix would crash when starting on Linux.
        * The issue is related to the RunGuard implementation used. It is disabled on linux until a better solution is found.




Version 2.7.4:
    Change:
      * Added a log message to signify that a User has connected to a Server Instance.



    Bugfixes:
      * Fixed an issue where Server Comments would not be forwarded( Echoed ) to the originating user when the setting is enabled.
      * Fixed an issue where ReMix would infinitely connect to a slot ( Function ) during the MasterMix Information Update process.
        * This issue caused Remix to request the MasterMix Info numerous times in succession and with each iteration the request count would increase.
          * This bug is ultimately a Qt issue as the connected slots were designated at unique. We have however worked around this issue.




Version 2.7.3:
    Change:
      * The Packet Header validation now allows up to 5 exemptions for failing validation.
        * On each validation failure ReMix will attempt to re-issue a valid Packet Slot to the offending User.
          * It is unvalidated if Syn-Real Game Clients will accept or use the new Slot; however we can try it.
      * Attempt to catch half-open connections caused by an improperly disconnected User.
        * We are attempting to catch these connection failures before the Idle Disconnect ( If Toggled On ) would otherwise catch them.
        * The specific errors we're looking at are "RemoteHostClosedError" and "SocketTimeoutError".



    Bugfixes:




Version 2.7.2:
    Change:
      * 




    Bugfixes:
      * Fixed an issue where the MasterMixLog file was not being closed when the date changed.
        * This lead to the Logger Class writing to a single file in the wrong date directory.
      * Attempted fix of the Packet Slot assignment.
        * Previously a Player's Packet Slot was based on their position within the Player Storage Vector.
          * This method of doing things has a somewhat large possibility of leading to a "Slot Collision".
          * The new way of doing this assignment should reduce the possibility of collisions as we're now actively checking that the Slot has not been assigned to another User.
        * I cannot verify that this code will work as intended.
      * Fixed an issue where a User flagged for an Idle-Disconnect ( Default 10 minutes ) would continuously be flagged for a disconnect.
        * This led to: A) The User would remain connected for a longer period of time than would otherwise be possible..
        * B) The User would be sent numerous messages detailing their disconnect.
        * NOTE: A User is only flagged for auto-disconnect if ReMix has not received a Packet from the User within the default period of "10 Minutes" or the period of time set within the Rules Dialog by the Host.




Version 2.7.1:
    Change:
      * Removed references within code to the unused log type "PktForgeLog".




    Bugfixes:
      * Fixed an issue where manually muting a User via the PlayerList UI Elements would not properly mute the selected user.
      * Fixed an issue where ReMix could close improperly due to an incorrect cleanup of the MasterMixThread object.
      * Fixed an issue when starting ReMix with instances using the "AutoRestart" toggle that would allow multiple consecutive downloads of the synreal.ini from the MasterInfoHost.




Version 2.7.01:
    Change:
      * Added an additional Log message within the MasterMixThread to show if/ and how much data has been downloaded from the Master Mix Host.



    Bugfixes:
      * The process for automatically downloading the MasterMix information is now properly started.
        * Note: This only happens every 24 hours.
      * Fixed an issue where starting a new ReMix Instance would cause the MasterMixThread to completely re-download the MasterMix information.




Version 2.7.0:
    Change:
      * Implemented a MasterMixThread Object.
        * This Object is used to automatically download and update the MasterMix information for all currently running ReMix instances.
        * Note: The old method of obtaining the MasterMix Host information ( while unused ) will remain within the code base for the time being.
      * Added the ability to override the Master Mix Host Addresss via the Settings dialog.
        * Note: This override can be used in conjunction with a manual IP address override ( Also provided via the Settings Dialog ).
          * When both overrides are enabled, ReMix will default to the IP Address Override while also obtaining the information from the overriden Host Address.
      * Removed an unused Log type: "PacketForge".
      * The UdpThread Object now handles User Pings on it's own instead of forwarding certain requests to the PacketHandler Object.
        * This reduces the need to parse the information twice, and allows the Log to appear in cronological oder.



    Bugfixes:
      * Fixed an issue related to downloading the MasterMix Information ( synreal.ini ).
        * The Old behavior would continuously append the newly downloaded information to the file.
        * This is an undesirable behavior as it reduces the chances of actually obtaining the proper MasterMix address.
      * Fixed an issue where it could be possible for a ReMix instance to attempt communication with the Master Mix without having the valid Port set.
        * Note: This did not pose an issue before now, as overriding the MasterMix Host Information was not possible.




Version 2.6.12:
    Change:
      * Converted the Logger View to a simple List View. This allows the information it more easily be read without adjusting the column height of a Table View.
        * Over-all functionality and information provided remains the same.
          * This change is in preparation for allowing a context menu to copy a complete row from the log view.



    Bugfixes:
      * Fixed an issue preventing ReMix from sending the Server List when requested from a user with a Golden Soul.
      * Fixed an issue when manually un-muting a User via the User Information Viewer would not update the User's status correctly.




Version 2.6.11:
    Change:
      * Added a new Log type "MasterMixLog".
        * Note: This log file will contain any information related to communicating with the Master Mix, or obtaining information about it.
          * With the new addition to the Logger Class, and being able to filter logs it should now be easier to diagnose a Master Mix issue.
      * Re-Named the "UsageLog" to "ClientLog".
		* Note: The changes to the "UsageLog" is a long overdue change in an effort to make the information contained more easily found.
      * Renamed a few other log files to follow the naming of the other log types.
        * The logs affected are: Comments, AdminUsage, and PktForge.
        * The are now (as you can guess): CommentLog, AdminUsageLog, and PktForgeLog.



    Bugfixes:
      * Fixed an issue where closing a Server instance tagged with the rule "AutoRestart" would not appear within the Create Instance Dialog list.
      * Fixed an issue where closing a Server instance would prompt to remove the rule "AutoRestart" whether or not the rule was even enabled.
      * Fixed an issue where the Logger Class would not properly clean up the WriteThread object when closing ReMix.
        * This is to prevent ReMix from retaining a "lock" on the log files, and possibly prevent a proper closure of the application.
      * Fixed an issue where "Chat" would appear within the LogView UI.




Version 2.6.10:
    Change:
      * Added code to convert the bytes sent to and from Users into a more user friendly format.
      * Removed code related to calculating the classic baud (bytes per second) in leiu of simply showing how many bytes have been sent/received in a modern/ user friendly format.
      * Changed the code used to filter the Log View UI to be less verbose, more efficient, and over-all easier to undertsand what is happening.
      * When manually closing a server instance; in addition to confirming the instance closure, we now also check if the AutoRestart rule should be disabled.



    Bugfixes:
      * Correctly calculate the sizes of packets send/received into the over-all server statistics.




Version 2.6.9:
    Change:
      * Implemented the TODO from version 2.6.6.
        * The Log View UI can now be filtered via a dropdown menu placed within the Logger Class UI.
      * Added a checkbox within the Logger Class UI to allow for automatic clearing of logs.
        * The interval is 24 hours, and cannot be changed or set to another value.
      * Added a Button within the Logger Class UI to allow for manual clearing of logs.
        * There is a confirmation dialog when manually clearing so as to not accidentally clear logs.



    Bugfixes:
      * Fixed a malformed versioning packet beign sent to the Master Mix.
        * The malformed packet functioned correctly, but it's better to be safe and fix it.




Version 2.6.8:
    Change:
      * Added the ability to override the default Master Mix IP Address.
        * This override is set via the Settings dialog under the direct Settings Tab.
        * The format for this override is a String value: <IpAddress>< : ><Port>.
        * Note: The Colon( : ) is a required delimiter and any address not containing it will be accepted but will not work.
        * Note: This Override is used across all Server Instances as such only one game *type* may be hosted at once.
        * Note: When loading a Master Mix Override from file the process of fetching the Normal Master Address is skipped, thus a full ReMix restart is required to resume normal operation.
      * Converted a few C-Style enums to the new C++ style enum class.



    Bugfixes:
      * Fixed the issue of having the Create Instance dialog remaining open when auto-restarting server instances.




Version 2.6.7:
    Change:
      * Added a new Rule within the Settings/Rules dialog "Auto Restart" to enable ReMix to automatically restart a server when re-opening ReMix.
        * Known issue: The Create Instance Dialog will also open, and if the host has no other servers to run, must be closed manually.
      * Consolidated code related to initializing a ReMix instance to make it easier to understand.



    Bugfixes:
      * 




Version 2.6.6:
    Change:
      * Added a new Log File "PingLog" which will now log pings from Users.
        * Previously these were written to the "UsageLog".
        * TODO: Implement a method to hide certain log types from the Log Viewer.
      * Reduced usages of "magic" numbers in relation to converting strings to integers.
        * An enum encompassing the various valid formats within Qt has been added.



    Bugfixes:
      * Fixed an issue where ReMix could show Users as having connected for longer than the a Server Instance has been online.
      * Fixed an issue where a ping response was listed within the Log Viewer before the actual ping itself.
      * Fixed an issue where Players could connect using negative sernums.
        * Players connecting with a negative sernum will be disconnected as though they were a BlueCoded SerNum, regardless if the Host has enabled the setting.




Version 2.6.5:
    Change:
      * Comments to a Server Instance will now be echoed to all admins on all initialized Server Instances.
        * The setting "Echo Comments to Admins" must be enabled.
      * ReMix no longer tracks a User's Ping count.
      * User-list request responses are now omitted from the Log View if the Server has no connected Users.
      * The command "/info" will now only count Users as Admins if the Admin has logged in with a password.
        * Previously all Admins were counted regardless of login state.



    Bugfixes:
      * Negative SerNums are now correctly handled.
      * The commands: "/kick", "/ban", and "/mute" can now be used with the "all" sub-command if no message has been provided. e.g. "/ban all"
      * Commands using the sub-command "all" will no-longer spam the command isser with failures due to tank or User statuses.
      * Pets of the same level as a Player can no longer be called within another Player's camp (scene).
      * Fixed a crash related to the "/camp allow" sub-command.
        * If a User were to omit the soul-targeting (/camp allow soul 4000) command the command would fallthrough and access an invalid nullptr.




Version 2.6.4:
    Change:
      * Convert variable initializers to be C++11 brace initializers to be more consitent with newer code.
      * Reduced the usage of auto casted variables when the type is known within source.
      * Removed a few unused source files from the repository.



    Bugfixes:
      * Finalized fixes related to the improper conversion of SerNums from hex to dec.




Version 2.6.3:
    Change:
      * ReMix now checks the 'K' and 's' packet for Pet and Player levels. We also parse the Player's AFK status, but it is not meaningfully used.
        * If calling a pet into another Player's scene then we check these two levels and confirm that the Pet can be called by the Player.
        * If the pet is a higher level than the Player then we drop the packet and inform the Player that the pet cannot be called.



    Bugfixes:
      * Fixed an issue where GS sernums were being read as white souls.
      * Properly clean up the CampExemption object when closing ReMix.




Version 2.6.2:
    Change:
      * Implemented the ability for Users with the Remote Administrator rank Admin or above to alter a Player's camp(scene) status. Whether locked or allowing only current Players.
        * These features can be used by appending "soul *PlayerSerNum" to the normal sub-command. e.g. "/camp lock soul 4000".
        * As ReMix does not supprt sub-sub commands, this command does not appear within the output generated via "/list".
      * Implemented the ability for Players to individually allow specific Users to bypass a camp(scene)'s locked status.
        * This is done via the sub-command "allow", and the appended sub-command "soul". e.g. "/camp allow soul 4000".
          * Players are added to an exemption list which is saved to disk.
          * The command "allow" currently acts as a toggle, and allowing a previously exempted Player will remove them from the exemption list.
        * As noted previously, the "soul" command does not appear within the output generated via "/list" for the "/camp" command.
        * An example of the "/list" output: camp[ lock, unlock, onlycurrent, allownew, allow,  ], 
        




Version 2.6.1:
    Bugfixes:
      * The method for preventing new Players from entering old camps(scenes) should now correctly function and should allow Players to enter camps(scenes) that are set to the "allow all"/default status.




Version 2.6.0:
    Change:
      * Implemented the chat command "/camp" with 4 sub-commands; "lock", "unlock", "onlycurrent", and "allownew".
        * Using "/camp" without any subcommands will issue the Player their current camp(scene) settings.
        * "lock" Prevents all other Players from entering any scene(camp) hosted by the command issuer.
        * "unlock" Returns the camp(scene) to default functionality. 
          * Unless "onlycurrent" is toggled on.
        * "onlycurrent" Makes it so only Players online when the command issuers scene(camp) was created can enter.
          * This is an opt-out function and is not the default.
        * "allowall" Allows Players to enter a camp(scene) hosted by the command issuer. This is regardless of connection or camp(scene) creation time.
          * Unless "lock" is toggled on.
      * The locked state issued via the command "/camp lock" is not saved to the Player's data on disk and will only persist while the Player remains connected.
        * The "onlycurrent" and "allowall" states are however saved to disk and will persist permanently until changed by the Player.



    Compatibility:
      * The server rule "maxAFK" has been renamed to "maxIdle" thus compatibility with the old Rule is broken, and Server Hosts will need to re-set this rule if it was in use.




Version 2.5.11:
    Bugfixes:
      * Fixed an issue related to disconnecting idle users.
      * Idle duration is now correctly applied to all Player objects if changed after server initialization.
        * When showing the default Idle Duration, the Rule will remain un-checked.
      * The Rules Widget now reflects the default Idle Duration when the Host has not changed it.
        * This duration is only enforced when the Host has also enabled the Setting "Disconnect Idle Users".
      * UI Elements related to the Max Idle duration have been renamed to more correctly show their intended purpose.
        * Previously within the Rules Widget it was referred to as a "Max AFK Duration".




Version 2.5.10:
    Change:
      * ReMix will now attempt to re-fetch the 'synreal.ini' file every 24 hours down from previously 48 hours.
      * ReMix no longer parses any packets for Users within a Muted state.
      * Users in a disconnected state will no longer be parsed as a valid User from the Players Vector.



    Bugfixes:
      * ReMix will no longer Echo comments from Users to ReMote Administrators when comments are being forwarded to all Remote Administrators.
        * This is to prevent the same message from being sent to the originating User\Remote Administrator twice.
      * Attempt to prevent the 'synreal.ini' file from being created in a 'null' or empty state.
        * ReMix will now also properly re-initialize itself with any newly fetched information contained within the 'synreal.ini' file.
      * Reduce UI 'lag' in relation to resizing UI columns.
      * Player Objects will now correctly be placed in a disconnected state when the TcpSocket signals that it is disconnecting.




Version 2.5.9:
    Change:
      * The Server Class now only handles receiving a socketdescriptor and forwarding it to the ReMixWidget Class.
      * The Settings class now controls the plrBioHash.
      * The Player Class now handles it's own readyRead signal/slot, and emits incoming packets to the PacketHandler Class.
      * The ServerInfo class should now interact with the UPNP class much better to allow forwarding ports before checking in with the MixMaster.



    Bugfixes:
      * Added a few QMutexLockers within the Settings Class to prevent possible threading issues from reading rules/settings.
      * A ReMix Server's GameInfo string will once again be obtained from the RulesWidget Class and appended to any Ping Response.
      * Fixed a crash related to creating a Server Instance.
        * The crash was caused by deleting a (global) RandDev Object from within the Settings Class.
      * Fixed an issue when closing a ReMix Instance.
        * Instances would close far too fast to properly inform the MasterMix that it had indeed closed.
        * This issue is most apparent when closing a large number of servers at once.
      * ReMix will now warn the host when they have reached the maxiumum server count.
        * In the case that the Server Count has been reached, no Server Instance will be created.
        * Previously the Server Instance was created, and active without being appended to the ReMixTabWidget UI.
          * If a User were to attempt to create another Server Instance with ReMix in this state, the MasterMix would ban the User.
      * Fixed a possible crash related to not properly deleting the UdpThread Objects stored within the ServerInfo Class.




Version 2.5.8:
    Change:
      * Settings and Rules are now handled via directly accessing the values using the getSetting/getRule functions directly in conjunction with Keys and SubKeys.
        * The goal of these changes is to reduce the verbosity of the Settings and Rules class files.
      * Convert the remaining uses of the older style of accessing Settings.
        * One exemption is related to getting the ServerID. This is remanining in use due to generating the ServerID in place.
      * Completely merged the Rules Class use cases into the Settings Class.
      * Comments from Users within the Comments Dialog Window will now be printed more like the Chat View.
        * The CommentLog no-longer double-prints the timestamp or new (blank) lines.
      * Add the ability to delete Remix Servers from the CreateInstance Dialog.
        * Server deletions will be confirmed to prevent accidental deletions.
      * Unify Master Server Messages to a signal to the Player class.
        * This reduces the risk of sending a Master Message to a nullptr, and the need to iterate over every Player Object.
      * ReMix will no longer Parse a User's BIO data for the DVariable, or the WVariable.
      * When changing a ReMix Server Instance's Name, the Instance will first disconnect from the MasterMix before reconnecting with the new name.
      * The Player Class now Inherits from the QTcpSocket class.
        * This allows the Player class to send/recieve packets on it's own without the need for a QTcpSocket variable or checking it's validity.



    Bugfixes:
      * Comments are once again only written to disk when the corresponding Setting is toggled on.
      * Opening a ReMix Instance will no longer overwrite settings if the name of a previous Open Instance is used.
      * When both "Allow Duplicate IP's" and "Ban Duplicate IP's" enabled, ReMix will only ban a User if "Allow Duplicate IP's" is disabled.
        * TODO: A more elegant solution is to toggle each setting off when the other is enabled.
      * The Settings "Allow Duplicate IP's" and "Ban Duplicate IP's" will now toggle eachother off as needed. The previous change is undone.
        * The Rules "AllPK" and "NoPK" will also toggle eachother off as needed.
        * The Rules "AllPK" and "ArenaPK" will also toggle eachother off as needed.
      * Server Passwords are once again correctly checked.
      * The Rules dialog will now once again properly handle checking and unchecking the "Server password" option.
      * Revoking a ReMote Administrator who is in a pending state will now properly revoke the rights to register with the server.


     
    Remove:
      * Some redundant class files.
      * Files related to the WorldShuffler Class.
      * Files related to the Rules Class.
      * Removed unused/commented code from several Class Files.




Version 2.5.7:
    Change:
      * ReMix now properly sends a response when receiving a ":SR?" formatted packet.
        * The information sent within this packet is stored and used to verify that the receiving User is operating correctly.
        * When ReMix senses that the stored value is different from what the User is sendint, we disconnect the User and send them a message.
      
	  

    Bugfixes:
      * Fixed an issue where the UPNP class would become disconnected from the Logger class.
        * This issue was preventing the UPNP from inserting logs to the Logger UI and disk.
      * Fixed a string formatting error in relation to the new Packet Header checks.




Version 2.5.6:
    Change:
      * Added a new Threaded Class Object. UdpThread.
        * This class handles some interactions with Users and the Master Mix via the UDP protocol.
        * While the PacketHandler class still parses some packet information, the UdpClass parses and handles any packets that require a response.
        * The ServerInfo class retains control over MasterCheckin timing, and sends a signal to the UdpClass to send the packet itself.
      * The Logger class now has multiple file handles for each individual log type.
        * These handles are opened as they are needed, and will remain open until ReMix closes.
          * These file handles are also closed and re-opened when the date used for the Log Folder changes.
        * These changes are done in an attempt to reduce file opening and closing overhead.
      * Convert all calls to the Logger::insertLog() into QObject signals to the Logger class.
        * These Signals are forwarded to the WriteThread for writing.
      * Added a new threaded Class Object WriteThread.
        * This new class handles all file write operations within the Logger Class.
      
	  

    Bugfixes:
      * Reduced latency overhead when pinging a ReMix server hosting multiple Worlds/Games.
        * The MasterUDP Socket used is now threaded, and should be thread-safe.
      * Remote Administrator commands will now inform the Admin of an issue if they attempt to use a command on theirself.
      * Sernum Information is now properly parsed from the MIX5 and MIX6 packet types.
        * This allows an Admin to log-in before being prompted if they so desire.





Version 2.5.5:
    Change:
      * Reduced latency when forwarding packets to players.
        * Packets will be sent to all eligible players at the same time when it is received.
        * Packets sent by a Player will now properly be ignored when ReMix forwards it to that Player Object.
        * The Player class will now check if a packet is eligible to be sent to the selected Player.
        * Parsing of the MIX packets is now handled within the same function as the SR packets.
      * Consolidated the isBanned, isMuted, removeBan, and removeMute into semi-universal functions. e.g. isPunished, and removePunishment.
        * Ban and Mutes will no longer check for certain sub-values. Namely; PunishmentTypes::WV, and PunishmentTypes::DV.
        * These ban types are holdovers from the standard synMix and the checking required to remove the ban isn't worth the effort to retain them any longer.
        * This means that only the User SerNum and IP addresses are checked for punishments.
      * The Server class now handles the bioHash as a static object.
        * Various static functions have been added to facilitate accessing this data from the other Class Objects.
      * Add two new schemas to the UPNP class from the WiFiAlliance.
        * "urn:schemas-wifialliance-org:service:WFAWLANConfig:1" and "urn:schemas-wifialliance-org:device:WFADevice:1"
	  

    Bugfixes:
      * Properly inform Users when the server has automatically muted their connection.
      * Properly format the message sent to the User when Muting and Un-Muting from the PlayerListWidget Object.
        * ReMix also now properly applies the correct Mute duration to the Player Object.





Version 2.5.4:
    Change:

    Bugfixes:
      * ReMix now detects when a User is within a Well Scene and no longer saves the Camp packet associated with that scene.
        * Entering camp scenes created before connecting to the server will now correctly function.
      * Mutes will now once again check if the IP address of the User has an outstanding Mute.
      * Attempt to prevent checking in to the Master Mix until the Master Mix IP address has been found.





Version 2.5.3:
    Change:
      * ReMix will now log, and show within the Logger UI when a Server Side Variable is being accessed with a read/write.
        * The newly generated logs will appear within the log directory and sub directory with the name [QuestLog].
      * Bans issued via Remote Administrator commands issued with no duration will now default to a 7 day ban.
        * Previously this was 30 days.
      * ReMix will now send socket information to newly connecting Users.
        * This information is used within WoS' skin transfer routines.


    Bugfixes:
      * Server Side Variables when being written will now forward the updated variable information to all connected Users.
        * Previously no update information was being sent. It has been verified that the intended behaviour was to forward the update to connected Users.
      * Corrected a state mismatch with the message being sent with the /vanish admin command.
        * When no sub-command was issued, the state would always show as being "invisible" within the message being sent to the Remote Administrator.
      * ReMix will no longer attempt to re-forward a port via UPNP that it has decided should be a permanent forward.
      * Fix build error.

    Source:
      * Begin converting the source from a 80 column text limit to 180 columns.





Version 2.5.2:
    Change:
      * Users who have been Muted will no longer have the ability to send comments via the Admin Window to the Server Host.


    Bugfixes:
      * Remote Administrator commands will no longer send a warning that the selected target is an Admin or Offline if neither state is true.
      * Mutes, whether automatic or remote-initiated via Admin Commands will now default to 10 minutes.
        * This duration was previously 30 Days.
      * Chat from the Chat view will no longer appear within the Logger UI.
      * When the "Ban Duplicate IP's" option is enabled, Users will now only be banned for a duration of 10 Minutes.
        * This duration was previously 30 Days.




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



