PROJECT:
	Xytron

AUTHOR:
	Damian Coventry
	Music by Robert DeVoe

CONTACT:
	http://damiancoventry.com/
	damian.coventry@gmail.com

CREATION DATE:
	June-July 2007

COPYRIGHT NOTICE:
    (C) Omenware.  Created in 2007 as copyright work.  All rights reserved.

NOTES:
	You will need some form of OpenGL hardware support to run this game.  You
	don't need to own an expensive card, this game was developed on a machine
	running an "Intel 82915G/GV/910GL Express Chipset Family" adapter. This is
	a cheap adapter and manages to achieve well over 200FPS in windowed mode,
	and reaches the monitor's refresh rate of 75FPS in fullscreen mode.

SUPPORTED OPERATING SYSTEMS:
	Windows Vista	Runs poorly, because at the time of this writing no vendors have written OpenGL ICDs for Vista
	Windows XP		Runs great
	Windows 2000	Untested
	Windows 98		Untested

DEPENDENCIES:
	In order for the "xytron.exe" file to run, each of the following DLLs must be accessible in your path:
		winmm.dll		- MS Windows Multimedia interface.  Used to run the game loop against a time base.
		kernel32.dll	- MS Windows Kernel interface.  Used to create and terminate Xytron's process.
		user32.dll		- MS Windowing interface.  Used to create and control a desktop window.
		gdi32.dll		- MS Graphics Device interface.  Used to setup OpenGL's pixel format and to do page flipping.
		ole32.dll		- MS COM library.  Used to pull up DirectSound, DirectInput and DirectShow (Active Movie).
		msvcp80.dll		- MS Visual C++ Run-time.  Used for container and string objects.
		msvcr80.dll		- MS Visual C Run-time.  Used for memory allocation and file I/O.
		opengl32.dll	- OpenGL interface. Used for drawing to the screen.
		glu32.dll		- OpenGL utility interface. Used to build textures (Mip maps).

	You need to have DirectX 8.0 or later installed on your computer because the game uses DirectInput and
	DirectSound.  For music the game uses DirectShow.
	
	However, the game will run without sound and/or music if you do not have a sound card installed on
	your computer.

	Because of the DirectX 8.0 dependency you must have the following COM object registered on your computer:
		dinput8.dll		- MS DirectInput.  Used to detect keyboard and mouse events.

	The COM objects used for sound support are:
		dsound.dll		- MS DirectSound.  Used to play sounds.
		dsound3d.dll	- MS DirectSound3D.  Used to play 3d sounds.

	The COM object used for music support is:
		quartz.dll		- MS Active Movie. Used to play music.










CHEAT CODES:
    I                   Toggle invulnerability on/off
    S                   Skip to next wave
    End                 Skip to the last level of the game
    P                   Add 10000 points to your score
    B                   Skip to Boss/Mini Boss Enemy
    W                   Level up the current Weapon
    L <number> Enter    Where number is from 0 to 29. For example you'd press
                        the following key sequence to go to level 15: L 1 4 Enter
