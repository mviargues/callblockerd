/*
 callblocker - blocking unwanted calls from your home phone
 Copyright (C) 2015-2020 Patrick Ammann <pammann@gmx.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "AnalogPhone.h"
#include "Logger.h"

#define LOOP_WAIT_TIME_USEC (500 * 1000) // 500 milliseconds

static bool s_appRunning = true;
static bool s_appReloadConfig = false;

static void signal_handler(int signal)
{
    Logger::info("exiting (signal %i received)...", signal);
    s_appRunning = false;
}

class Main {
private:
    AnalogPhone m_analogPhone;

public:
    Main() : m_analogPhone()
    {
        Logger::start(false);
    }

    virtual ~Main()
    {
        Logger::stop();
    }

    /**
     * Start listening to the given device. Device is the name of the linux socker, e.g. "/dev/ttyACM0"
     */
    void start(std::string device)
    {
        if (!m_analogPhone.init(device)) {
            Logger::error("Failed to initialize the device %s", device.c_str());
            return;
        }

        Logger::debug("Main::loop() enter main loop...");
        while (s_appRunning) {
            m_analogPhone.run();

            (void)usleep(LOOP_WAIT_TIME_USEC);
        }
    }
};

int main(int argc, char* argv[])
{
    std::string defaultDevice = "/dev/ttyACM0"; // TODO: pass as an argument

    // register signal handler for break-in-keys (e.g. ctrl+c)
    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);
    // register signal handler for systemd shutdown request
    signal(SIGTERM, signal_handler);

    Logger::info("Starting callblockerd");

    Main m;
    m.start(defaultDevice);

    return 0;
}
