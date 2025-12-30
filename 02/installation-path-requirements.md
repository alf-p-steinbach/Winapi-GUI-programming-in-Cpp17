#### Requirements for a tools installation directory path.

Where *can* a tools installation directory reasonably be placed?

We/you will be using tools ported from Unix, and such tools, and especially their installers, are notorious for being unable to handle **spaces in paths**. Unix paths can contain spaces just as Windows paths can so it’s a mystery why this is so, but there it is: programs ported from Unix are often really space-averse. A space? Gasp! \<tool dying\>

Additionally some programmers’ tools with too close ties to Unix require an installation path that is free of certain other characters. As of this writing the very common Qt GUI library is an example: it refuses to install to a path that includes the “@” character, which I use as a shortest possible “clean root” name. Well I tricked it by definining an alias (specifically a Windows “junction” defined with `mklink`) called “root”, and then at least the installer was happy, but there is the nagging lingering suspicion that if I start using it then some part of it will malfunction when it’s exposed to the “@”.

And so if a directory for the tutorial is to contain also all tools installed for the tutorial activities, which would be natural & clean, then its path should be free of spaces and “@” characters.

The [MSYS2 installation instructions](https://www.msys2.org/#installation) lists the requirements

> ❞ short ASCII-only path on a NTFS volume, no accents, no spaces, no symlinks, no subst or network drives, no FAT

A **symlink** is a file created with `mklink`, that wrt. to e.g. C++ standard library operations acts as an alias for a file or directory. A **subst drive** is a drive letter created with `subst`, that acts as an alias for a directory.  A **network drive** is a drive letter created with `net use` or the corresponding Windows Explorer functionality (right click in *This PC*), that is like a subst drive except that it can literally be a drive that refers to a server on the net, so it’s far more heavy machinery, involves security, and is less convenient.

**NTFS** is short for *NT file system* where NT is short for *New Technology*. It’s the default file system implementation in Windows, so for a directory on the “c:” drive this requirement is satisfied. **FAT** is an old file system implementation from the days of DOS, before Windows, no longer used, but more modern variants like **FAT32** are still used, even hidden on an ordinary PC:

```text
[C:\Users\alfps]
> wmic volume get driveletter, label, filesystem, name
DriveLetter  FileSystem  Label                    Name
C:           NTFS        OS                       C:\
             NTFS        RECOVERY                 \\?\Volume{222bde03-f6be-4b15-b921-a8ebe44b1bf5}\
             FAT32       MYASUS                   \\?\Volume{98eeae90-64e3-4c05-a210-deeb5195eb0d}\
F:           NTFS        Seagate Expansion Drive  F:\
             FAT32       SYSTEM                   \\?\Volume{5cf8e662-be2e-4727-9033-1622bce3fde7}\
D:                                                D:\
E:                                                E:\
```

Arguably the USB drive at “f:” should have been formatted with something other than NTFS because its (silly, over-engineered) security system starts sabotaging things when the drive is used on some other computer. But anyway the internal FAT32 partitions shown above are not a problem since they are ordinarily not visible or accessible. No tools will be installed there.

The easiest way to comply with these requirements is to put the tutorial folder in “c:\”. As of this writing Windows still permits you to create directories in “c:\” but you are not permitted to place files there unless you Insist™. The restriction implies that “c:\” is *Windows’ own playground*, and it is, but there is an ages long tradition for placing installation directories there.
