#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<assert.h>
#include "utilities.h"
#include "ants.h"
#include "ls.h"
#include "InOut.h"

static char Delimiters[] = "= \n\t\r\f\v\xef\xbb\xbf";
static char *GetFileName(char *Line);
static char *ReadLine(FILE * InputFile);
static int fscanint(FILE * f, int *v);
static void check_out_of_range(double value, double minval, double maxval, const char *optionName);
static void check_paramaters(void);
static char *LastLine;
static char *Buffer;
static int MaxBuffer;

typedef struct Options Options;
struct Options {
	char *Problem_File;
	char *Tour_File;
	int Max_Tries;
	int Max_Tours;
	int Max_Time;
	int Seed;
	int Optimum;
	int Ants;
	int NN_Ants;
	double alpha;
	double beta;
	double rho;
	double Q0;
	int Elitist_Ants;
	int RAS_Ranks;
	int NN_Local_Search;
	int Local_Search;
	int Dont_Look_Bits;
	char Solver[5];
};

Options options;

void read_parameters_aco(char *fname) {
	char *Line, *Keyword, *Token;
	FILE *para_file;
	int i, j;

	para_file = fopen(fname, "r");
	if (para_file == NULL) {
		fprintf(stderr, "无法打开文件%s\n", fname);
		exit(1);
	}
	assert(para_file != NULL);

	while ((Line = ReadLine(para_file))) {
		if (!(Keyword = strtok(Line, Delimiters)))
			continue;
		if (Keyword[0] == '#')
			continue;
		for (i = 0; i < strlen(Keyword); i++)
			Keyword[i] = (char)toupper(Keyword[i]);
		if (strcmp(Keyword , "PROBLEM_FILE") == 0) {
			if (!(options.Problem_File = GetFileName(0)))
				fprintf(stderr, "PROBLEM_FILE：应该输入字符串");
		}
		else if (strcmp(Keyword, "TOUR_FILE") == 0) {
			if (!(options.Tour_File = GetFileName(0)))
				fprintf(stderr, "TOUR_FILE：应该输入字符串");
		}
		else if (strcmp(Keyword, "MAX_TRIES") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Max_Tries))
				fprintf(stderr, "MAX_TRIES: 应该输入整数");
		}
		else if (strcmp(Keyword, "MAX_TOURS") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Max_Tours))
				fprintf(stderr, "MAX_TOURS: 应该输入整数");
		}
		else if (strcmp(Keyword, "MAX_TIME") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Max_Time))
				fprintf(stderr, "MAX_TIME: 应该输入整数");
		}
		else if (strcmp(Keyword, "SEED") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Seed))
				fprintf(stderr, "SEED: 应该输入整数");
		}
		else if (strcmp(Keyword, "OPTIMUM") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Optimum))
				fprintf(stderr, "OPTIMUM: 应该输入整数");
		}
		else if (strcmp(Keyword, "ANTS") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Ants))
				fprintf(stderr, "ANTS: 应该输入DOUBLE");
		}
		else if (strcmp(Keyword, "NN_ANTS") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.NN_Ants))
				fprintf(stderr, "NN_ANTS: 应该输入DOUBLE");
		}
		else if (strcmp(Keyword, "ALPHA") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%lf", &options.alpha))
				fprintf(stderr, "ALPHA: 应该输入DOUBLE");
		}
		else if (strcmp(Keyword, "BETA") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%lf", &options.beta))
				fprintf(stderr, "BETA: 应该输入DOUBLE");
		}
		else if (strcmp(Keyword, "RHO") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%lf", &options.rho))
				fprintf(stderr, "RHO: 应该输入DOUBLE");
		}
		else if (strcmp(Keyword, "Q0") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%lf", &options.Q0))
				fprintf(stderr, "Q0: 应该输入DOUBLE");
		}
		else if (strcmp(Keyword, "ELITIST_ANTS") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Elitist_Ants))
				fprintf(stderr, "ELITIST_ANTS: 应该输入整数");
		}
		else if (strcmp(Keyword, "RAS_RANKS") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.RAS_Ranks))
				fprintf(stderr, "RAS_RANKS: 应该输入整数");
		}
		else if (strcmp(Keyword, "NN_LOCAL_SEARCH") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.NN_Local_Search))
				fprintf(stderr, "NN_LOCAL_SEARCH: 应该输入整数");
		}
		else if (strcmp(Keyword, "LOCAL_SEARCH") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Local_Search))
				fprintf(stderr, "LOCAL_SEARCH: 应该输入整数");
		}
		else if (strcmp(Keyword, "DONT_LOOK_BITS") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%d", &options.Dont_Look_Bits))
				fprintf(stderr, "DONT_LOOK_BITS: 应该输入整数");
		}
		else if (strcmp(Keyword, "SOLVER") == 0) {
			if (!(Token = strtok(0, Delimiters)) || !sscanf(Token, "%s", &(options.Solver)))
				fprintf(stderr, "SOLVER: 应该输入字符串");
		}
		else if (strcmp(Keyword, "EOF") == 0) 
			break;
	}

	if (!options.Problem_File) {
		fscanf(stderr, "Problem File 是必须参数，缺失！");
		exit(1);
	}
	fclose(para_file);
	if(LastLine!= NULL) free(LastLine);

	check_paramaters();
}

