### I2C tutorial

I2C protocol is a way of communication between master and slave devices which are usually low speed. It's easy to implement with two wires 
and there can be more than one master and one slave device.

Each slave device has a unique address for a master to differentiate whom it's writing to and reading from. All devices are connected with two wires Serial clock line and Serial data line.(SCK,SDA) Each wire is connected with a pull-up resistor. 

I2C bus devices pull-down the voltage on the bus instead of using their own operating voltage. 
This allows devices with different operating voltages to be connected but only if the lower voltage is still readable
by the higher-voltage device. 

For example, it’s OK to connect a 3.3V I2C sensor to a 5V Arduino because the latter can still read 3.3V. 
Therefore, as our camera requires 3.3V for pull up but Arduino gives 5V pull-up which will destroy the camera but with disabled internal pull-up, Arduino could still read from a 3.3V camera. 

![I2C scheme](https://snag.gy/oF6LuH.jpg)

![I2C](https://snag.gy/QDwcP1.jpg)
The picture indicates how master and Slave communicate with SCK and SDA.
1. A start signal is sent from the master device. SDA go low before SCL goes high
2. 7-bit address is the first 7 most significant bits in the first byte and 8th bit is for read/write.'
3. After each byte, an acknowledgment bit is sent from slave to master. It's high if the slave has a response and low if no response.
4. If read/write mode didn't change, then the following bytes after the address is data. 
5. If we want to terminate the read/write, the master sends a pause signal after the last acknowledgment 
