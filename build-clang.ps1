cd $PSScriptRoot

if (-not (Test-Path -LiteralPath ./build-ninja)) {
    mkdir build-ninja
    cd build-ninja
    cmake -G Ninja -DCMAKE_C_COMPILER="Clang" -DCMAKE_CXX_COMPILER="Clang" ..
    cd ..
}

cd .\build-ninja
cmake --build .
$outcode = $LASTEXITCODE

cp compile_commands.json ..
cd ..

exit $outcode
