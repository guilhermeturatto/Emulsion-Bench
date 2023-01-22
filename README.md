# Emulsion Bench

This project is about the control of actuators in an experimental bench for emulsions between water and crude oil at the
[T2F (Thermal Flow Fluid Group)](https://t2f.ufsc.br/eng/index_eng.html) laboratory of [UFSC (Universidade Federal de Santa Catarina)](https://en.ufsc.br/). 

<p align="center">
<img src="https://i.imgur.com/wObW6H1.png" alt="drawing" width="1000"/>
</p>
<!-- ![Emulsion-Bench-Project](https://i.imgur.com/wObW6H1.png) -->

* The experimental bench must create different flow patterns and emulsions between water and crude oil, and it's composed of several measuring instruments and actuators.
For data acquisition, flow meters, pressure and temperature sensors are used with the National Instruments CompactDAQ (Data Acquisition System).
To regulate the experiments, an electro-pneumatic control valve is employed in conjunction with two motor pumps, controlled by frequency inverters.

* The user controls the experiments through LabVIEW on a computer, which communicates with the CompactDAQ for data acquisition and with the microcontroller to send commands.
The diagram below shows the communications and signals present in the bench.

<p align="center">
<img src="https://imgur.com/ulCbK7X.png" alt="drawing" width="1000"/>
</p>

<!-- ![Emulsion-Bench-Signals](https://imgur.com/ulCbK7X.png) -->

* The schematic of the hardware used in conjunction with the microcontroller is presented below.

<p align="center">
<img src="https://imgur.com/6xvSWnQ.png" alt="drawing" width="1000"/>
</p>

<!-- ![Emulsion-Bench-Hardware](https://imgur.com/6xvSWnQ.png) -->
