#from __future__ import division
import spidev
import time

def bitstring(n):
    s = bin(n)[2:]
    return '0'*(8-len(s)) + s

def read(adc_channel=0, spi_channel=0):
    conn = spidev.SpiDev(0, spi_channel)
    conn.max_speed_hz = 3000000 # 1.2 MHz
    cmd = 128
    if adc_channel:
        cmd += 32
    reply_bytes = conn.xfer2([cmd, 0])
    reply_bitstring = ''.join(bitstring(n) for n in reply_bytes)
    reply = reply_bitstring[5:15]
    #return int(reply, 2) / 2**10
    return int(reply, 2)

if __name__ == '__main__':
	current_time = time.time()
	end_time = current_time + 1.0
	while(time.time() < end_time):
		print read()
#	time.sleep(0.00016666667)
