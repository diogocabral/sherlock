/*
 *  sig.c - written by Rob Pike, modified by Loki
 *
 *  This program doesn't quite do what the specification says.
 *  It adds a '-o outfile' option which allows the user to
 *  specify which file should receive the output (rather than
 *  always going to stdout). The default is still stdout.
 *  It also doesn't use stdin as input. This was done so that
 *  the usage message would appear if the user just types 'sig'.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum{
	WORDSIZE = 128
};

int		Ntoken = 3;
int		Zerobits = 4;
unsigned long	zeromask;
int		ntoken = 0;
char **		token;
FILE *		outfile;

void	signature(FILE*);

void usage(void)
{
	fprintf(stderr, "usage: sig");
	fprintf(stderr, " [-z zerobits]");
	fprintf(stderr, " [-n chainlength]");
	fprintf(stderr, " [-o outfile]");
	fprintf(stderr, " file ...\n");

	fprintf(stderr, "defaults:");
	fprintf(stderr, " zerobits=3");
	fprintf(stderr, " chainlength=4");
	fprintf(stderr, " outfile=the screen");
	fprintf(stderr, "\n");
	exit(2);
}

int main(int argc, char *argv[])
{
	FILE *f;
	int i, start, nfiles;
	char *s, *outname;

	outfile = stdout;
	outname = NULL;

	for (start=1; start < argc; start++) {
		if (argv[start][0] != '-')
			break;
		switch (argv[start][1]) {
		case 'z':
			s = argv[++start];
			if (s == NULL)
				usage();
			Zerobits = atoi(s);
			if (Zerobits < 0 || Zerobits > 31)
				usage();
			break;
		case 'n':
			s = argv[++start];
			if (s == NULL)
				usage();
			Ntoken = atoi(s);
			if (Ntoken <= 0)
				usage();
			break;
		case 'o':
			s = argv[++start];
			if (s == NULL)
				usage();
			outname = s;
			break;
		default:
			usage();
		}
	}

	nfiles = argc - start;
	if (nfiles < 1)
		usage();

	if (outname != NULL)
		outfile = fopen(outname, "w");

	zeromask = (1<<Zerobits)-1;

	for (i=start; i < argc; i++) {
		f = fopen(argv[i], "r");
		if (f == NULL) {
			fprintf(stderr, "sig: can't open %s:", argv[i]);
			perror(NULL);
			continue;
		}
		signature(f);
		fclose(f);
	}

	return 0;
}

unsigned long hash(char *tok[])
{
	unsigned long h;
	unsigned char *s;
	int i;

	h = 0;
	for (i=0; i < Ntoken; i++)
		for (s=(unsigned char*)tok[i]; *s; s++)
			h = h*31 + *s;
	return h;
}

void dotoken(char *s)
{
	unsigned long h;
	int i;

	for (i=Ntoken; --i > 0; )
		strcpy(token[i], token[i-1]);
	strcpy(token[0], s);
	ntoken++;
	if (ntoken < Ntoken)
		return;
	h = hash(token);
	if ((h & zeromask) == 0)
		fprintf(outfile, "%0lx\n", h>>Zerobits);
}

void tokenize(char *s)
{
	char *t, *e;

	e = s+strlen(s);
	while (s < e) {
		while (*s=='\t' || *s=='\n' || *s==' ')
			s++;
		t = s;
		while (*s!= '\0' && *s!='\t' && *s!='\n' && *s!=' ')
			s++;
		*s = '\0';
		if (s > t)
			dotoken(t);
		s++;
	}
}

void signature(FILE *f)
{
	char buf[1024];
	int i;

	token = malloc(Ntoken * sizeof(char*));
	for(i=0; i < Ntoken; i++)
		token[i] = malloc(WORDSIZE+1);

	while (fgets(buf, sizeof buf, f) != NULL)
		tokenize(buf);
}
