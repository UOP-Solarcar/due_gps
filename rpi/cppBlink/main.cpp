#include <csignal>
#include <gpiod.hpp>
#include <unistd.h>

volatile sig_atomic_t stop;

void signal_callback_handler(int signum) { stop = 1; }

int main() {
    gpiod::chip chip("gpiochip0");
    auto line = chip.get_line(17); // GPIO17
    line.request({"blink", gpiod::line_request::DIRECTION_OUTPUT, 0}, 1);

    signal(SIGINT, signal_callback_handler);

    while (!stop) {
        sleep(1);
        line.set_value(0);
        sleep(1);
        line.set_value(1);
    }
    line.release();
    return EXIT_SUCCESS;
}
