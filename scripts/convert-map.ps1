$previousDir = $(Get-Location)

$prefixPath = $env:SH3_RESOURCES

if ($prefixPath.Length -eq 0) {
  Write-Output "Error: environment variable SH3_RESOURCES is not set!"
  exit
}

try {
  Set-Location "$PSScriptRoot/../blender"
  ./.venv/Scripts/Activate.ps1
  python .\process.py --input "$prefixPath\maps\mall-real\mall-real-split.blend"
  # python .\process.py --input "$prefixPath\X0\MR1F-MFA\mr1f-pp.blend"
  # python .\process.py --input "$prefixPath\maps\debug\cs-arrows.blend"
}
finally {
  Set-Location $previousDir
}
