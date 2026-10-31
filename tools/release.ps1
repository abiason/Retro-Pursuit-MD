param(
    [string]$Sdk = 'C:/sgdk190',
    [string]$Python = 'python',
    [switch]$SkipTests
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$make = Join-Path $Sdk 'bin/make.exe'
Push-Location $projectRoot
try {
    New-Item -ItemType Directory -Force out | Out-Null
    if (-not $SkipTests) { & "$PSScriptRoot/test.ps1" }
    $hashes = @()
    for ($pass = 1; $pass -le 2; $pass++) {
        & $make -f Makefile "SGDK=$Sdk" clean
        if ($LASTEXITCODE -ne 0) { throw "Clean failed: $pass" }
        & $make -f Makefile "SGDK=$Sdk" release 2>&1 | Tee-Object "out/release-build-$pass.log"
        if ($LASTEXITCODE -ne 0) { throw "Build failed: $pass" }
        & $Python tools/verify_rom.py out/rom.bin
        if ($LASTEXITCODE -ne 0) { throw "ROM validation failed: $pass" }
        $hashes += (Get-FileHash out/rom.bin -Algorithm SHA256).Hash.ToLowerInvariant()
    }
    if ($hashes[0] -ne $hashes[1]) { throw 'Clean builds differ' }
    $receipt = [ordered]@{
        sdk = $Sdk; clean_builds = 2; sha256 = $hashes[0]
        rom_bytes = (Get-Item out/rom.bin).Length
        tests_run = (-not $SkipTests)
    }
    $receipt | ConvertTo-Json | Set-Content out/release-build.json -Encoding utf8
    & (Join-Path $Sdk 'bin/size.exe') out/rom.out
    if ($LASTEXITCODE -ne 0) { throw 'Size report failed' }
    Write-Output "PASS: two identical clean ROM builds ($($hashes[0]))"
} finally { Pop-Location }
