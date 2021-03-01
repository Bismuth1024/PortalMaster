# PortalMaster

An editor and tool for a certain series of Mifare Classic 1K based games.  It was designed for my own personal use, and as such uses a very specific setup of an Arduino Nano connecting a Macbook and a PN532.  However, things like encryption/decryption, toy data map information, file i/o, etc. are of course universal to whatever communication hardware/software is used between the toys and the PC.

Functionality list (will be updated as work progresses) 

Currently supported:

  -Editing of stats of figures up to and including gen 5.
  
  -Saving and loading backup files to/from figures.
  
  -Correction of checksums on "broken" figures.
  
  -Cloning or creation of figures with Gen2/CUID magic tags, for games up to gen 4.
  
  
In progress:

  -Editing stats of later figures (work out the checksums)
  
  -Simulating/cloning gen 5+ figures (5 doesn't seem to like them)
  
  -Working out how to simulate game 6 (very unlikely, Ed22519, but still hopeful)
  
  -Finer control of quests (e.g. game 1) or heroic challenges
  
Much of this is built upon work by other projects; very little of it involves any breakthroughs in keys or data structures from me.
