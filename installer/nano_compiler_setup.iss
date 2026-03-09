; nanoC++ 和 nanoPython 安装脚本
; 使用 Inno Setup 编译

#define MyAppName "nanoCompiler"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "nanoCompiler Team"
#define MyAppURL "https://github.com/nanocompiler"
#define MyAppExeName "nanoC++.exe"
#define MyAppPythonExeName "nanoPython.exe"

[Setup]
; 基本信息
AppId={{12345678-1234-1234-1234-123456789012}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\nanoCompiler
DefaultGroupName=nanoCompiler
DisableProgramGroupPage=yes
LicenseFile=..\LICENSE
OutputDir=..\installer\output
OutputBaseFilename=nanoCompiler_setup_v{#MyAppVersion}
SetupIconFile=..\resources\icon.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; 权限要求
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog

; 支持的 Windows 版本
MinVersion=6.1sp1
ArchitecturesAllowed=x86 x64
ArchitecturesInstallIn64BitMode=x64

; 安装程序界面
WizardImageFile=..\resources\installer_wizard.bmp
WizardSmallImageFile=..\resources\installer_wizard_small.bmp

[Languages]
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
; nanoC++ 主程序
Source: "..\nanoC++\build\Release\nanoC++.exe"; DestDir: "{app}\nanoC++"; Flags: ignoreversion
Source: "..\nanoC++\build\Release\*.dll"; DestDir: "{app}\nanoC++"; Flags: ignoreversion recursesubdirs

; nanoPython 主程序
Source: "..\nanoPython\build\Release\nanoPython.exe"; DestDir: "{app}\nanoPython"; Flags: ignoreversion
Source: "..\nanoPython\build\Release\*.dll"; DestDir: "{app}\nanoPython"; Flags: ignoreversion recursesubdirs

; TCC 编译器
Source: "..\third_party\tcc\*"; DestDir: "{app}\tcc"; Flags: ignoreversion recursesubdirs

; MicroPython 解释器
Source: "..\third_party\micropython\*"; DestDir: "{app}\micropython"; Flags: ignoreversion recursesubdirs

; 资源文件
Source: "..\nanoC++\resources\*"; DestDir: "{app}\nanoC++\resources"; Flags: ignoreversion recursesubdirs
Source: "..\nanoPython\resources\*"; DestDir: "{app}\nanoPython\resources"; Flags: ignoreversion recursesubdirs

; 示例代码
Source: "..\examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs

; 文档
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\docs\*"; DestDir: "{app}\docs"; Flags: ignoreversion recursesubdirs

[Icons]
; nanoC++ 快捷方式
Name: "{group}\nanoC++"; Filename: "{app}\nanoC++\nanoC++.exe"
Name: "{group}\nanoPython"; Filename: "{app}\nanoPython\nanoPython.exe"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

; 桌面快捷方式
Name: "{autodesktop}\nanoC++"; Filename: "{app}\nanoC++\nanoC++.exe"; Tasks: desktopicon
Name: "{autodesktop}\nanoPython"; Filename: "{app}\nanoPython\nanoPython.exe"; Tasks: desktopicon

; 快速启动
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\nanoC++"; Filename: "{app}\nanoC++\nanoC++.exe"; Tasks: quicklaunchicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\nanoPython"; Filename: "{app}\nanoPython\nanoPython.exe"; Tasks: quicklaunchicon

[Run]
; 安装后运行选项
Filename: "{app}\nanoC++\nanoC++.exe"; Description: "{cm:LaunchProgram,nanoC++}"; Flags: nowait postinstall skipifsilent
Filename: "{app}\nanoPython\nanoPython.exe"; Description: "{cm:LaunchProgram,nanoPython}"; Flags: nowait postinstall skipifsilent

[Registry]
; 文件关联 - C/C++ 文件
Root: HKA; Subkey: "Software\Classes\.c"; ValueType: string; ValueName: ""; ValueData: "nanoC.Source"; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.cpp"; ValueType: string; ValueName: ""; ValueData: "nanoC.Source"; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.h"; ValueType: string; ValueName: ""; ValueData: "nanoC.Header"; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.hpp"; ValueType: string; ValueName: ""; ValueData: "nanoC.Header"; Flags: uninsdeletevalue

; 文件关联 - Python 文件
Root: HKA; Subkey: "Software\Classes\.py"; ValueType: string; ValueName: ""; ValueData: "nanoPython.Source"; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.pyw"; ValueType: string; ValueName: ""; ValueData: "nanoPython.Source"; Flags: uninsdeletevalue

; nanoC++ 文件类型
Root: HKA; Subkey: "Software\Classes\nanoC.Source"; ValueType: string; ValueName: ""; ValueData: "C/C++ Source File"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\nanoC.Source\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\nanoC++\nanoC++.exe,0"
Root: HKA; Subkey: "Software\Classes\nanoC.Source\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\nanoC++\nanoC++.exe"" ""%1"""

Root: HKA; Subkey: "Software\Classes\nanoC.Header"; ValueType: string; ValueName: ""; ValueData: "C/C++ Header File"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\nanoC.Header\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\nanoC++\nanoC++.exe,1"
Root: HKA; Subkey: "Software\Classes\nanoC.Header\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\nanoC++\nanoC++.exe"" ""%1"""

; nanoPython 文件类型
Root: HKA; Subkey: "Software\Classes\nanoPython.Source"; ValueType: string; ValueName: ""; ValueData: "Python Source File"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\nanoPython.Source\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\nanoPython\nanoPython.exe,0"
Root: HKA; Subkey: "Software\Classes\nanoPython.Source\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\nanoPython\nanoPython.exe"" ""%1"""

; 添加到 PATH 环境变量
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\tcc;{app}\micropython"; Check: IsAdminInstallMode
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\tcc;{app}\micropython"; Check: not IsAdminInstallMode

[UninstallDelete]
Type: filesandordirs; Name: "{app}\nanoC++"
Type: filesandordirs; Name: "{app}\nanoPython"
Type: filesandordirs; Name: "{app}\tcc"
Type: filesandordirs; Name: "{app}\micropython"
Type: filesandordirs; Name: "{app}\examples"
Type: filesandordirs; Name: "{app}\docs"

[Code]
function InitializeSetup(): Boolean;
begin
  // 检查 Windows 版本
  if not IsWindowsVersionOrNewer(6, 1) then begin
    MsgBox('此程序需要 Windows 7 或更高版本。', mbError, MB_OK);
    Result := false;
  end else begin
    Result := true;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    // 安装完成后刷新环境变量
    RefreshEnvironmentVariables();
  end;
end;

procedure RefreshEnvironmentVariables();
var
  S: AnsiString;
begin
  // 通知系统环境变量已更改
  SetLength(S, 1);
  S[1] := #0;
  Windows.SendMessageTimeout($FFFF, $001A, 0, PAnsiChar(S), 2, 5000, nil);
end;
