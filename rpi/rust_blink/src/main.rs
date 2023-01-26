use std::error::Error;
use std::thread;
use std::time::Duration;

use rppal::gpio::Gpio;

// Gpio uses BCM pin numbering. BCM GPIO 23 is tied to physical pin 16.
const GPIO_LED: u8 = 17;

fn main() -> Result<(), Box<dyn Error>> {
    let mut pin = Gpio::new()?.get(GPIO_LED)?.into_output();

    pin.set_high();
    loop {
        thread::sleep(Duration::from_millis(500));
        pin.set_low();
        thread::sleep(Duration::from_millis(500));
        pin.set_high();
    }
}
