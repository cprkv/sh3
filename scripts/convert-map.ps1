$previousDir = $(Get-Location)

$prefixPath = $env:SH3_RESOURCES

try {
  Set-Location "$PSScriptRoot/../blender"
  ./.venv/Scripts/Activate.ps1
  python .\demo.py --input "$prefixPath\X0\MR1F-MFA\mr1f-pp.blend"
  python .\demo.py --input "$prefixPath\maps\debug\cs-arrows.blend"
}
finally {
  Set-Location $previousDir
}
