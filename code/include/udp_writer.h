/**
 * UDP Writer monitors shared payload queue then de-queues & sends it
 *
 * @author: Hung Nguyen
 * @version 1.0 04/03/16
 */

#ifndef JAM_UDP_WRITER_H
#define JAM_UDP_WRITER_H

#include <iostream>

class UdpWriter {
public:
    UdpWriter();

    ~UdpWriter();

    void operator()();

private:
};

#endif //JAM_UDP_WRITER_H
