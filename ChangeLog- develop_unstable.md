TODO:
  * 

Version 3.0.0:
    Change:
      * Initial branch from develop_stable.
      * Converted the Player and Server class objects to Smart Pointers (QSharedPointer<Server> and QSharedPointer<Player>).
        * Corrected some logic related to the usage of Smart Pointers in order to increase reliablity and stability.
      * Reduced the complexity of the MasterMixThread class.
      * Added overloads for "User::addMute" and "User::addBan" for use in manual actions.
      



    Bugfixes:
      * Fixed a possible crash related to the Server class deconstructing while the UdpThread continued to run.
      * Fixed a possible crash related to the MasterMixThread class.
        * The crash was related to a dangling pointer to an invalid Server instance.
      * Fixed an issue when closing a Server Instance related to the AutoRestart rule.
        * We now prompt the User if they wish to remove ther AutoRestart rule before closing the Server instance.
        * If the User Accepts the Rule Removal and Declines the Server closure the AutoRestart rule remains activated.