static void check_paramaters(void) {
	if (options.Problem_File != NULL)
	{
		if (strlen(options.Problem_File) >= LINE_BUF_LEN) {
			fprintf(stderr, "error: too long input filename '%s', maximum length is %d", options.Problem_File, LINE_BUF_LEN);
			exit(1);
		}
		strcpy(name_buf, options.Problem_File);
	}

	if (options.Tour_File != NULL) {
		if (strlen(options.Tour_File) >= LINE_BUF_LEN) {
			fprintf(stderr, "error: too long input filename '%s', maximum length is %d", options.Tour_File, LINE_BUF_LEN);
			exit(1);
		}
		strcpy(tour_buf, options.Tour_File);
	}
	if (options.Max_Tries > 0) {
		max_tries = options.Max_Tries;
		check_out_of_range(max_tries, 1, MAXIMUM_NO_TRIES, "max_tries (tries)");
	}

	if (options.Max_Time > 0)
	{
		max_time = options.Max_Time;
		check_out_of_range(max_time, 0.0, 86400., "max_time (seconds)");
	}
	if (options.Max_Tours > 0) {
		max_tours = options.Max_Tours;
		check_out_of_range(max_tours, 1, LONG_MAX, "max_tries (tries)");
	}
	if (options.Seed) {
		seed = options.Seed;
	}
	if (options.Optimum) {
		optimal = options.Optimum;
	}

	as_flag = eas_flag = ras_flag = mmas_flag = bwas_flag = acs_flag = FALSE;

	if (strcmp(options.Solver, "AS") == 0) {
		as_flag = TRUE;
		set_default_as_parameters();
	}
	if (strcmp(options.Solver, "EAS") == 0) {
		eas_flag = TRUE;
		set_default_eas_parameters();
	}
	if (strcmp(options.Solver, "RAS") == 0) {
		ras_flag = TRUE;
		set_default_ras_parameters();
	}
	if (strcmp(options.Solver, "MMAS") == 0) {
		mmas_flag = TRUE;
		set_default_mmas_parameters();
	}
	if (strcmp(options.Solver, "BWAS") == 0) {
		bwas_flag = TRUE;
		set_default_bwas_parameters();
	}
	if (strcmp(options.Solver, "ACS") == 0) {
		acs_flag = TRUE;
		set_default_acs_parameters();
	}
	if (options.Local_Search >= 0) {
		ls_flag = options.Local_Search;
		check_out_of_range(ls_flag, 0, 3, "ls_flag");
	}
	if (ls_flag) {
		set_default_ls_parameters();
	}
	if (options.Ants != 0) {
		n_ants = options.Ants;
		check_out_of_range(n_ants, 1, MAX_ANTS - 1, "n_ants");
	}
	if (options.NN_Ants != 0) {
		nn_ants = options.NN_Ants;
		check_out_of_range(nn_ants, 1, 100, "nn_ants");
	}
	if (options.alpha) {
		alpha = options.alpha;
		check_out_of_range(alpha, 0., 100., "alpha");
	}
	if (options.beta) {
		beta = options.beta;
		check_out_of_range(beta, 0., 100., "beta");
	}
	if (options.rho) {
		rho = options.rho;
		check_out_of_range(rho, 0.000001, 1., "rho");
	}
	if (options.Q0 >= 0) {
		q_0 = options.Q0;
		check_out_of_range(q_0, 0., 1., "q0");
	}
	if (options.Elitist_Ants) {
		elitist_ants = options.Elitist_Ants;
		check_out_of_range(elitist_ants, 0, LONG_MAX, "elitistants");
	}
	if (options.RAS_Ranks) {
		ras_ranks = options.RAS_Ranks;
		check_out_of_range(ras_ranks, 0, LONG_MAX, "rasranks");
	}
	if (options.NN_Local_Search) {
		nn_ls = options.NN_Local_Search;
		check_out_of_range(nn_ls, 0, LONG_MAX, "nnls");
	}
	if (options.Dont_Look_Bits) {
		dlb_flag = options.Dont_Look_Bits;
		check_out_of_range(dlb_flag, 0, 1, "dlb_flag");
	}
}

