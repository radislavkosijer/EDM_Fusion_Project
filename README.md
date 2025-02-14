# EDM Fusion Project
---
Project assignment for the course **Digital Signal Processing Systems**. <br>
This project assignment was developed in the CrossCore Embedded Studio environment and is intended for use on the ADSP-21489 EzBoard platform. This project implements working with multi-focused images, which are merged into a single image using the EMD algorithm, where all pixels are in focus. <br> 
Below is an explanation of the repository structure and the method of using the program.

## Repository Contents

- **C code** – Main implementation of signal processing algorithms.
- **Python scripts** – Auxiliary scripts for analysis and visualization of results.
- **Report** – Detailed project documentation, including a description of the problem, methods used, results, and analyses.
- **.gitignore** and **LICENSE** – Standard files for excluding unnecessary files and legal terms of use.

## Git Repository Structure

<pre>
EDM_Fusion_Project/ <br>
├── README.md                       # Project description and instructions
├── LICENSE                         # License (MIT)
├── .gitignore                      # Rules for ignoring files
├── Izvjestaj_Radislav_Kosijer.pdf  # Detailed project report
├── src/                            # Source code
│   ├── Images/                     # Directory containing multi-focused images
│   ├── main.c                      # Main program
│   ├── main.h                      # Header
│   ├── emd.h                       # Definition of EMD and auxiliary functions 
│   ├── emd.c                       # Implementation of EMD and auxiliary functions 
│   ├── decision_mask.h             # Definition of functions related to mask determination
│   ├── decision_mask.c             # Implementation of functions related to mask determination
│   ├── fusion.h                    # Definition of functions for fusion and image saving
│   ├── fusion.c                    # Implementation of functions for fusion and image saving
│   ├── led.h                       # Definition of functions for LED logic
│   ├── led.c                       # Implementation of functions for LED logic
│   └── generate_header.py          # Script for generating C header from an image
└── Debug/                          # Directory containing debug information
│   ├── generate_bmp_image.py       # Script for generating a .bmp image
│   └── generate_jpg_image.py       # Script for generating a .jpg image
└── system/startup_ldf              # Directory containing debug information
    └── app.ldf                     # .ldf file containing information about memory segments
</pre>

## How to Compile?

The project is developed in the C programming language, with the help of Python scripts. The first step in using the program is to generate a C header file using the _generate_header.py_ script in the following way:

```bash
python3 generate_header.py
```
After generating, it is necessary to include the generated header in _main.c_ and add the required information found in the header. After running and compiling, the C program will create a _fused_img.bin_ file. Once this binary file is created, it is necessary to run the Python script in the same directory where the file is located (in this case, the Debug directory) in the following way

```bash
python3 generate_bmp_image.py
```
or
```bash
python3 generate_jpg_image.py
```

Depending on the image format desired by the user. <br>
After running the script, the desired image is obtained, where all pixels are in focus.
