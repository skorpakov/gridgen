// gridgen.cpp
//

#include "stdafx.h"

#include <Magick++.h>
#include <string>
#include <iostream>

#include <math.h>

using namespace std;
using namespace Magick;

// Grid generation modes
#define GG_MODE_TARGET	1	// target grid for printing
#define GG_MODE_MASK	2	// mask grid for scanned target ajustments
#define GG_MODE_RULER	3	// single ruler

// Macros to convert millimeters to pixels
#define mmToPx(mm) ((mm)/25.4*DPI)
#define mmToPxF(mm) ((unsigned int)(floor(((mm)*DPI)/25.4)))
#define mmToPxR(mm) ((unsigned int)(round(((mm)*DPI)/25.4)))

int main(int argc, char ** argv)
{
	// Standart page sizes in mm
	unsigned int pageASizes[7][2] = { { 841, 1189 }, { 594, 841 }, { 420, 594 }, { 297, 420 }, { 210, 297 }, { 148, 210 }, { 105, 148 } };

	// Defaults
	unsigned int mode = GG_MODE_TARGET;
	unsigned int rasterFactor = (mode == GG_MODE_MASK ? 1 : 3);
	unsigned long bordermm = 5;
	unsigned int pageASize = 4;
	unsigned long sizeXmm = 0;
	unsigned long sizeYmm = 0;
	unsigned int outputDPI = 600;
	bool middleRuler = false;
	unsigned int markHeight1mm = 3;
	unsigned int markHeight5mm = 5;
	unsigned int markHeight0_5mm = 2;
	bool saveUnscaled = false;
	string fileName = "grid";

	// Read command line parameters
	int currentArg = 1;
	while ((currentArg < argc) && (argv[currentArg][0] == '-'))
	{
		string strArg = argv[currentArg];
		if (strArg.substr(1, 2) == "M:") {
			if (strArg.substr(3) == "target") mode = GG_MODE_TARGET;
			else if (strArg.substr(3) == "mask") mode = GG_MODE_MASK;
			else if (strArg.substr(3) == "ruler") mode = GG_MODE_RULER;
		}
		if (strArg.substr(1, 2) == "F:") rasterFactor = std::stoi(strArg.substr(3));
		if (strArg.substr(1, 2) == "B:") bordermm = std::stoi(strArg.substr(3));
		if (strArg.substr(1, 1) == "A") pageASize = std::stoi(strArg.substr(2));
		if (strArg.substr(1, 2) == "W:") sizeXmm = std::stoi(strArg.substr(3));
		if (strArg.substr(1, 2) == "L:") sizeXmm = std::stoi(strArg.substr(3));
		if (strArg.substr(1, 2) == "H:") sizeYmm = std::stoi(strArg.substr(3));
		if (strArg.substr(1, 2) == "D:") outputDPI = std::stoi(strArg.substr(3));
		if (strArg.substr(1) == "R") middleRuler = true;
		if (strArg.substr(1) == "U") saveUnscaled = true;
		currentArg++;
	}

	if ((argc < 2) || (currentArg > (argc - 1)))
	{
		cout << "Usage: gridgen [options] <filename>" << endl;
		cout << "\toptions:" << endl;
		cout << "\t  -M:target|mask|ruler    grid generation mode (default: target):" << endl;
		cout << "\t    target                 target grid for printing" << endl;
		cout << "\t    mask                   mask grid for scanned target ajustments" << endl;
		cout << "\t    ruler                  single ruler" << endl;
		cout << "\t  -F:<scale factor>       raster scale factor for target mode (default: 3)" << endl;
		cout << "\t  -B:<border width>       border width in mm (default: 5)" << endl;
		cout << "\t  -A0|-A1|...|-A6         image size of standard page format (default: A4)" << endl;
		cout << "\t  -W:<width>              image width in mm" << endl;
		cout << "\t  -H:<height>             image height in mm" << endl;
		cout << "\t  -L:<length>             ruler length in mm (for ruler mode)" << endl;
		cout << "\t  -D:<density>            output image DPI (default: 600)" << endl;
		cout << "\t  -R                      enable middle rulers drawing (default: disabled)" << endl;
		cout << "\t  -U                      enable saving of unscaled image (default: disabled)" << endl;
		cout << "\t<filename>                output base file name (without extension)" << endl;

		return 0;
	}

	// Build output file name based on parameters
	fileName = argv[currentArg];
	if (mode == GG_MODE_RULER) {
		if (sizeXmm == 0) sizeXmm = pageASizes[pageASize][0];
		fileName += "_" + to_string(sizeXmm);
	}
	else
		fileName += ((sizeXmm == 0) && (sizeYmm == 0) ? "_A" + to_string(pageASize) : "_" + to_string(sizeXmm) + "x" + to_string(sizeYmm));
	fileName += "_" + to_string(outputDPI) + "dpi";
	if (mode != GG_MODE_RULER)
		fileName += "_b" + to_string(bordermm);
	if((mode != GG_MODE_MASK) && (rasterFactor > 1))
		fileName += "_f" + to_string(rasterFactor);
	if ((middleRuler) && (mode != GG_MODE_RULER))
		fileName += "_mid";
	if (mode == GG_MODE_MASK)
		fileName += "_mask";
	if (saveUnscaled && (mode != GG_MODE_MASK) && (rasterFactor > 1))
		fileName += "_unscaled";

	// Use the width and height of a standard page only if they are not specified in parameters.
	if (sizeXmm == 0) sizeXmm = pageASizes[pageASize][0];
	if (sizeYmm == 0) sizeYmm = pageASizes[pageASize][1];

	// Calculate position of middel rulers
	unsigned long middleXmm = (unsigned long)((round((sizeXmm - bordermm * 2) / 20.0) * 10));
	unsigned long middleYmm = (unsigned long)((round((sizeYmm - bordermm * 2) / 20.0) * 10));

	// Calculate working DPI for target and ruler mode depending on raster scale factor
	unsigned int DPI = outputDPI * (mode == GG_MODE_MASK ? 1 : rasterFactor);

	char *color = mode == GG_MODE_MASK ? "red" : "black";
	char *bgColor = mode == GG_MODE_MASK ? "transparent" : "white";

	// Display working parameters
	cout << "Mode: " << (mode == GG_MODE_TARGET ? "target" : (mode == GG_MODE_MASK ? "mask" : "ruler")) << endl;
	if ((mode != GG_MODE_MASK) && (rasterFactor > 1))
		cout << "Raster factor: x" << rasterFactor << endl;
	if (mode == GG_MODE_RULER)
		cout << "Length: " << sizeXmm << "mm" << endl;
	else
		cout << "Size: " << sizeXmm << "x" << sizeYmm << "mm" << endl;
	if (mode != GG_MODE_RULER)
		cout << "Border: " << bordermm << "mm" << endl;
	cout << "Output DPI: " << outputDPI << endl;
	if ((middleRuler) && (mode != GG_MODE_RULER))
		cout << "Drawing middle rulers enabled" << endl;
	if(saveUnscaled)
		cout << "Saving unscaled image enabled" << endl;
	cout << "Output file name: " << fileName << endl;

	cout << endl << "Initializing" << endl;

	InitializeMagick(*argv);

	try {

		Image image(mode == GG_MODE_RULER ? Geometry(mmToPxF(sizeXmm - bordermm * 2), mmToPxF(10)) : Geometry(mmToPxF(sizeXmm) + 1, mmToPxF(sizeYmm) + 1), bgColor);

/*  Ruler **********************************************************************************/
		if (mode == GG_MODE_RULER) {
			cout << "Drawing ruler" << endl;

			image.strokeColor(color);
			image.strokeWidth(rasterFactor);
			image.strokeAntiAlias(false);
			image.fontPointsize(mmToPx(2.7));
			image.fillColor("black");

			// Draw marks with 0.2 mm step
			for (double i = 0; i < (sizeXmm/* - bordermm * 2*/); i += 0.2)
			{
				i = round(i * 10) / 10; // prevent error accumulation;
				unsigned int x = mmToPxF(i), yFrom = 0, yTo = mmToPxF((fmod(i, 10) == 0 ? 6 : (fmod(i, 5) == 0 ? 5 : (fmod(i, 1) == 0 ? 4 : 2))));
				image.draw(DrawableLine(x, yFrom, x, yTo));
			}

			// Draw border line around
			image.draw(DrawableLine(0, 0, 0, mmToPxF(10) - 1));
			image.draw(DrawableLine(0, 0, mmToPxF(sizeXmm/* - bordermm * 2*/), 0));
			image.draw(DrawableLine(0, mmToPxF(10) - 1, mmToPxF(sizeXmm/* - bordermm * 2*/), mmToPxF(10) - 1));

			// Draw numbers
			image.strokeWidth(1);

			for (double i = 10; i < (sizeXmm/* - bordermm * 2*/); i += 10)
				image.draw(DrawableText(mmToPxF(i - (i < 100 ? 0.8 : 1.6)), mmToPxF(9), std::to_string((unsigned int)(floor(i / 10)))));
		}

/*	Grid ************************************************************************************/
		else {

			if (mode != GG_MODE_MASK)
				image.type(GrayscaleType);

			image.strokeColor(color);
			image.strokeWidth(mode == GG_MODE_MASK ? 1 : rasterFactor);
			image.strokeAntiAlias(false);
			image.fillColor("transparent");

/*	Draw border **************************************************************************/
			cout << "Drawing border" << endl;

			for (unsigned int i = 0; i <= bordermm; i++)
			{
				unsigned int xFrom = mmToPxF(i), yFrom = xFrom, xTo = mmToPxF(sizeXmm - i), yTo = mmToPxF(sizeYmm - i);
				image.draw(DrawableLine(xFrom, yFrom, xTo, yFrom));
				image.draw(DrawableLine(xTo, yFrom, xTo, yTo));
				image.draw(DrawableLine(xTo, yTo, xFrom, yTo));
				image.draw(DrawableLine(xFrom, yTo, xFrom, yFrom));
			}

/*	Draw horizontal lines ****************************************************************/
			cout << "Drawing horizontal lines" << endl;

			for (unsigned int i = 0; i <= (sizeYmm - bordermm * 2); i++)
			{
				// Draw grid lines and left ruler
				unsigned int xFrom = mmToPxF(fmod(i, 10) == 0 ? 0 : bordermm), xTo = mmToPxF(fmod(i, 10) == 0 ? sizeXmm : bordermm + (fmod(i, 5) == 0 ? markHeight5mm : markHeight1mm)), y = mmToPxF(i + bordermm);
				image.draw(DrawableLine(xFrom, y, xTo, y));

				if (fmod(i, 10) != 0) {
					// Draw right ruler
					xFrom = mmToPxF(sizeXmm - bordermm); xTo = mmToPxF(sizeXmm - bordermm - (fmod(i, 5) == 0 ? markHeight5mm : markHeight1mm));
					image.draw(DrawableLine(xFrom, y, xTo, y));
					// Draw middle ruler
					if (middleRuler) {
						xFrom = mmToPxF(middleXmm + bordermm - (fmod(i, 5) == 0 ? 5 : markHeight1mm)); xTo = mmToPxF(middleXmm + bordermm + (fmod(i, 5) == 0 ? markHeight5mm : markHeight1mm));
						image.draw(DrawableLine(xFrom, y, xTo, y));
					}
				}

				// Draw middle ruler 0.5mm marks
				if ((middleRuler) && (i <= (sizeYmm - bordermm * 2 - 1))) {
					xFrom = mmToPxF(middleXmm + bordermm - markHeight0_5mm); xTo = mmToPxF(middleXmm + bordermm + markHeight0_5mm); y = mmToPxF(0.5 + i + bordermm);
					image.draw(DrawableLine(xFrom, y, xTo, y));
				}
			}

/*	Draw vertical lines ******************************************************************/
			cout << "Drawing vertical lines" << endl;

			for (unsigned int i = 0; i <= (sizeXmm - bordermm * 2); i++)
			{
				// Draw grid lines and top ruler
				unsigned int x = mmToPxF(i + bordermm), yFrom = mmToPxF(fmod(i, 10) == 0 ? 0 : bordermm), yTo = mmToPxF(fmod(i, 10) == 0 ? sizeYmm : bordermm + (fmod(i, 5) == 0 ? markHeight5mm : markHeight1mm));
				image.draw(DrawableLine(x, yFrom, x, yTo));

				if (fmod(i, 10) != 0) {
					// Draw bottom ruler
					yFrom = mmToPxF(sizeYmm - bordermm); yTo = mmToPxF(sizeYmm - bordermm - (fmod(i, 5) == 0 ? markHeight5mm : markHeight1mm));
					image.draw(DrawableLine(x, yFrom, x, yTo));
					// Draw middle ruler
					if (middleRuler) {
						yFrom = mmToPxF(middleYmm + bordermm - (fmod(i, 5) == 0 ? 5 : markHeight1mm)); yTo = mmToPxF(middleYmm + bordermm + (fmod(i, 5) == 0 ? markHeight5mm : markHeight1mm));
						image.draw(DrawableLine(x, yFrom, x, yTo));
					}
				}

				// Draw middle ruler 0.5mm marks
				if ((middleRuler) && (i <= (sizeXmm - bordermm * 2 - 1))) {
					x = mmToPxF(0.5 + i + bordermm); yFrom = mmToPxF(middleYmm + bordermm - markHeight0_5mm); yTo = mmToPxF(middleYmm + bordermm + markHeight0_5mm);
					image.draw(DrawableLine(x, yFrom, x, yTo));
				}
			}

/*	Draw diagonal lines ******************************************************************
			cout << "Drawing diagonal lines" << endl;

			for (unsigned int i = 0; i <= (sizeYmm + sizeXmm - bordermm * 2); i += 5)
			{
				unsigned int xFrom = mmToPxF(0), xTo = mmToPxF(sizeXmm), yFrom = mmToPxF(i), yTo = mmToPxF(i + sizeXmm);
				image.draw(DrawableLine(xFrom, yFrom, xTo, yTo));
			}

/*	Draw numbers *************************************************************************/
			cout << "Drawing numbers" << endl;

			image.fontPointsize(mmToPx(2));
			image.fillColor(color);
			image.strokeWidth(1);

			// Draw left ruler numbers
			for (unsigned int i = 10; i <= (sizeYmm - bordermm * 2); i += 10)
				image.draw(DrawableText(mmToPxF(bordermm + (i < 100 ? 8.7 : 7.6)), mmToPxF(bordermm + i - 0.3), std::to_string((unsigned int)(floor(i / 10)))));

			// Draw top ruler numbers
			for (unsigned int i = 20; i <= (sizeXmm - bordermm * 2); i += 10)
				image.draw(DrawableText(mmToPxF(bordermm + i - (i < 100 ? 1.3 : 2.4)), mmToPxF(bordermm + 9.7), std::to_string((unsigned int)(floor(i / 10)))));

		}
/*	Scale target to required DPI ************************************************************/
		if (!saveUnscaled && (mode != GG_MODE_MASK) && (rasterFactor > 1)) {
			cout << "Scaling target to required DPI" << endl;
			DPI = outputDPI;
//			image.resize(Geometry(mmToPxF(sizeXmm) + 1, mmToPxF(sizeYmm) + 1));
			image.resize(Geometry((unsigned int)round(image.baseColumns() / rasterFactor), (unsigned int)round(image.baseRows() / rasterFactor)));
		}

/*	Save to PNG **************************************************************************/
		cout << "Saving to PNG" << endl;

		image.depth(8);
		image.compressType(LZWCompression/*RLECompression*/);
		image.resolutionUnits(PixelsPerInchResolution);
		image.density(Point(DPI, DPI));
		image.write(fileName + ".png");

		cout << "Done" << endl;
	}
	catch (exception &error_)
	{
		cout << "Caught exception: " << error_.what() << endl;
		return 1;
	}

	return 0;
}

