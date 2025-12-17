The ESp32 reads temp and hum from dht senosr.
OLED displays latest readings clearly on screen.
Data is sent to blynk app using virtual pins V0(temp) and v1(humidity).
A timer updates everything every 5 seconds.
A button on GPIO5 gives you a manual refresh anytime you press it.
The loop continuously runs Blynk, the timer, and checks the button for instant updates.
