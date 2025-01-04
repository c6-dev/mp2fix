# Max Payne 2 launcher skip fix

When `-nodialog` launch option is used in MP2, several graphics settings don't get loaded correctly, namely AA, texture filtering and texture color depth. This patch fixes it.  

Before and after the patch, when `-nodialog` is used and AA set to 8:

![Untitled](https://github.com/c6-dev/mp2fix/assets/31777460/87506cd4-35a1-48e3-98d8-b8cde09e9354)

Supports latest Steam release only.  

To install, unpack `dsound.dll` into the game folder.  
Alternatively, If you want to use another mod that uses dsound DLL, you can unpack `dinput.dll` instead.  

Same problem is present in Max Payne 1 - fix available [here](https://github.com/c6-dev/mp1fix).
