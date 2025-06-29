# Emulsion Bench

This project is about the control and monitoring of an experimental bench for studies of emulsions between water and crude oil at the
[T2F (Thermal Flow Fluid Group)](https://t2f.ufsc.br/eng/index_eng.html) laboratory of [UFSC (Universidade Federal de Santa Catarina)](https://en.ufsc.br/). 

You can find the complete project at the following link: https://repositorio.ufsc.br/handle/123456789/243281

<p align="center">
<img src="https://i.imgur.com/esLbaOC.png" alt="drawing" width="1000"/>
</p>

## Design and Implementation
The user controls and monitors the experiments on a computer through [LabVIEW](https://www.ni.com/pt-br/support/downloads/software-products/download.labview.html#460283), which communicates with the [CompactDAQ](https://www.ni.com/pt-br/shop/compactdaq.html) for data acquisition and the [Tiva-C](https://www.ti.com/tool/EK-TM4C123GXL?utm_source=google&utm_medium=cpc&utm_campaign=epd-null-null-prodfolderdynamic-cpc-pf-google-wwe_int&utm_content=prodfolddynamic&ds_k=DYNAMIC+SEARCH+ADS&DCM=yes&gclid=Cj0KCQiA_bieBhDSARIsADU4zLe4EC0d31GO48MGUNpbr3e5-KMYJV8Ui7qZ0HeW3qAOminWc6kiey0aAsx3EALw_wcB&gclsrc=aw.ds) (TM4C123GH6PM) microcontroller to send commands.

The diagram below shows the communications and signals present in the experimental bench.

<p align="center">
<img src="https://imgur.com/Z9Ih7F7.png" alt="drawing" width="1000"/>
</p>


### UML Diagrams

* #### Use Case

<p align="center">
<img src="https://imgur.com/qEGRoH0.png" alt="drawing" width="600">   <img src="https://imgur.com/g2qIJmG.png" alt="drawing" width="600">
</p>

* #### Class Diagram (Firmware Tiva-C)

<p align="center">
<img src="https://imgur.com/rOYfy1p.png" alt="drawing" width="700"/>
</p>

## User Interface

<p align="center">
<img src="https://i.imgur.com/Y23VDEK.png" alt="drawing" width="500"/>
</p>

## Usage

This project is intend to use with [Code Composer Studio](https://www.ti.com/tool/CCSTUDIO), to [Tiva-C](https://www.ti.com/tool/EK-TM4C123GXL?utm_source=google&utm_medium=cpc&utm_campaign=epd-null-null-prodfolderdynamic-cpc-pf-google-wwe_int&utm_content=prodfolddynamic&ds_k=DYNAMIC+SEARCH+ADS&DCM=yes&gclid=Cj0KCQiA_bieBhDSARIsADU4zLe4EC0d31GO48MGUNpbr3e5-KMYJV8Ui7qZ0HeW3qAOminWc6kiey0aAsx3EALw_wcB&gclsrc=aw.ds) firmware development, and [LabVIEW](https://www.ni.com/pt-br/support/downloads/software-products/download.labview.html#460283) to user interface program.

## Support
If you want some help with this work contact me: guilherme.turatto@gmail.com

More information about the project: https://repositorio.ufsc.br/handle/123456789/243281
