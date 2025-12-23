# Compile the gamemode locally using the bundled qawno/pawncc
# NOTE: qawno is at the workspace root, so jump up an extra level from gamemodes\scripts
$Qawno = Join-Path $PSScriptRoot '..\..\qawno\pawncc.exe'
$Main = Join-Path $PSScriptRoot '..\rp_openmp\main.pwn'
& $Qawno $Main -i"$PSScriptRoot\..\..\qawno\include" -o"$PSScriptRoot\..\rp_openmp.amx" -v2
