/*
*	hpgl.c
*	8.2.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "printer.h"
#include "graph.h"

#define MAX_CMD_LEN 50

/* pen up = 0, down = 1 */
static int pen = 0;
static double scale = 1;

static void process_cmd(PRINTER *p, char *cmd);
static void hpgl_pa(PRINTER *p, char *cmd);
static void hpgl_pr(PRINTER *p, char *cmd);
static void hpgl_ci(PRINTER *p, char *cmd);
static int getParamAsInt(char *cmd, int which);


/* Draw hpgl file */
void hpgl_draw_from_file(PRINTER *p, char *file_name) {

	FILE *fr;
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("Error: Cannot open file %s for reading\n", file_name);
		return;
	}

	pr_init(p);

	char c;
	char cmd[MAX_CMD_LEN + 1];
	*cmd = '\0';
	int len = 0;
	while ((c = getc(fr)) != EOF) {

		if ((c == ';') || (strlen(cmd) == MAX_CMD_LEN)) {
			process_cmd(p, cmd);
			*cmd = '\0';
			int len = 0;
		} else {
			strncat(cmd, &c, 1);
			len++;
		}
	}

	fclose(fr);

	xy_hm(p);
}

static void process_cmd(PRINTER *p, char *cmd) {
	if (!strncmp(cmd, "PA", 2)) {
		hpgl_pa(p, cmd);
		return;
	}
	if (!strncmp(cmd, "PR", 2)) {
		hpgl_pr(p, cmd);
		return;
	}
	if (!strncmp(cmd, "PU", 2)) {
		pen = 0;
		return;
	}
	if (!strncmp(cmd, "PD", 2)) {
		pen = 1;
		return;
	}
	if (!strncmp(cmd, "EA", 2)) {
		fprintf(stderr, "Warning: Edge Rectangle Absolute is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "ER", 2)) {
		fprintf(stderr, "Warning: Edge Rectangle Relative is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "RA", 2)) {
		fprintf(stderr, "Warning: Shade Rectangle Absolute is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "RR", 2)) {
		fprintf(stderr, "Warning: Shade Rectangle Relative is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "AA", 2)) {
		fprintf(stderr, "Warning: Arc Absolute is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "AR", 2)) {
		fprintf(stderr, "Warning: Arc Relative is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "CI", 2)) {
		hpgl_ci(p, cmd);
		return;
	}
	if (!strncmp(cmd, "EW", 2)) {
		fprintf(stderr, "Warning: Edge Wedge is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "WG", 2)) {
		fprintf(stderr, "Warning: Shade Wedge is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "FT", 2)) {
		fprintf(stderr, "Warning: Fill Type is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "LT", 2)) {
		fprintf(stderr, "Warning: Line Type is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "PM", 2)) {
		fprintf(stderr, "Warning: Polygon Mode is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "PT", 2)) {
		fprintf(stderr, "Warning: Pen Thickness is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "SP", 2)) {
		fprintf(stderr, "Warning: Select Pen is not implemented\n");
		return;
	}
	if (!strncmp(cmd, "VS", 2)) {
		fprintf(stderr, "Warning: Velocity Select is not implemented\n");
		return;
	}

	printf("Unknown command: %s\n", cmd);
}


/* Plot Absolute */
static void hpgl_pa(PRINTER *p, char *cmd) {
	int x = scale * (double) getParamAsInt(cmd, 0);
	int y = scale * (double) getParamAsInt(cmd, 1);
	if (pen) {
		xy_va(p, x, y);
	} else {
		xy_ma(p, x, y);
	}	
}

/* Plot Relative */
static void hpgl_pr(PRINTER *p, char *cmd) {
	int x = scale * (double) getParamAsInt(cmd, 0);
	int y = scale * (double) getParamAsInt(cmd, 1);
	if (pen) {
		xy_vr(p, x, y);
	} else {
		xy_mr(p, x, y);
	}
}

/* Circle */
static void hpgl_ci(PRINTER *p, char *cmd) {
	int r = scale * (double) getParamAsInt(cmd, 0);
	xy_cr(p, r);
}

/* Extracts parameters from cmd */
static int getParamAsInt(char *cmd, int which) {
	char *p = cmd;
	if (*p != '\0' && *(p + 1) != '\0') {
		p += 2;
	}
	int i = 0;
	while (*p != '\0') {
		if (i == which) {
			return atoi(p);
		}
		if ((*p == ',') || (*p == ' ')) {
			i++;
		}
		p++;
	}

	return 0;
}
