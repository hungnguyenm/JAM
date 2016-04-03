/**
 * UDP Writer monitors shared payload queue then de-queues & sends it
 *
 * @author: Hung Nguyen
 * @version 1.0 04/03/16
 */

#include "../include/config.h"
#include "../include/payload.h"
#include "../include/udp_writer.h"

UdpWriter::UdpWriter() {

};

UdpWriter::~UdpWriter() {

};

void UdpWriter::operator()() {
    DCOUT("INFO: UdpWriter - Thread started.");

};

