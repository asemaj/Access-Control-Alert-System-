# Access Control & Alert System


# Introduction​

In an era where security plays a crucial role in safeguarding our surroundings, the development and integration of advanced technologies becomes important. The purpose of this system is to keep things secure – only letting in those with the right fingerprint and catching anyone unauthorized with a snapshot from the camera triggered by the motion sensor. If your fingerprint checks out, the motion sensor takes a break, ensuring that only the right people are in the right places. It's all about making security smart and effective. Stick around, and we'll break down how our system works and why it's a game-changer for keeping spaces safe and sound.​


## Design ​

The design of our system was created to be more efficient in processing power and pin counts. We found that other designs had limited use and functionalities, such as using an Arduino Uno, which lacks WiFi communication capabilities.​

​![circuit](https://github.com/asemaj/Access-Control-Alert-System-/assets/97552073/bf0f4c4e-3563-42b3-aa87-9a937dc381fb)

Figure 1 Electrical Circuit.​

As shown in Figure 1, a 12V battery was used to connect the grounds and Voltages to the components through an AC connector. The fingerprint sensor's (R307) TX and RX was connected to GPIO 1 and GPIO 3 on the esp32. The Motion sensor shown on the right was also connected to the esp32 on Pin GPIO 36 as an input, following the connection of the cam where it took the output pin, GPIO 27 as an input to pin GPIO 13 on the esp32 Cam.​

​ ![77778](https://github.com/asemaj/Access-Control-Alert-System-/assets/97552073/555495dc-e04f-4ede-8b91-1484e738dc57)

Figure 2: Flowchart Diagram​

Figure 2 illustrates the flow chart that begins with fingerprint reading. If the fingerprint is not verified, the PIR sensor will look for motion. If motion is detected, it will capture an image; if not, it will stay on standby. If the fingerprint is verified, it will cause the PIR sensor to sleep and thus open the door.​


# Result​

By activating the system the fingerprint sensor will wait for a valid fingerprint, if found it will turn off the PIR sensor. And if a fingerprint was not valid or none was being shown to the sensor the PIR sensor will stay active.​

​​![System-Demo](https://github.com/asemaj/Access-Control-Alert-System-/assets/97552073/b210e555-b5c6-41f9-a85e-55697e0f9c18)

Figure 3: System Demo.​

The following contains 2 sensors (fingerprint and a motion sensor) and 3 microcontrollers (esp32 WROOM, esp32 cam, and an arduino uno).​

​![223322](https://github.com/asemaj/Access-Control-Alert-System-/assets/97552073/892f6a93-017b-4562-a763-0ebe82671e38)

Figure 4: PIR Sensor Detection.​


   Once an object is detected and has passed through the two zones, the high change in signals will result in a high signal that will be sent to the sensor indicting that an object has been detected. The info is then sent to the esp32 Cam to capture the intruder in case of a not verified fingerprint. (Figure 5)​


​![443344](https://github.com/asemaj/Access-Control-Alert-System-/assets/97552073/4e45877a-0314-4c88-9cdc-42e2b43c0527)

Figure 5: SMTP Email Client. ​

  If the person has entered the premises without a fingerprint verification, the motion will continue to stay on and detect any object in the vicinity and will result in a captured image. The image will then be sent immediately to the designated person. (Figure 5)​

# Conclusion​

In conclusion, our security system represents a practical response to the contemporary imperative of ensuring safety. By seamlessly integrating fingerprint recognition and motion-sensor-triggered cameras, the system addresses the fundamental need for security without unnecessary complexity. It goes beyond technological intricacies to offer a straightforward solution to real-world security challenges. The broader significance lies in its potential to reshape conventional approaches to security, emphasizing practicality over extravagance. In a landscape where safety is paramount, our system provides a reliable and uncomplicated means of safeguarding spaces. As we navigate the evolving landscape of security, our focus remains on delivering effective solutions that prioritize functionality and user-friendliness, contributing to a safer and more secure environment.
