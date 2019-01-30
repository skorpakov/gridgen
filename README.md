# Grid Generator
**Grid Generator** (gridgen) is a command line utility for generating grids aimed for measuring and correcting geometric distortions of printers and scanners.

## Table of Contents

* [Overview](#overview)
* [Usage](#usage)
* [Printer adjustments](#printers-adjustments)
* [Scanner adjustments](#scanner-adjustments)

## Overview

In some tasks it is necessary to obtain printouts, the size of which exactly match to the one specified in the source files. For example, printing drawings or patterns of parts. Unfortunately, printers are not very accurate devices that can introduce significant geometric distortion when printing. This may be due to the design of the mechanisms for moving the printhead and feeding the paper, especially for low-cost models. But even in professional printers, the feed rate varies with paper thickness. In addition, the mechanisms wear have a significant impact. One of the functions of the utility is to generate a test targets. You can print and measure them to estimate the geometric distortions that the printer introduces and correct them.

Another task that may also require high accuracy is scanning. For example, converting paper drawings into electronic format. Scanners also can introduce significant geometric distortion. The utility allows you to generate test targets in the form of transparent masks that match exactly to the targets for printing. In a graphics editor, such a mask can be superimposed on a scanned image of a printed target, and the geometric distortions introduced by the scanner can be estimated from the differences in the image and mask.

## Usage

    gridgen [options] <filename>
        options:
          -M:target|mask|ruler    grid generation mode (default: target):
            target                 target grid for printing
            mask                   mask grid for scanned target ajustments
            ruler                  single ruler
          -F:<scale factor>       raster scale factor for target mode (default: 3)
          -B:<border width>       border width in mm (default: 5)
          -A0...A6                image size of standard page format (default: A4)
          -W:<width>              image width in mm
          -H:<height>             image height in mm
          -L:<length>             ruler length in mm (for ruler mode)
          -D:<density>            output image DPI (default: 600)
          -R                      enable middle rulers drawing (default: disabled)
          -U                      enable saving of unscaled image (default: disabled)
        <filename>                output base file name (without extension)

The utility uses the ImageMagic library to create images. You must install the library before running the utility. The library is also required to build the utility from the source files. ImageMagic library can be downloaded from the [developer site](https://www.imagemagick.org/script/download.php).

## Printer adjustments

To generate a test grid for printing, use the target mode. Select the paper size, either by specifying the standard size from A0 to A6, or by specifying the height and width of the sheet in millimeters. Also specify the physical resolution of the printer in DPI. For Epson printers, this is usually 720dpi, for the rest 600dpi. The same resolution should be specified in the printer settings when printing. The grid should fit completely into the print area, so you must specify the size of the borders in millimeters. Specify borders one or two mm larger than indicated in the printer specifications. By the way you will determine the actual size of the borders.

Example: grid for A4 page size with 6 mm borders and resolution of 720 dpi.

    gridgen -M:target -A4 -B:6 -D:720 gridname

This command will generate a file named *gridname_A4_720dpi_b6_f3.png*, which you can now print.

You must wait at least 10 minutes after printing to allow the ink to dry (in the case of inkjet printing) or to cool the sheet (in the case of laser printing), since paper can significantly change its size when wet or heated.

Measure the size of the printed grid using a good steel ruler and bright lighting. Try to look perpendicular to the paper to achieve high accuracy.

Next, there are two options. In professional printers, horizontal printing accuracy is usually high enough. And the vertical size can be adjusted for each type of paper in the driver settings (the Paper Feed Adjustment parameter). For several attempts, you can pick up the feed value and obtain high geometric accuracy of printing.

Inexpensive models usually do not have this setting, and you will have to adjust the size of the images before printing. It is convenient to use any graphics editor which can record a sequence of commands in macros, and then apply them to the desired image (for example, Actions in Adobe Photoshop). You need to write a macro with the image scaling command with parameters that compensate for print distortion. You can calculate these parameters using your printed grid measurements.

## Scanner adjustments

After you have received an accurate printout of the test grid, you can proceed to the scanner adjustments.
