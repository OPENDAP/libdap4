;; This builds an installer for libdap - it assumes that you have the
;; OPeNDAP Tools 1.0 (or later?) package installed in the default location,
;; that you have just built libdap from source and that the users of this
;; installer are interested in a binary distribution of libdap and will
;; build libdap itself.
;;
;; jhrg 24 April 2008

[Setup]
AppName=OPeNDAP libdap
AppVerName=OPeNDAP libdap 3.8.0
AppPublisher=OPeNDAP
DefaultDirName={sd}\opendap
DefaultGroupName=OPeNDAP libdap
AllowNoIcons=yes
InfoBeforeFile=BeforeInstall.txt
OutputBaseFilename=libdap 3.8.0
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
Source: "..\..\prepkg\opendap\libdap\dll\*"; DestDir: "{sys}"; Flags: ignoreversion

;; This gives the binary user the whole set of opendap tools which just includes a handful of source/header files
;; That they might not need, but then again they might.
;;Source: "C:\Program Files\opendap-tools\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\opendap-tools\dll\*"; DestDir: "{sys}"; Flags: ignoreversion

;; This somewhat inscrutable code sets the 'Path' environment variable so users can run getdap, et c.,
;; In a shell (cmd.exe).
[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "Path"; ValueData: "{olddata};{app}\bin"

[Icons]
Name: "{group}\{cm:UninstallProgram,libdap 3.8.0}"; Filename: "{uninstallexe}"



