/*
* Copyright (C) 2017 - Charlton Head
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <zstd.h>

void *
malloc_or_exit(size_t size)
{
	void *buf = malloc(size);

	if (!buf) {
		fputs("Malloc failed!\n", stderr);
		exit(2);
	}

	return buf;
}

void
compress(int clevel)
{
	size_t read;
	size_t bufinsize      = ZSTD_CStreamInSize();
	size_t bufoutsize     = ZSTD_CStreamOutSize();
	size_t toread         = bufinsize;
	void *bufin           = malloc_or_exit(bufinsize);
	void *bufout          = malloc_or_exit(bufoutsize);
	ZSTD_CStream *cstream = ZSTD_createCStream();
	size_t result         = ZSTD_initCStream(cstream, clevel);

	if (ZSTD_isError(result)) {
		fprintf(stderr, "ZSTD_initCStream() failed: %s\n",
		        ZSTD_getErrorName(result));
		exit(3);
	}

	while ((read = fread(bufin, 1, toread, stdin)) > 0) {
		ZSTD_inBuffer input = {bufin, read, 0};
		while (input.pos < input.size) {
			ZSTD_outBuffer output = {bufout, bufoutsize, 0};
			toread = ZSTD_compressStream(cstream, &output, &input);
			fwrite(bufout, 1, output.pos, stdout);
		}
	}

	ZSTD_outBuffer output   = {bufout, bufoutsize, 0};
	
	if (ZSTD_endStream(cstream, &output) > 0) {
		fputs("Cannot fully flush cstream!", stderr);
		exit(4);
	}

	fwrite(bufout, 1, output.pos, stdout);

	ZSTD_freeCStream(cstream);
	free(bufin);
	free(bufout);
}

int
main(int argc, char *argv[])
{
	int clevel = 1;

	if (argc>1 && argv[1][0] == '-')
		sscanf(argv[1], "-%d", &clevel);

	if (clevel > 19 || clevel < 1) {
		fputs("Compression level must be 1-19.\n", stderr);
		return 1;
	}

	compress(clevel);

	return 0;
}