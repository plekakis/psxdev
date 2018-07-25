/*
 *      MDEC Encoder
 *
 *      (C) 1994, 1995, 1996, 1997 Sony Computer Entertainment Inc.
 *      All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdec.h"

const char * const VERSION = "3.2";
const char * const CopyRight =
	"(C) 1994, 1995, 1996, 1997 Sony Computer Entertainment Inc."
	"All Rights Reserved.";

char *commandname = NULL;	/* command name */

/*
 * global variables
 */
char	*ifile  = "";		/* input file name */
char 	*ofile  = "";		/* output file name */
long	quant    = 1;		/* initial quant value */
u_long	bsSizeLimit = 0;	/* bs date size limit */
long	width = 16;			/* width of image */
long	height = 16;		/* height of image */
double  fullness = 0.9;		/* target fullness of bs data */
Format	format = RGBformat;	/* format of input image */

/*
 * static functions
 */
static void getoption(int argc, char **argv);
static void usage();

#ifdef MAC
int main()
{
	int argc = 10;
	static char *argv[] = {	/* sample data */
		"menc32",
		"-rgb",
		"-w",
		"320",
		"-h",
		"240",
		"-i",
		"p320a.rgb",
		"-o",
		"p320a.bs"

	};
	printf("start ... ");
	fflush(stdout);
#else
int main(int argc, char *argv[])
{
#endif

	commandname = argv[0];

	/*
	 *	process command line options
	 */
	getoption(argc, argv);
	

	/*
	 * open input/output files
	 */
	FILE *input = stdin;
	FILE *output = stdout;
	if (*ifile) {
		input = fopen(ifile, "rb");
		if (!input) {
			fprintf(stderr, "Cannot open %s.\n", ifile);
			exit(-1);
		}
	}
	if (*ofile) {
		output = fopen(ofile, "wb");
		if (!output) {
			fprintf(stderr, "Cannot open %s.\n", ofile);
			exit(-1);
		}
	}

	/*
	 *	check the format of input data and allocate buffers
	 */
	u_int pixSize = 0;
	switch (format) {
		case RGBformat:
			pixSize = width * height * 3;
			break;

		case YUVformat:
			pixSize = width * height * 2;
			break;

		default:
			fprintf(stderr, "Unknown format\n");
			exit(-1);
	}
	u_char *pix = new u_char[pixSize];
	u_long bsBufSize = pixSize / sizeof(u_long);
	u_long *bs = new u_long[bsBufSize];
	if (!pix || !bs) {
		fprintf(stderr, "Can't allocate memory\n");
		exit(-1);
	}


	/*
	 * read input date
	 */
	if (fread(pix, sizeof(*pix), pixSize, input) != pixSize) {
		fprintf(stderr, "Cannot read data from %s\n",
								(*ifile)? ifile: "stdin");
		exit(-1);
	}
	if (input && *ifile) {
		fclose(input);
	}

	/*
	 * initialization of MDEC library
	 */
	MdecBegin();
	
	/*
	 * encode one frame
	 */
	u_long bsSize;
	u_short finalQuant;
	if (bsSizeLimit) {
		bsSize = MdecEncode(
			pix,
			format,
			(u_short)width,
			(u_short)height,
			bs,
			bsBufSize,
			bsSizeLimit,
			fullness,
			(u_short)quant,
			&finalQuant
		);
	} else {
		bsSize = MdecEncode(
			pix,
			format,
			(u_short)width,
			(u_short)height,
			bs,
			bsBufSize,
			(u_short)quant
		);
	}


	/*
	 * write output data
	 */
	if (fwrite(bs, 1, (u_int)bsSize, output) != bsSize) {
		fprintf(stderr, "Cannot write data to %s\n",
			(*ofile)? ofile: "stdout");
		exit(-1);
	}
	if (output && *ofile) {
		fclose(output);
	}

	/*
	 * release memory
	 */
	delete pix;
	delete bs;

	/*
	 * terminate MDEC library
	 */
	MdecEnd();

#ifdef MAC
	printf("normal exit\n");
#endif

	return (int)finalQuant;
}

/*
 * process comand line options
 */
static void getoption(int argc, char **argv)
{
	int		fileSpecified = 0;
	char **argvLimit = argv + argc;
	
	for (argv++; argv < argvLimit; argv++) {
		if (strcmp(*argv, "-help") == 0)
			usage();
		else if (strcmp(*argv, "-i") == 0)	ifile  = *++argv;
		else if (strcmp(*argv, "-o") == 0)	ofile  = *++argv;
		else if (strcmp(*argv, "-w") == 0)	width  = atol(*++argv);
		else if (strcmp(*argv, "-h")  == 0)	height  = atol(*++argv);
		else if (strcmp(*argv, "-rgb") == 0)	{
			format = RGBformat;
		} else if (strcmp(*argv, "-yuv") == 0) {
			format  = YUVformat;
		} else if (strcmp(*argv, "-fullness") == 0) fullness  = (double)atof( *++argv);
		else if (strcmp(*argv, "-quant") == 0) quant  = atol(*++argv);
		else if (strcmp(*argv, "-size") == 0) bsSizeLimit = atol(*++argv);
		else if (**argv == '-' || fileSpecified) {
			usage();
			exit(-1);
		} else {
			ifile = *argv;
			fileSpecified = 1;
		}
	}
}

/*
 * usage
 */
static void usage()
{
	fprintf(stderr, "%s\n", CopyRight);
	fprintf(stderr, "usage: %s [options] [input file]\t(version %s)\n", commandname, VERSION);
	fprintf(stderr, "\t-i file:    input file\n");
	fprintf(stderr, "\t-o file:    encoded bs file\n");
	fprintf(stderr, "\t-quant n:   quantization param          default: 1\n");
	fprintf(stderr, "\t-size size: limit size of bs file (bytes)\n");
	fprintf(stderr, "\t-w width:   width of input image\n");
	fprintf(stderr, "\t-h height:  height of input image\n");
	fprintf(stderr, "\t-rgb:       input image -> rgb file(default)\n");
	fprintf(stderr, "\t-yuv:       input image -> yuv file\n");
	fprintf(stderr, "\t-fullness:     fullness of bs data: 0.0-1.0 (default: 0.9)\n");
	fprintf(stderr, "\t-help\n");

	exit(-1);
}

