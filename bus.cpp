#include "bus.h"

void SnoopBus::SendMessage(BusMessage msg, uint64_t addr) {
    if (msg == BusMessage::BUSRD) {

    } else if (msg == BusMessage::BUSRDX) {

    } else if (msg == BusMessage::BUSUPGR) {

    }
} 