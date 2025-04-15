# Ağ Trafiği Analiz Aracı (İSH Shell)

# Renkli çıktı için tanımlar
$RED = "`e[0;31m"
$GREEN = "`e[0;32m"
$YELLOW = "`e[1;33m"
$NC = "`e[0m" # No Color

# Ana Menü Fonksiyonu
function Show-Menu {
    Write-Host -ForegroundColor Green "=== Ağ Trafiği Analiz Aracı ==="
    Write-Host "1) Belirli bir IP adresini filtrele"
    Write-Host "2) Belirli bir portu filtrele"
    Write-Host "3) Tüm trafiği kaydet"
    Write-Host "4) Çıkış"
    $global:choice = Read-Host "Seçiminizi yapın"
}

# Trafiği analiz et fonksiyonu
function Analyze-Traffic {
    param (
        [string]$Filter
    )
    Write-Host -ForegroundColor Yellow "Trafik analiz ediliyor..."
    Write-Host -ForegroundColor Green "tcpdump komutu: tcpdump $Filter"
    
    # tcpdump çağrısı
    Start-Process "tcpdump" -ArgumentList "$Filter -n -c 100" -NoNewWindow -Wait
}

# Trafiği kaydet fonksiyonu
function Save-Traffic {
    param (
        [string]$Filter
    )
    $outputFile = "trafik_analizi_$((Get-Date -Format 'yyyyMMdd_HHmmss')).pcap"
    Write-Host -ForegroundColor Yellow "Trafik kaydediliyor..."
    Write-Host -ForegroundColor Green "tcpdump komutu: tcpdump $Filter -w $outputFile"
    
    # tcpdump çağrısı
    Start-Process "tcpdump" -ArgumentList "$Filter -w $outputFile" -NoNewWindow -Wait
    Write-Host -ForegroundColor Green "Trafik $outputFile dosyasına kaydedildi."
}

# Ana döngü
while ($true) {
    Show-Menu
    switch ($choice) {
        "1" {
            $ip = Read-Host "İzlenecek IP adresini girin"
            Analyze-Traffic -Filter "host $ip"
        }
        "2" {
            $port = Read-Host "İzlenecek portu girin"
            Analyze-Traffic -Filter "port $port"
        }
        "3" {
            Write-Host "Kaydedilecek trafiği seçin:"
            Write-Host "1) Tüm trafik"
            Write-Host "2) Belirli bir IP adresi"
            Write-Host "3) Belirli bir port"
            $saveChoice = Read-Host "Seçiminizi yapın"

            switch ($saveChoice) {
                "1" {
                    Save-Traffic -Filter ""
                }
                "2" {
                    $saveIP = Read-Host "Kaydedilecek IP adresini girin"
                    Save-Traffic -Filter "host $saveIP"
                }
                "3" {
                    $savePort = Read-Host "Kaydedilecek portu girin"
                    Save-Traffic -Filter "port $savePort"
                }
                default {
                    Write-Host -ForegroundColor Red "Geçersiz seçim!"
                }
            }
        }
        "4" {
            Write-Host -ForegroundColor Green "Çıkış yapılıyor..."
            break
        }
        default {
            Write-Host -ForegroundColor Red "Geçersiz seçim!"
        }
    }
}