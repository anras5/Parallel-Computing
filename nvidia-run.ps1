$env:SystemRoot = "C:\WINDOWS"
$env:SystemDrive = "C:"
$env:TEMP = "N:\sem_VI\PR\temp"

if (-not (Test-Path $env:TEMP)) {
    New-Item -ItemType Directory -Path $env:TEMP | Out-Null
}

$env:PATH += ";C:\WINDOWS\system32;C:\WINDOWS"

# Adjust the paths according to your version
$env:PATH += ";C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.1\bin"
$env:PATH += ";D:\Visual Studio\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64"
$env:LIB = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.1\lib\x64\"
$env:INCLUDE = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.1\include"

powershell.exe