# DIY CNC Plotter

A simple CNC drawing machine built using Arduino UNO, 28BYJ stepper motors, and ULN2003 drivers.  
It interprets basic G-code commands (`G0`, `G1`, `M300`) to draw vector art with ~2–3mm accuracy.

### Components Used
- Arduino UNO
- 3 × 28BYJ-48 stepper motors
- 3 × ULN2003 driver boards
- Servo motor for pen lift
- 3D printed frame

### Workflow
1. Convert black & white PNG ➝ SVG using Inkscape  
2. Convert SVG ➝ G-code using [svg2gcode](https://svg2gcode.com)  
3. Send G-code to Arduino over serial  

### Limitations
- No support for `G2`, `G3`, or absolute positioning  
- Accuracy is limited due to motor type and mechanical design

> Developed by Chirag Gujrathi | 1st Year B.Tech (Robotics & AI), COEP Technological University Pune

