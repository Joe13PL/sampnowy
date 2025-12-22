# Compile the gamemode locally using the bundled qawno/pawncc
$Qawno = Join-Path $PSScriptRoot '..\qawno\pawncc.exe'
$Main = Join-Path $PSScriptRoot '..\rp_openmp\main.pwn'
& $Qawno $Main -i"$PSScriptRoot\..\qawno\include" -o"$PSScriptRoot\..\rp_openmp.amx" -v2
