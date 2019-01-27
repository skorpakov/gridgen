# Grid Generator
**Grid Generator** (gridgen) is a command line utility for generating grids aimed for measuring and correcting geometric distortions of printers and scanners.

## Overview

In some tasks it is necessary to obtain printouts, the size of which exactly match to the one specified in the source files. For example, printing drawings or patterns of parts. Unfortunately, printers are not very accurate devices that can introduce significant geometric distortion when printing. This may be due to the design of the mechanisms for moving the printhead and feeding the paper, especially for low-cost models. But even in professional printers, the feed rate varies with paper thickness. In addition, the mechanisms wear have a significant impact. One of the functions of the utility is to generate a test targets. You can print and measure them to estimate the geometric distortions that the printer introduces and correct them.

Another task that may also require high accuracy is scanning. For example, converting paper drawings into electronic format. Scanners also can introduce significant geometric distortion. The utility allows you to generate test targets in the form of transparent masks that match exactly to the targets for printing. In a graphics editor, such a mask can be superimposed on a scanned image of a printed target, and the geometric distortions introduced by the scanner can be estimated from the differences in the image and mask.
