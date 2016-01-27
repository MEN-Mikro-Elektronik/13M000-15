F2 BBIS Board Driver - Notes for Windows NT              6/9/2000 ds
===========================================


Serial COM Ports
----------------
Windows NT version 4.0 uses the serial.sys driver for COM1..4 of the
F2 CPU Board.

Since the current BIOS doesn't assign the right IRQs to the COM3 and
COM4 ports, and Windows NT doesn't known the IRQs currently used for
these ports, you must perform the following steps:

1) Specify the IRQs you want to use for COM3 and COM4 in the F2 board
   descriptor.
  
2) Now you must inform the serial NT driver which IRQs are used by
   the COM ports. To do this, goto Start->Settings->Control Panel->Ports
   and select COM3/COM4. Under Settings->Advanced you must select the
   same IRQs as specified in the board descriptor (1).
   
3) Restart NT or open the device panel and stop/restart the serial driver.

4) Start the F2 board driver men_f2.

Now you can use COM1..4.
