$repositoryRoot = "$PSScriptRoot/.."
$generate = "$repositoryRoot/deps/hlsl-meta-generator/generate.js"

$version = "5.0"
$namespace = "core::render::shaders"
$srcDir = "$repositoryRoot/deps/data/shaders"
$outDir = "$repositoryRoot/core/core/render/shaders"

node $generate $version $namespace $srcDir $outDir
