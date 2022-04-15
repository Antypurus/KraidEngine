cd $PSScriptRoot

if (-not (Test-Path -LiteralPath ./build-ninja)) {
    mkdir build-ninja
    cd build-ninja
    cmake -G Ninja ..
    cd ..
}

cd .\build-ninja
cmake --build .
$outcode = $LASTEXITCODE

cp compile_commands.json ..
cd ..

exit $outcode
