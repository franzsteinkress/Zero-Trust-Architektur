## Logs ohne Farben exportieren
#docker-compose logs --no-color > raw_logs.txt
#
## Zeilen einlesen, nach dem Zeitstempel (der am Zeilenanfang steht) sortieren
## und in eine neue Datei schreiben
#Get-Content raw_logs.txt | Sort-Object { $_ } | Set-Content sorted_logs.txt
#
#Write-Host "Logs wurden erfolgreich sortiert in 'sorted_logs.txt' gespeichert." -ForegroundColor Green

## 1. Logs abrufen
#$rawLogs = docker-compose logs --no-color
#
## 2. Sortieren und als UTF-8 speichern
#$rawLogs | Sort-Object { 
#    $idx = $_.IndexOf("["); 
#    if($idx -ge 0) { $_.Substring($idx) } else { $_ } 
#} | Out-File -FilePath "sorted_logs.txt" -Encoding utf8
#
#Write-Host "Logs wurden in UTF-8 sortiert und gespeichert." -ForegroundColor Green

# 1. Logs abrufen
$rawLogs = docker-compose logs --no-color

# 2. Sortieren (basierend auf deiner Zeitstempel-Klammer)
$sortedLogs = $rawLogs | Sort-Object { 
    $idx = $_.IndexOf("["); 
    if($idx -ge 0) { $_.Substring($idx) } else { $_ } 
}

# 3. Speichern in UTF-8 OHNE BOM
# Wir erstellen ein UTF8-Objekt ohne Markierung
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllLines("$(Get-Location)\sorted_logs.txt", $sortedLogs, $utf8NoBom)

Write-Host "Check: sorted_logs.txt ist jetzt reines UTF-8 (ohne BOM) für VS Code!" -ForegroundColor Green

## 1. Logs als saubere UTF8-Strings abrufen (verhindert Fehlinterpretation durch PS-Standard)
#$rawLogs = docker-compose logs --no-color
#
## 2. Sortieren (basierend auf deinem Zeitstempel-Format)
#$sortedLogs = $rawLogs | Sort-Object { 
#    $idx = $_.IndexOf("["); 
#    if($idx -ge 0) { $_.Substring($idx) } else { $_ } 
#}
#
## 3. Absolut sicheres Schreiben in UTF-8 OHNE BOM
## Das verhindert '├ä' (Ä) und '┬░' (°)
#[System.IO.File]::WriteAllLines("$(Get-Location)\sorted_logs.txt", $sortedLogs, (New-Object System.Text.UTF8Encoding($false)))
#
#Write-Host "Fix angewendet: 'Identität' und '22.5°C' sollten jetzt korrekt sein." -ForegroundColor Cyan