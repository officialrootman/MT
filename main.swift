import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Termal durumu izlemeye başla
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(handleThermalStateChange),
            name: ProcessInfo.thermalStateDidChangeNotification,
            object: nil
        )

        // Mevcut termal durum
        checkThermalState()
    }

    @objc func handleThermalStateChange() {
        checkThermalState()
    }

    func checkThermalState() {
        let thermalState = ProcessInfo.processInfo.thermalState

        switch thermalState {
        case .nominal:
            print("Cihaz normal sıcaklıkta çalışıyor.")
        case .fair:
            print("Cihaz hafif bir ısınma durumu algıladı.")
        case .serious:
            print("Cihaz ciddi bir ısınma durumu algıladı. İşlem yükünü azaltmayı düşünün.")
        case .critical:
            print("Cihaz aşırı ısındı! Acil önlem alınması gerekiyor.")
        @unknown default:
            print("Bilinmeyen termal durum algılandı.")
        }
    }

    deinit {
        NotificationCenter.default.removeObserver(self)
    }
}