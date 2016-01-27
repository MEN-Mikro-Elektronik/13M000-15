Windows specifics for SMBPCI/SMBISA board drivers                  2010/12/13 dp
=================================================

Installing the Board Driver Manually
------------------------------------
* If you have to install the SMBPCI/SMBISA board driver manually or not
  depends on the used HW. Please refer to the HW specific installation note!

* Use the Add Hardware Wizard to install the board driver manually.

* Choose the proper wizard options to install a new device manually and specify
  the location of the inf-file belonging to the SMBPCI/SMBISA board driver (e.g.
  13bb0370_smbpci.inf).
  Then select the correct model (e.g. SMBus on F210) for the used SMBbus
  controller and continue the installation with the wizard.

* If you have successfully installed the board driver, the driver should
  recognize the assigned SMB devices and the Found New Hardware Wizard should
  appear for each SMB device to guide you through the installation of the
  matching device drivers.

Please refer to the MDIS under Windows user manual for further information
about the installation of MEN's Windows MDIS drivers.
