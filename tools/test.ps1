$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
Push-Location $projectRoot
try {
    New-Item -ItemType Directory -Force out | Out-Null
    $sources = @('src/core/game.c', 'src/entities/player.c',
        'src/systems/physics.c', 'src/systems/animation.c',
        'src/systems/collision.c', 'src/world/level.c', 'src/world/navigation.c', 'src/entities/enemy.c', 'src/core/game_session.c', 'src/systems/score.c', 'src/systems/timer.c', 'src/entities/obstacle.c', 'src/core/scene_manager.c', 'src/world/stage.c', 'src/systems/audio_sequencer.c')
    foreach ($suite in @('foundation', 'collision', 'world', 'enemy', 'score', 'session', 'obstacle', 'ui', 'stage', 'audio')) {
        $outputPath = "out/test_$suite.exe"
        & gcc -std=c99 -Wall -Wextra -Werror -Isrc "tests/test_$suite.c" @sources -o $outputPath
        if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $suite" }
        & (Join-Path $projectRoot $outputPath)
        if ($LASTEXITCODE -ne 0) { throw "Test failed: $suite" }
    }
} finally {
    Pop-Location
}
