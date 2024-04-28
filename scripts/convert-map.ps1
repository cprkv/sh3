$previousDir = $(Get-Location)

try {
  Set-Location "$PSScriptRoot/../blender"
  ./.venv/Scripts/Activate.ps1
  python .\demo.py --input "F:\sh3-tools\RESOURCES1\X0\MR1F-MFA\mr1f-pp.blend"
}
finally {
  Set-Location $previousDir
}
