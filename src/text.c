/*
*	text.c
*	26.1.2014
*	Jaroslav Beran (jaroslav.beran@gmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "printer.h"
#include "graph.h"
#include "text.h"

#define FONT_DEFINITION_FILE "font1.fnt"
#define FONT_CMD_SIZE 4

#define FONT_TABLE_SIZE 224

static char **font = NULL;
static int char_size = 10;		/* in mm */
static int char_h_dist = 2;		/* in mm */
static double text_angle = 0;


static void draw_char(PRINTER *p, unsigned char c);
static char * get_first_command(char *s, int p[], int size);
static int load_font(char *font_definition_file);
static void free_font(void);


/* Set font size */
void xy_set_font_size(int i) {
	char_size = i;
}


/* Set text angle */
void xy_set_text_angle(double angle) {
	text_angle = angle;
}


/* Write text */
void xy_write(PRINTER *p, char *text) {
	if (load_font(FONT_DEFINITION_FILE) != 0) {
		return;
	}

	char *p_t;
	for (p_t = text; *p_t != '\0'; p_t++) {
		draw_char(p, *p_t);
	}

	free_font();
}


/* draw char */
static void draw_char(PRINTER *p, unsigned char c) {
	if (c < 32) {
		printf("char(%d)\n", c);
		return;
	}

	char *s = font[c - 32];
	int cmd[FONT_CMD_SIZE];
	D_POSITION pos;
	
	while ((s = get_first_command(s, cmd, FONT_CMD_SIZE)) != NULL) {
		pos = _transform_position(cmd[1] * char_size, cmd[2] * char_size, text_angle);

		 switch (cmd[0]) {
			case 'm' :      xy_mr(p, pos.x, pos.y);
					break;
			case 'v' :      xy_vr(p, pos.x, pos.y);
					break;
			case 'p' :      xy_pr(p, pos.x, pos.y);
					break;
		 }
	}

	pos = _transform_position(char_h_dist * char_size, 0, text_angle);
	xy_mr(p, pos.x, pos.y);
}


static char * get_first_command(char *s, int cmd[], int size) {
	int i;
	for (i = 0; i < size ; i++) {
		cmd[i] = 0;
	}
	
	i = 0;
	while (1) {

		if (i == 0) {
			cmd[i] = *s;
			i++;
			if (*s == '\0') {
				return NULL;
			}
		}

		if (*s == '\0') {
			return s;
		}

		if (*s == ';') {
			return ++s;
		}
		
		if ((*s == ',') && (i < size)) {
			cmd[i] = atoi(s + 1);
			i++;
		}

		s++;
	}
}


/* Load font */
static int load_font(char *font_definition_file) {
	if (font != NULL) free_font();

	if ((font = (char **) malloc(FONT_TABLE_SIZE * sizeof(char *))) == NULL) {
		printf("Error: Cannot allocate memory for font (1)\n");
		return -1;
	}

	FILE *fr;

	if ((fr = fopen(font_definition_file, "r")) == NULL) {
		printf("Error: Cannot open file %s for reading\n", font_definition_file);
		return -1;
	}

	char line[2048];
	int i = 0;
	while (fgets(line, sizeof(line), fr) != NULL) {

		if ((strlen(line) > 0) && strncmp(line, "#", 1) && strncmp(line, "\n", 1)) {

			if (i == FONT_TABLE_SIZE) {
				printf("Error: Font definition file is longer than %d\n", FONT_TABLE_SIZE);
				fclose(fr);
				return -1;
			}

			if ((font[i] = (char *) malloc((strlen(line) + 1) * sizeof(char))) == NULL) {
				printf("Error: Cannot allocate memory for font (2)\n");
				fclose(fr);
				return -1;
			}

			strcpy(font[i], line);
			i++;
		}
	}

	fclose(fr);

	if (i != FONT_TABLE_SIZE) {
		printf("Error: Font definition file is shorter than %d\n", FONT_TABLE_SIZE);
		return -1;
	}

	return 0;
}

/* Free font */
static void free_font(void) {

	if (font == NULL) return;

	int i;
	for (i = 0; i < FONT_TABLE_SIZE ; i++) {
		if (font[i] != NULL) {
			free((void *) font[i]);
		}
	}
	free((void *) font);
	font = NULL;
}
