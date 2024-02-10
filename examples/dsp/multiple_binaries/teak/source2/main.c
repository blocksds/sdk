// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <teak/teak.h>

int main(void)
{
    teakInit();

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0);
        uint16_t data1 = apbpReceiveData(1);
        uint16_t data2 = apbpReceiveData(2);

        apbpSendData(0, data0);
        apbpSendData(1, -data1);
        apbpSendData(2, -data2);
    }

    return 0;
}
