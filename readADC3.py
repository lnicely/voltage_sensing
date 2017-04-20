import spidev
import time

spi = spidev.SpiDev()
spi.open(0,0)

def read_spi(channel):
    spidata = spi.xfer2([96,0])
    print("Raw ADC: {}".format(spidata))
    data = ((spidata[0] & 3) << 8) + spidata[1]
    return data

try:
    current_time = time.time()
    end_time = current_time + 0.05
    while(time.time() < end_time):
        channeldata = read_spi(1)
        voltage = round(((channeldata * 3300) / 1024),0)
        print("Data (dec) {}".format(channeldata))
        print("Data (bin) {:010b}".format(channeldata))
        print("Voltage (mV): {}".format(voltage))

except KeyboardInterrupt:
    spi.close()
