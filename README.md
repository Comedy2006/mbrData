# mbrData
**DISCLAIMER:** This program still contains major security issues (memory management, etc.). For now, only the Windows version is available (I'll do a Linux version too).<br>
<ul>
<li> [Documentation](README.md#Documentation) </li>
<li> [Attributes](README.md#Attributes) </li>
<li> [Syntax](README.md#Syntax) </li>
<li> [Changelog](README.md#Changelog) </li>
</ul>
## Documentation
Btw you need to run this program in admin mode, I messed up the manifest...
### Attributes
The current version (1.0.0) contains the following attributes:<br>
`-a`: program automatically tries to determine the bootable drive<br>
`-c`: make a custom scan of the first 512 Bytes of your physical drive<br>
`-s`: if you want to save the output to a file of your desire<br>
`-t`: if you want to directly output the first 512 Bytes on your terminal<br>
### Syntax
`-a -s filename(and extension)`<br>
`-a -t`<br>
`-c -s filename(and extension) (number of the physical disk)`<br>
`-c -t (number of the physical disk)`<br>

Example:<br>
`file.exe -a -s output.txt`<br>
`file.exe -c -s output.txt 0` (0 stands for \\\\.\\PhysicalDrive0)<br>

## Changelog
Version 1.0.0: Initial release<br>
Plans for next update: fix the Manifest file so I can run this program in admin mode
