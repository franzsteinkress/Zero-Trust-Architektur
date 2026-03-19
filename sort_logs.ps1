<#
@file sort_logs.ps1
@brief Analyse-Tool für die Log-Aufbereitung.
@details Liest Docker-Logs ein, erzwingt UTF-8 und sortiert Einträge 
chronologisch unter Beibehaltung des Kontextes (Folgezeilen ohne Zeitstempel).
#>

# ==============================================================================
# sort_logs.ps1 - Zero-Trust-Architektur Log-Processor (Context-Aware)
# ==============================================================================

$OutputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "--- Starte Log-Extraktion und Kontext-Sortierung ---" -ForegroundColor Cyan

# 1. Logs von Docker ziehen
$rawOutput = docker-compose logs --no-color

# 2. Kontext-Verarbeitung
Write-Host "--- Analysiere Zeitstempel-Bloecke ---" -ForegroundColor Yellow

$currentTimestamp = "00:00:00.000000000"
$processedLogs = $rawOutput | ForEach-Object {
    $line = $_
    
    # Extrahiere Zeitstempel mit Regex: [HH:mm:ss.ms]
    if ($line -match "\[(\d{2}:\d{2}:\d{2}\.\d+)\]") {
        $currentTimestamp = $Matches[1]
    }
    
    # Erstelle ein Objekt, das den Zeitstempel "mitschleppt"
    [PSCustomObject]@{
        SortKey = $currentTimestamp
        Content = $line
    }
}

# 3. Filtern (Leere Zeilen raus) und Sortieren
$sortedLogs = $processedLogs | Where-Object {
    # Filtert Zeilen, die nach dem '|' wirklich leer sind
    $_.Content -match "\|\s*\S+"
} | Sort-Object SortKey | ForEach-Object { $_.Content }

# 4. Speichern (UTF-8 ohne BOM)
$utf8 = New-Object System.Text.UTF8Encoding($false)
$outputPath = Join-Path (Get-Location) "sorted_logs.txt"

if ($sortedLogs) {
    [System.IO.File]::WriteAllLines($outputPath, $sortedLogs, $utf8)
    Write-Host "Done! Kontext-erhaltende Sortierung abgeschlossen." -ForegroundColor Green
}