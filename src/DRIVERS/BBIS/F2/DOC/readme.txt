F2 BBIS Board Driver - Notes                             6/9/2000 ds
============================


Introduction
------------
The F2 board driver supports several on-board devices located on
the F2 CPU board. The devices correspond to ISA or PCI devices:

 On-board | Corresponding                   | ISA/PCI | Used
 Device   | Chip (function)                 | Device  | Resources
 ---------+---------+-----------------------+---------+--------------
 0x1000   | Z8536 (Watchdog, SMB-Ctr, Trig) | ISA     | io space, IRQ
          | LM79 (System monitor)           | ISA     | IRQ
 ---------+---------------------------------+---------+--------------            
 0x1001   | ALI M1543C (User LEDs, GPIOs)   | PCI     | config space 


IRQ Settings
------------
The IRQ used for on-board Device 0x1000 depends on the F2 BIOS version.
Since the current BIOS version 00.02 doesn't assign any IRQ to on-board
device 0x1000, the IRQ must be specified via the optional descriptor
key IRQ_CIO_LM.

Furthermore, the current BIOS doesn't assign the proper IRQ for
COM3/COM4 specified in the BIOS setup:

 IRQ - BIOS setup | used IRQ
 -----------------+----------
 IRQ3             | IRQ7
 IRQ4             | IRQ9
 IRQ10            | - 
 IRQ11            | -

If you want to set an apropriate IRQ for COM3 or COM4, you can use the
descriptor keys IRQ_COM3 / IRQ_COM4.

For a detailed description of the descriptor keys, refer to the
driver function description (F2_Init) and to the example descriptors.

A T T E N T I O N :
+--------------------------------------------------------------------+
| The IRQs used by on-board device 0x1000, COM3 and COM4 must be set |
| to 'Legacy ISA' in the BIOS PNP/PCI CONFIGURATION SETUP.           |
| Otherwise the BIOS may assign these IRQs to PCI devices and the    |
| on-board device 0x1000, COM3 and COM4 won't work.                  |
+--------------------------------------------------------------------+


Further information
-------------------
For Windows NT refer to the readme_nt.txt file.