static char *GetFileName(char *Line)
{
	char *Rest = strtok(Line, "\n\t\r\f"), *t;

	if (!Rest)
		return 0;
	while (isspace(*Rest))
		Rest++;
	if (!Line) {
		if (*Rest == '=')
			Rest++;
	}
	while (isspace(*Rest))
		Rest++;
	for (t = Rest + strlen(Rest) - 1; isspace(*t); t--)
		*t = '\0';
	if (!strlen(Rest))
		return 0;
	assert(t = (char *)malloc(strlen(Rest) + 1));
	strcpy(t, Rest);
	return t;
}

static int EndOfLine(FILE * InputFile, int c)
{
	int EOL = (c == '\r' || c == '\n');
	if (c == '\r') {
		c = fgetc(InputFile);
		if (c != '\n' && c != EOF)
			ungetc(c, InputFile);
	}
	return EOL;
}

static char *ReadLine(FILE * InputFile)
{
	int i, c;

	if (Buffer == 0)
		assert(Buffer = (char *)malloc(MaxBuffer = 80));
	for (i = 0; (c = fgetc(InputFile)) != EOF && !EndOfLine(InputFile, c); i++) {
		if (i >= MaxBuffer - 1) {
			MaxBuffer *= 2;
			assert(Buffer = (char *)realloc(Buffer, MaxBuffer));
		}
		Buffer[i] = (char)c;
	}
	Buffer[i] = '\0';
	if (!LastLine || (int)strlen(LastLine) < i) {
		free(LastLine);
		assert(LastLine = (char *)malloc((i + 1) * sizeof(char)));
	}
	strcpy(LastLine, Buffer);
	return c == EOF && i == 0 ? 0 : Buffer;
}

static int fscanint(FILE * f, int *v)
{
	int val;
	int c, sign = 1;

	while (isspace(c = getc(f)));
	if (c == '-' || c == '+') {
		if (c == '-')
			sign = -1;
		if ((c = getc(f)) == EOF) {
			ungetc(c, f);
			return 0;
		}
	}
	if (!isdigit(c)) {
		ungetc(c, f);
		return 0;
	}
	val = c - '0';
	while (isdigit(c = getc(f)))
		val = 10 * val + (c - '0');
	*v = sign * val;
	return 1;
}

static void check_out_of_range(double value, double minval, double maxval, const char *optionName)
	/*
	FUNCTION: check whether parameter values are within allowed range
	INPUT:    none
	OUTPUT:   none
	COMMENTS: none
	*/
{
	if (value < minval || value > maxval) {
		fprintf(stderr, "Error: Option `%s' out of range [%g, %g]\n",
			optionName, minval, maxval);
		exit(1);
	}
}

