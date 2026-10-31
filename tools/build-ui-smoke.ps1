param([string]$Sdk = 'C:/sgdk190', [switch]$Campaign)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$smokeRoot = Join-Path $projectRoot 'out/ui-smoke'
$smokeSource = Join-Path $smokeRoot 'src'
New-Item -ItemType Directory -Force $smokeSource | Out-Null
foreach ($sourceItem in Get-ChildItem -LiteralPath (Join-Path $projectRoot 'src') -Force) {
    Copy-Item -LiteralPath $sourceItem.FullName -Destination $smokeSource -Recurse -Force
}
Copy-Item -LiteralPath (Join-Path $projectRoot 'tests/rom_ui_smoke.c') -Destination (Join-Path $smokeSource 'main.c') -Force
if ($Campaign) {
    $harnessPath = Join-Path $smokeSource 'main.c'
    $harnessText = Get-Content -LiteralPath $harnessPath -Raw
    Set-Content -LiteralPath $harnessPath -Value ("#define RP_CAMPAIGN_SMOKE 1`n" + $harnessText) -Encoding ascii
}
# Mode switches change staged main.c even when Copy-Item restores an older source timestamp.
(Get-Item -LiteralPath (Join-Path $smokeSource 'main.c')).LastWriteTime = Get-Date
Copy-Item -LiteralPath (Join-Path $projectRoot 'Makefile') -Destination (Join-Path $smokeRoot 'Makefile') -Force
& (Join-Path $Sdk 'bin/make.exe') -C ($smokeRoot -replace '\\', '/') -f Makefile "SGDK=$Sdk" release
if ($LASTEXITCODE -ne 0) { throw 'UI smoke ROM compilation failed' }
$artifactName = if ($Campaign) { '00-campaign-smoke.bin' } else { '00-ui-smoke.bin' }
Copy-Item -LiteralPath (Join-Path $smokeRoot 'out/rom.bin') -Destination (Join-Path $projectRoot "out/$artifactName") -Force
Write-Output "Visual-test ROM: out/$artifactName (production out/rom.bin unchanged)"
