;; This builds an installer for libdap - it assumes that you have the
;; OPeNDAP Tools 1.0 (or later) package installed in the default location,
;; that you have just built libdap from source and that the users of this
;; installer are interested in a binary distribution of libdap and will
;; build libdap itself.
;;
;; jhrg 24 April 2008

[Setup]
AppName=OPeNDAP libdap
AppVerName=OPeNDAP libdap 3.8.2
AppPublisher=OPeNDAP
DefaultDirName={sd}\opendap
DefaultGroupName=OPeNDAP libdap
AllowNoIcons=yes
InfoBeforeFile=BeforeInstall.txt
OutputBaseFilename=libdap_3.8.2
Compression=lzma/ultra
SolidCompression=yes
LicenseFile=License.txt
AlwaysRestart=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
;; This grabs the newly built libdap code _assuming_ that the libdap.iss file is run from within libdap/VCPP and
;; that the 'nmake package' target has just been run.
Source: "..\..\prepkg\opendap\libdap\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

;; Put all of the DLLs in the opendap\bin directory - I have no idea why putting the DLLs in a \dll directory
;; doesn't work, but on a non-development machine I set up it definitely does not work while putting them in
;; the \bin directory does.
Source: "..\..\prepkg\opendap\libdap\dll\*"; DestDir: "{app}\bin"; Flags: ignoreversion

;; DLLs needed to run our code that are usually not part of win32. Developers should get the OPeNDAP-Tools dist
;; too. I spilt this up because I figure developers can handle two installs and so that we can update the tools
;; (bison, et c.) and libdap separately.
Source: "C:\opendap-tools\dll\*"; DestDir: "{app}\bin"; Flags: ignoreversion

;; These are the VC++ DLLs needed to actually run the code
Source: "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs

;; This somewhat inscrutable code sets the 'Path' environment variable so users can run getdap, et c.,
;; in a shell (cmd.exe).
[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "Path"; ValueData: "{olddata};{app}\bin"

[Icons]
Name: "{group}\{cm:UninstallProgram,libdap 3.8.2}"; Filename: "{uninstallexe}"



