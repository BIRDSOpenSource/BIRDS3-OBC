
<p align="center">
<a href="https://lean-sat.org/opensource/" class="button icon calendar">Join Monthly Webinar</a>
</p>

<img  width="25%" src="https://github.com/BIRDSOpenSource/BIRDS3-ProceduresAndReports/assets/100206676/2d054074-dcd0-4ff8-a535-d025f32914fd"> <img width="40%" src="https://github.com/BIRDSOpenSource/BIRDS3-ProceduresAndReports/assets/100206676/c257e8cf-d65f-473c-98e1-21886804d913">


# BIRDS3-OBC
 Documentation for the On Board Computer used in the BIRDS3 Satellite


## About the BIRDS3 Program:

The project was led by Kyushu Institute of Technology in Japan and involves students from Sri Lanka, Bhutan, Nepal and Japan.
Satellite Names: NepaliSat-1 (Nepal), Raavana-1 (Sri Lanka) and Ugiusu (Japan)

Stakeholders: Nepal Academy of Science and Technology (NAST) for Nepal, Arthur C Clarke Institute for Modern Technologies (ACCIMT) for Sri Lanka, and Kyushu Institute of Technology (Kyutech) for Japan.

Project duration: 2018-2020

CubeSat: 1U (10x10x10) cm3

Weight: 1.13kg

Launch Date: 17th April 2019

BIRDS-3 has 4 main missions in total; LoRa Demonstration Mission (LDM), Imaging Mission (CAM), Attitude Determination and Control Mission (ADCS) and Software Configurable Backplane Mission (BPB) 

## Organisation structure

```bash

├── .gitattributes
├── BIRDS3_ComPIC_Firmware
│   ├── CMD_FUN.c
│   ├── COMPIC_CMMANDS.c
│   ├── COMPIC_FUN.c
│   ├── COMPIC_MAIN.c
│   ├── COMPIC_MAIN.ccspjt
│   ├── COMPIC_MAIN.hex
│   ├── COMPIC_MAIN_SRI.h
│   ├── COM_FLASH.c
│   ├── CWSRI.c
│   ├── PACKET_CODESRI.c
│   └── compicreg.h
├── BIRDS3_MainPIC_Firmware
│   ├── AUTO_MSN.c
│   ├── FLASH_OPERATION.c
│   ├── MEMORY_OPERATION.c
│   ├── PIC16F1787_REG.h
│   ├── PIC18F67J94_registers.h
│   ├── SATELLITE_LOG.c
│   ├── analyze_command.c
│   ├── flash_ memory_ MT25QL01GBBB_OF.c
│   ├── main pic1.h
│   ├── main_pic.c
│   ├── main_pic.ccspjt
│   ├── main_pic.hex
│   ├── mainpic_function.c
│   ├── mainpic_function.h
│   └── reserve_function.c
├── BIRDS3_ResetPIC_Firmware
│   ├── PIC16F1789_registers.h
│   ├── define_fun.c
│   ├── power_enable.c
│   ├── read_current.c
│   ├── reset_fm_v1p2.c
│   ├── reset_fm_v1p2.ccspjt
│   └── reset_fm_v1p2.hex
├── LICENSE
├── OBC_Partslist.xlsx
├── OBC_Schematic.pdf
└── README.md

```

## Features


## I like this project, can I help you?
You are welcome 🙂

* Give us a star ⭐
* Watch repository to be notified about updates 👀
* If you will find some errors, report them in Issues 🐞
* Try Birds designs on some real computer and let us know how it worked in Discussions. 💬
* We would greatly appreciate if you would also tell others about this ecosystem, it helps us to improve BIRDS Open Source. Thank you!
