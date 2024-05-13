$date = Get-Date -Format "yyyy.MM.dd"
$projectRoot = "$PSScriptRoot/.."
$dataDir = "$projectRoot/deps/data"
$demoDir = "$projectRoot/demos/sh-$date"

Remove-Item -Recurse -Force "$demoDir"
Remove-Item -Recurse -Force "$demoDir.7z"
New-Item -ItemType Directory -Path "$demoDir" -Force
New-Item -ItemType Directory -Path "$demoDir/data/maps/mall-real" -Force
New-Item -ItemType Directory -Path "$demoDir/data/shaders" -Force

Copy-Item "$projectRoot/bin/release/game-cpp.exe" "$demoDir/sh3.exe"
Copy-Item "$dataDir/maps/mall-real/mall-real-split.chunk" "$demoDir/data/maps/mall-real"
Copy-Item "$dataDir/maps/mall-real/mall-real-split.scene.json" "$demoDir/data/maps/mall-real"
Copy-Item "$dataDir/maps/mall-real/mall-real-split.scene.json" "$demoDir/data/maps/mall-real"
Copy-Item "$dataDir/shaders/*.hlsl" "$demoDir/data/shaders"

Set-Content -Path "$demoDir/project-config.json" -Value @'
{
  "gameData": "data"
}
'@

7z a "$demoDir.7z" "$demoDir" -mx6

Invoke-Item "$projectRoot/demos"
Write-Output "done!"
