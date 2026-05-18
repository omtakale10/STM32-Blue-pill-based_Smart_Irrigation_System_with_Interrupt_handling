# STM32-Blue-pill-based-Smart-Irrigation-System-with-Interrupt-handling.
Automated Irrigation System built on STM32F103C8T6 using Bare-Metal C. Monitors soil moisture via 12-bit ADC and detects rain using EXTI interrupt to control a water pump automatically. Implemented with register-level programming (no HAL), combining polling + interrupt architecture for efficient, real-time water management.

❖ Flowchart 

<img width="787" height="1169" alt="image" src="https://github.com/user-attachments/assets/15251ff1-d08d-4322-adef-55fecad8fbaf" />

❖ INTERFACING DIAGRAM 

<img width="975" height="582" alt="image" src="https://github.com/user-attachments/assets/777af48f-37d7-4bb0-a56b-3c0ec4f518ae" />


❖ LEARNINGS FORM THE PROJECT 

•	Understood how STM32 peripherals work using Bare-Metal C and register-level programming. 

•	Learned configuring GPIO pins as input, output, analog, and pull-up manually. 

•	Gained experience enabling peripheral clocks using the RCC registers. 

•	Learned ADC configuration, channel selection, sampling time, and reading sensor data.

•	Learned to configure external interrupts (EXTI) and map pins using AFIO registers. 

•	Gained experience writing and handling interrupt service routines (ISR). 

•	Understood interfacing of soil moisture sensor (ADC) and rain sensor (digital interrupt). 

•	Learned how to control a relay/water pump using GPIO output logic. 

•	Learned combining polling + interrupt-driven control for reliable automation. 

•	Improved debugging and problem-solving using step-by-step hardware logic analysis. 
