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

#include "AnalogPhone.h" // API

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "Logger.h"
#include "Utils.h"

/*
  AT commands:
  http://support.usr.com/support/5637/5637-ug/ref_data.html
*/
#define AT_Z_STR "AT Z S0=0 E0 Q0 V1"
#define AT_CID_STR "AT+VCID=1"
#define AT_HANGUP_STR "ATH0"
#define AT_PICKUP_STR "ATH1"

#define RING_SILENCE_TIME_SEC 7 // when ringing each 5s a RING is expected -> 7s should be long enough
#define PICKUP_HANGUP_TIME_SEC 2 // pickup and then hangup after 2s

AnalogPhone::AnalogPhone()
{
    Logger::debug("AnalogPhone::AnalogPhone()...");
    m_numRings = 0;
    m_foundCID = false;
}

AnalogPhone::~AnalogPhone()
{
    Logger::debug("AnalogPhone::~AnalogPhone()...");
}


bool AnalogPhone::blockNumber(const std::string number) {
    std::string res;
    // TODO: externalize URL
    bool succeeded = Utils::executeCommand("curl http://localhost/api/call?number=" + number, &res);
    if (!succeeded) {
        Logger::error("Failed to access the API");
        return false;
    }

    return res == "blocked=true";
}

bool AnalogPhone::init(const std::string device)
{
    Logger::debug("AnalogPhone::init(%s)...", device.c_str());

    if (!m_modem.open(device)) {
        return false;
    }

    if (!m_modem.sendCommand(AT_Z_STR)) {
        return false;
    }
    if (!m_modem.sendCommand(AT_CID_STR)) {
        return false;
    }

    Logger::info("AnalogPhone::init: successfully initialized device %s", device.c_str());
    return true;
}

// load this into a seperate thread, needed for LiveAPI access, which may take some time...,
// or offload LiveAPI access itself into a seperate thread? YES?
void AnalogPhone::run()
{
    std::string data;
    if (m_modem.getData(&data)) {
        if (data == "RING") {
            m_ringTimer.restart(RING_SILENCE_TIME_SEC);
            m_numRings++;

            if (m_numRings == 1) {
                Logger::debug("State changed to RINGING");
            }

            // handle no caller ID
            if (m_numRings == 2) {
                if (!m_foundCID) {
                    Logger::warn("Not expecting second RING without receiving caller ID");
                }
            }
        } else {
            // between first and second RING:
            // DATE=0306
            // TIME=1517
            // NMBR=0123456789
            // NAME=aasdasdd

            //Logger::debug("CID(orig): '%s'", data.c_str());

            bool block = false;
            std::string number = "none";
            std::string name = "";

            std::vector<std::pair<std::string, std::string>> result;
            Utils::parseCallerID(data, &result);
            for (auto& it : result) {
                std::string key = it.first;
                std::string value = it.second;
                Logger::debug("CID(tok): '%s=%s'", key.c_str(), value.c_str());

                if (key == "NMBR") {
                    number = value;
                } else if (key == "NAME") {
                    name = value;
                }
            } // for

            block = blockNumber(number);
            Logger::info("Blocking number? %s", block ? "yes" : "no");

            // if (block) {
            //     m_modem.sendCommand(AT_PICKUP_STR); // pickup
            //     m_hangupTimer.restart(PICKUP_HANGUP_TIME_SEC);
            // }
        }
    }

    // detect "ringing stopped"
    if (m_ringTimer.isActive() && m_ringTimer.hasElapsed()) {
        Logger::debug("State changed to RINGING_STOPPED");
        m_ringTimer.stop();
        m_numRings = 0;
        m_foundCID = false;
    }

    // hangup
    if (m_hangupTimer.isActive() && m_hangupTimer.hasElapsed()) {
        m_hangupTimer.stop();
        m_modem.sendCommand(AT_HANGUP_STR); // hangup
        Logger::debug("State changed to HANGUP");
    }
}
