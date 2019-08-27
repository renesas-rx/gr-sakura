PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_t4_rx
=========

Document Number
---------------
R20AN0051JJ0206-RX-T4
R20AN0051EJ0206-RX-T4

Version
-------
v2.06

Overview
--------
The TCP/IP functions for Renesas MCUs.

These configuration options can be found in "r_config\r_t4_rx_config.h". 
An original copy of the configuration file is stored in 
"r_t4_rx\ref\r_t4_rx_config_reference.h".


Features
--------
* T4 has small footprint for Renesas MCUs.
* T4 can add TCP/IP functions for Renesas MCUs easily.
* T4 supports Renesas MCUs. Especially Ethernet Controller Model.

Supported MCUs
--------------
* RX Family

Boards Tested On
----------------
* RSKRX71M
* RSKRX64M
* RSKRX65N
* RSKRX63N
* RSKRX62N

Limitations
-----------
None

Peripherals Used Directly
-------------------------
None

Required Packages
-----------------
None

How to add to your project
--------------------------
Please refer to the Adding Firmware Integration Technology Modules to Projects.
"r01an1723ju0111_rx.pdf/r01an1826jj0102_rx.pdf(R20AN0051JJ0206-RX-T4)"
"r01an1723eu0111_rx.pdf/r01an1826ej0102_rx.pdf(R20AN0051EJ0206-RX-T4)".

Toolchain(s) Used
-----------------
* Renesas RX Compiler V.2.05.00

File Structure
--------------
r_t4_rx
|   readme.txt
|   
+---doc
|   +---en
|   |       r20an0051ej0206-rx-t4.pdf
|   |       r20uw0031ej0109-t4tiny.pdf
|   |       r20uw0032ej0108-t4tiny.pdf
|   |       
|   \---ja
|           r20an0051jj0206-rx-t4.pdf
|           r20uw0031jj0109-t4tiny.pdf
|           r20uw0032jj0108-t4tiny.pdf
|           
+---lib
|       r_mw_version.h
|       r_stdint.h
|       r_t4_itcpip.h
|       T4_Library_rxv1_ether_big.lib
|       T4_Library_rxv1_ether_big_debug.lib
|       T4_Library_rxv1_ether_little.lib
|       T4_Library_rxv1_ether_little_debug.lib
|       
+---make_lib
|       make_lib.zip
|       
+---ref
|       config_tcpudp_reference.tpl
|       r_t4_rx_config_reference.h
|       
\---src
        config_tcpudp.c
        
