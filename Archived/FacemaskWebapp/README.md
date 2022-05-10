### Facemask Webapp

This webapp allows a user to create design patterns and animations for a LED-matrix that is based on a linear addressable LED-strip.

Exported binary file format:
```
uint8: total animation steps
//step 1
    uin16: numbers of colors in this step,
    uint8: r,
    uint8: g,
    uint8: b,
    uint16: number of leds in this step
    uint16 led_indexes[] 
//step 2
    uint16: numbers of colors in this step,
    uint8: r,
    uint8: g,
    uint8: b,
    uint16: number of leds in this step
    uint16 led_indexes[] 
//step n
  ....
```
