#import "ViewController.h"

@interface ViewController ()
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Termal durum değişikliği bildirimlerini dinle
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(thermalStateChanged)
                                                 name:NSProcessInfoThermalStateDidChangeNotification
                                               object:nil];
    
    // Mevcut termal durumu kontrol et
    [self checkThermalState];
}

- (void)thermalStateChanged {
    [self checkThermalState];
}

- (void)checkThermalState {
    NSProcessInfoThermalState thermalState = [[NSProcessInfo processInfo] thermalState];
    
    switch (thermalState) {
        case NSProcessInfoThermalStateNominal:
            NSLog(@"Cihaz normal sıcaklıkta çalışıyor.");
            break;
        case NSProcessInfoThermalStateFair:
            NSLog(@"Cihaz hafif bir ısınma durumu algıladı.");
            break;
        case NSProcessInfoThermalStateSerious:
            NSLog(@"Cihaz ciddi bir ısınma durumu algıladı. İşlem yükünü azaltmayı düşünün.");
            break;
        case NSProcessInfoThermalStateCritical:
            NSLog(@"Cihaz aşırı ısındı! Acil önlem alınması gerekiyor.");
            break;
        default:
            NSLog(@"Bilinmeyen termal durum algılandı.");
            break;
    }
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end