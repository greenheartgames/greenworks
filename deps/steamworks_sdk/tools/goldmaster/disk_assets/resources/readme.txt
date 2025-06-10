Steam Retail Setup File Overview:
---------------------------------

root\Setup.exe :
  The main splash screen setup application launched when the customer inserts
  the disc. Setup detects the current installation state and displays a simple
  menu to install/play/reinstall or uninstall the game. Setup first loads
  setup_default.ini, then the setup_<language>.ini for the language chosen by
  the user, then all other resource files are loaded based on the current
  language configuration. Setup.exe itself does not require admin rights.
  
root\SteamService.exe :
  Helper application to run all additional 3rd party installers from a single
  elevated process under Vista. If you don't need to run 3rd party installers,
  this executable can be removed.

root\SteamSetup.exe
  Used to install Steam on the users machine.
  
root\autorun.inf : 
  Tells Windows to run Setup.exe when user inserts disc
  
root\splash.tga : 
  Image displayed by Steam while installing files from disk (392x165 pixel)
  
root\setup.ini :
  This is the first config file loaded by Setup regardless the chosen
  language. This file configures any language independent settings for this
  game like AppID or menu appearance. The syntax is "parameter" "value", a full
  list of configuration parameters is given later.

root\resources\setup_<language>.ini :  
  These files configure Setup for the chosen language. The files are Unicode
  encoded. They are loaded after setup_default.ini and can override any
  configuration value for this language. Usually they don't need modification.
  Currently supported language names are: english, german, french, italian,
  korean, spanish, schinese, tchinese, russian, thai, japanese, portuguese,
  polish, danish, dutch, finnish, norwegian, swedish, hungarian, czech
  
root\resources\eula.rtf :
  The end user license agreement shown before the user installs the game.
  This file is optional. If you have localized versions of this file,
  name them eula_<language>.rtf
  
root\resources\setup.bmp :
  The Setup background image as 640x480 pixel BMP file.  If you have 
  localized versions of this file, name them setup_<language>.bmp

root\resources\click.wav :
  Played when a button is click. Delete file to play no sound.
  
root\resources\hover.wav :
  Played when a mouse hovers over a button. Delete file to play no sound.

root\resources\launch.wav :
  Played when Setup starts. File can be deleted.
  

Setup_*.ini configuration values:
-------------------------------

"Game"       : game name as it appears in the Setup menu
"AppID"      : Steam AppID for this game
"Language"   : overrides current language, usually not needed
"RequiredSize" : Required free disc space in KB
"URL"        : target web URL for technical support button
"FontName"   : menu Windows font like "Impact" or "Arial"
"FontHeight" : menu font height, eg "24"
"MenuPos"    : menu X and Y coordinates, eg "100 100"
"MenuStyle"  : menu text style, 0 = align left, 1 = center, 2 = right
"ColorDefault" : default menu text color in RGB, eg "255 0 255"
"ColorHover" : menu text color for mouse hover event
"ColorDown"  : menu text color for mouse pressed event

