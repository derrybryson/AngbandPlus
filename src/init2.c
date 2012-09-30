/* File: init2.c */

/* Purpose: Initialization (part 2) -BEN- */

#include "angband.h"


#ifdef CHECK_MODIFICATION_TIME
#include <sys/types.h>
#include <sys/stat.h>
#endif /* CHECK_MODIFICATION_TIME */


/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * The "init1.c" file is used only to parse the ascii template files,
 * to create the binary image files.  If you include the binary image
 * files instead of the ascii template files, then you can undefine
 * "ALLOW_TEMPLATES", saving about 20K by removing "init1.c".  Note
 * that the binary image files are extremely system dependant.
 */



/*
 * Find the default paths to all of our important sub-directories.
 *
 * The purpose of each sub-directory is described in "variable.c".
 *
 * All of the sub-directories should, by default, be located inside
 * the main "lib" directory, whose location is very system dependant.
 *
 * This function takes a writable buffer, initially containing the
 * "path" to the "lib" directory, for example, "/pkg/lib/angband/",
 * or a system dependant string, for example, ":lib:".  The buffer
 * must be large enough to contain at least 32 more characters.
 *
 * Various command line options may allow some of the important
 * directories to be changed to user-specified directories, most
 * importantly, the "info" and "user" and "save" directories,
 * but this is done after this function, see "main.c".
 *
 * In general, the initial path should end in the appropriate "PATH_SEP"
 * string.  All of the "sub-directory" paths (created below or supplied
 * by the user) will NOT end in the "PATH_SEP" string, see the special
 * "path_build()" function in "util.c" for more information.
 *
 * Mega-Hack -- support fat raw files under NEXTSTEP, using special
 * "suffixed" directories for the "ANGBAND_DIR_DATA" directory, but
 * requiring the directories to be created by hand by the user.
 *
 * Hack -- first we free all the strings, since this is known
 * to succeed even if the strings have not been allocated yet,
 * as long as the variables start out as "NULL".  This allows
 * this function to be called multiple times, for example, to
 * try several base "path" values until a good one is found.
 */
void init_file_paths(char *path)
{
	char *tail;


	/*** Free everything ***/

	/* Free the main path */
	string_free(ANGBAND_DIR);

	/* Free the sub-paths */
	string_free(ANGBAND_DIR_APEX);
	string_free(ANGBAND_DIR_BONE);
	string_free(ANGBAND_DIR_DATA);
	string_free(ANGBAND_DIR_EDIT);
	string_free(ANGBAND_DIR_FILE);
	string_free(ANGBAND_DIR_HELP);
	string_free(ANGBAND_DIR_INFO);
	string_free(ANGBAND_DIR_SAVE);
	string_free(ANGBAND_DIR_SCPT);
	string_free(ANGBAND_DIR_USER);
	string_free(ANGBAND_DIR_XTRA);


	/*** Prepare the "path" ***/

	/* Hack -- save the main directory */
	ANGBAND_DIR = string_make(path);

	/* Prepare to append to the Base Path */
	tail = path + strlen(path);


#ifdef VM

	/*** Use "flat" paths with VM/ESA ***/

	/* Use "blank" path names */
	ANGBAND_DIR_APEX = string_make("");
	ANGBAND_DIR_BONE = string_make("");
	ANGBAND_DIR_DATA = string_make("");
	ANGBAND_DIR_EDIT = string_make("");
	ANGBAND_DIR_FILE = string_make("");
	ANGBAND_DIR_HELP = string_make("");
	ANGBAND_DIR_INFO = string_make("");
	ANGBAND_DIR_SAVE = string_make("");
        ANGBAND_DIR_SCPT = string_make("");
	ANGBAND_DIR_USER = string_make("");
	ANGBAND_DIR_XTRA = string_make("");

#else /* VM */


	/*** Build the sub-directory names ***/

	/* Build a path name */
	strcpy(tail, "apex");
	ANGBAND_DIR_APEX = string_make(path);

	/* Build a path name */
	strcpy(tail, "bone");
	ANGBAND_DIR_BONE = string_make(path);

	/* Build a path name */
	strcpy(tail, "data");
	ANGBAND_DIR_DATA = string_make(path);

	/* Build a path name */
	strcpy(tail, "edit");
	ANGBAND_DIR_EDIT = string_make(path);

	/* Build a path name */
	strcpy(tail, "file");
	ANGBAND_DIR_FILE = string_make(path);

	/* Build a path name */
	strcpy(tail, "help");
	ANGBAND_DIR_HELP = string_make(path);

	/* Build a path name */
	strcpy(tail, "info");
	ANGBAND_DIR_INFO = string_make(path);

	/* Build a path name */
	strcpy(tail, "save");
	ANGBAND_DIR_SAVE = string_make(path);

	/* Build a path name */
        strcpy(tail, "scpt");
        ANGBAND_DIR_SCPT = string_make(path);

	/* Build a path name */
	strcpy(tail, "user");
	ANGBAND_DIR_USER = string_make(path);

	/* Build a path name */
	strcpy(tail, "xtra");
	ANGBAND_DIR_XTRA = string_make(path);

#endif /* VM */


#ifdef NeXT

	/* Allow "fat binary" usage with NeXT */
	if (TRUE)
	{
		cptr next = NULL;

# if defined(m68k)
		next = "m68k";
# endif

# if defined(i386)
		next = "i386";
# endif

# if defined(sparc)
		next = "sparc";
# endif

# if defined(hppa)
		next = "hppa";
# endif

		/* Use special directory */
		if (next)
		{
			/* Forget the old path name */
			string_free(ANGBAND_DIR_DATA);

			/* Build a new path name */
			sprintf(tail, "data-%s", next);
			ANGBAND_DIR_DATA = string_make(path);
		}
	}

#endif /* NeXT */

}



#ifdef ALLOW_TEMPLATES


/*
 * Hack -- help give useful error messages
 */
s16b error_idx;
s16b error_line;


/*
 * Hack -- help initialize the fake "name" and "text" arrays when
 * parsing an "ascii" template file.
 */
u32b fake_name_size;
u32b fake_text_size;


/*
 * Standard error message text
 */
static cptr err_str[8] =
{
	NULL,
	"parse error",
	"obsolete file",
	"missing record header",
	"non-sequential records",
	"invalid flag specification",
	"undefined directive",
	"out of memory"
};


#endif /* ALLOW_TEMPLATES */


#ifdef CHECK_MODIFICATION_TIME

static errr check_modification_date(int fd, cptr template_file)
{
	char buf[1024];

	struct stat txt_stat, raw_stat;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, template_file);

	/* Access stats on text file */
	if (stat(buf, &txt_stat))
	{
		/* Error */
		return (-1);
	}

	/* Access stats on raw file */
	if (fstat(fd, &raw_stat))
	{
		/* Error */
		return (-1);
	}

	/* Ensure text file is not newer than raw file */
	if (txt_stat.st_mtime > raw_stat.st_mtime)
	{
		/* Reprocess text file */
		return (-1);
	}

	return (0);
}

#endif /* CHECK_MODIFICATION_TIME */

/*
 * Hack -- take notes on line 23
 */
static void note(cptr str)
{
	Term_erase(0, 23, 255);
	Term_putstr(20, 23, -1, TERM_WHITE, str);
	Term_fresh();
}



/*** Initialize from binary image files ***/


/*
 * Initialize the "f_info" array, by parsing a binary "image" file
 */
static errr init_f_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != f_head->v_major) ||
	    (test.v_minor != f_head->v_minor) ||
	    (test.v_patch != f_head->v_patch) ||
	    (test.v_extra != f_head->v_extra) ||
	    (test.info_num != f_head->info_num) ||
	    (test.info_len != f_head->info_len) ||
	    (test.head_size != f_head->head_size) ||
	    (test.info_size != f_head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*f_head) = test;


	/* Allocate the "f_info" array */
	C_MAKE(f_info, f_head->info_num, feature_type);

	/* Read the "f_info" array */
	fd_read(fd, (char*)(f_info), f_head->info_size);


	/* Allocate the "f_name" array */
	C_MAKE(f_name, f_head->name_size, char);

	/* Read the "f_name" array */
	fd_read(fd, (char*)(f_name), f_head->name_size);


#ifndef DELAY_LOAD_F_TEXT

	/* Allocate the "f_text" array */
	C_MAKE(f_text, f_head->text_size, char);

	/* Read the "f_text" array */
	fd_read(fd, (char*)(f_text), f_head->text_size);

#endif /* DELAY_LOAD_F_TEXT */


	/* Success */
	return (0);
}



/*
 * Initialize the "f_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_f_info(void)
{
	int fd;

	int mode = 0644;

	errr err = 0;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the header ***/

	/* Allocate the "header" */
	MAKE(f_head, header);

	/* Save the "version" */
	f_head->v_major = VERSION_MAJOR;
	f_head->v_minor = VERSION_MINOR;
	f_head->v_patch = VERSION_PATCH;
	f_head->v_extra = 0;

	/* Save the "record" information */
	f_head->info_num = max_f_idx;
	f_head->info_len = sizeof(feature_type);

	/* Save the size of "f_head" and "f_info" */
	f_head->head_size = sizeof(header);
	f_head->info_size = f_head->info_num * f_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "f_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, "f_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_f_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
		msg_print("Ignoring obsolete/defective 'f_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

	/* Fake the size of "f_name" and "f_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "f_info" array */
	C_MAKE(f_info, f_head->info_num, feature_type);

	/* Hack -- make "fake" arrays */
	C_MAKE(f_name, fake_name_size, char);
	C_MAKE(f_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "f_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
	if (!fp) quit("Cannot open 'f_info.txt' file.");

	/* Parse the file */
	err = init_f_info_txt(fp, buf);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d at line %d of 'f_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
		quit("Error in 'f_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "f_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
		fd_write(fd, (char*)(f_head), f_head->head_size);

		/* Dump the "f_info" array */
		fd_write(fd, (char*)(f_info), f_head->info_size);

		/* Dump the "f_name" array */
		fd_write(fd, (char*)(f_name), f_head->name_size);

		/* Dump the "f_text" array */
		fd_write(fd, (char*)(f_text), f_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

	/* Free the "f_info" array */
	C_KILL(f_info, f_head->info_num, feature_type);

	/* Hack -- Free the "fake" arrays */
	C_KILL(f_name, fake_name_size, char);
	C_KILL(f_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif	/* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "f_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd < 0) quit("Cannot load 'f_info.raw' file.");

	/* Attempt to parse the "raw" file */
	err = init_f_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
	if (err) quit("Cannot parse 'f_info.raw' file.");

	/* Success */
	return (0);
}



/*
 * Initialize the "k_info" array, by parsing a binary "image" file
 */
static errr init_k_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != k_head->v_major) ||
	    (test.v_minor != k_head->v_minor) ||
	    (test.v_patch != k_head->v_patch) ||
	    (test.v_extra != k_head->v_extra) ||
	    (test.info_num != k_head->info_num) ||
	    (test.info_len != k_head->info_len) ||
	    (test.head_size != k_head->head_size) ||
	    (test.info_size != k_head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*k_head) = test;


	/* Allocate the "k_info" array */
	C_MAKE(k_info, k_head->info_num, object_kind);

	/* Read the "k_info" array */
	fd_read(fd, (char*)(k_info), k_head->info_size);


	/* Allocate the "k_name" array */
	C_MAKE(k_name, k_head->name_size, char);

	/* Read the "k_name" array */
	fd_read(fd, (char*)(k_name), k_head->name_size);


#ifndef DELAY_LOAD_K_TEXT

	/* Allocate the "k_text" array */
	C_MAKE(k_text, k_head->text_size, char);

	/* Read the "k_text" array */
	fd_read(fd, (char*)(k_text), k_head->text_size);

#endif /* DELAY_LOAD_K_TEXT */


	/* Success */
	return (0);
}



/*
 * Initialize the "k_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_k_info(void)
{
	int fd;

	int mode = 0644;

	errr err = 0;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the header ***/

	/* Allocate the "header" */
	MAKE(k_head, header);

	/* Save the "version" */
	k_head->v_major = VERSION_MAJOR;
	k_head->v_minor = VERSION_MINOR;
	k_head->v_patch = VERSION_PATCH;
	k_head->v_extra = 0;

	/* Save the "record" information */
	k_head->info_num = max_k_idx;
	k_head->info_len = sizeof(object_kind);

	/* Save the size of "k_head" and "k_info" */
	k_head->head_size = sizeof(header);
	k_head->info_size = k_head->info_num * k_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "k_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, "k_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_k_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
		msg_print("Ignoring obsolete/defective 'k_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

	/* Fake the size of "k_name" and "k_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "k_info" array */
	C_MAKE(k_info, k_head->info_num, object_kind);

	/* Hack -- make "fake" arrays */
	C_MAKE(k_name, fake_name_size, char);
	C_MAKE(k_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "k_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
	if (!fp) quit("Cannot open 'k_info.txt' file.");

	/* Parse the file */
	err = init_k_info_txt(fp, buf);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d at line %d of 'k_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
		quit("Error in 'k_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "k_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
		fd_write(fd, (char*)(k_head), k_head->head_size);

		/* Dump the "k_info" array */
		fd_write(fd, (char*)(k_info), k_head->info_size);

		/* Dump the "k_name" array */
		fd_write(fd, (char*)(k_name), k_head->name_size);

		/* Dump the "k_text" array */
		fd_write(fd, (char*)(k_text), k_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

	/* Free the "k_info" array */
	C_KILL(k_info, k_head->info_num, object_kind);

	/* Hack -- Free the "fake" arrays */
	C_KILL(k_name, fake_name_size, char);
	C_KILL(k_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif	/* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "k_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd < 0) quit("Cannot load 'k_info.raw' file.");

	/* Attempt to parse the "raw" file */
	err = init_k_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
	if (err) quit("Cannot parse 'k_info.raw' file.");

	/* Success */
	return (0);
}



/*
 * Initialize the "a_info" array, by parsing a binary "image" file
 */
static errr init_a_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != a_head->v_major) ||
	    (test.v_minor != a_head->v_minor) ||
	    (test.v_patch != a_head->v_patch) ||
	    (test.v_extra != a_head->v_extra) ||
	    (test.info_num != a_head->info_num) ||
	    (test.info_len != a_head->info_len) ||
	    (test.head_size != a_head->head_size) ||
	    (test.info_size != a_head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*a_head) = test;


	/* Allocate the "a_info" array */
	C_MAKE(a_info, a_head->info_num, artifact_type);

	/* Read the "a_info" array */
	fd_read(fd, (char*)(a_info), a_head->info_size);


	/* Allocate the "a_name" array */
	C_MAKE(a_name, a_head->name_size, char);

	/* Read the "a_name" array */
	fd_read(fd, (char*)(a_name), a_head->name_size);


#ifndef DELAY_LOAD_A_TEXT

	/* Allocate the "a_text" array */
	C_MAKE(a_text, a_head->text_size, char);

	/* Read the "a_text" array */
	fd_read(fd, (char*)(a_text), a_head->text_size);

#endif /* DELAY_LOAD_A_TEXT */


	/* Success */
	return (0);
}



/*
 * Initialize the "a_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_a_info(void)
{
	int fd;

	int mode = 0644;

	errr err = 0;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the "header" ***/

	/* Allocate the "header" */
	MAKE(a_head, header);

	/* Save the "version" */
	a_head->v_major = VERSION_MAJOR;
	a_head->v_minor = VERSION_MINOR;
	a_head->v_patch = VERSION_PATCH;
	a_head->v_extra = 0;

	/* Save the "record" information */
	a_head->info_num = max_a_idx;
	a_head->info_len = sizeof(artifact_type);

	/* Save the size of "a_head" and "a_info" */
	a_head->head_size = sizeof(header);
	a_head->info_size = a_head->info_num * a_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "a_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, "a_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_a_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
		msg_print("Ignoring obsolete/defective 'a_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

	/* Fake the size of "a_name" and "a_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "a_info" array */
	C_MAKE(a_info, a_head->info_num, artifact_type);

	/* Hack -- make "fake" arrays */
	C_MAKE(a_name, fake_name_size, char);
	C_MAKE(a_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "a_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
	if (!fp) quit("Cannot open 'a_info.txt' file.");

	/* Parse the file */
	err = init_a_info_txt(fp, buf);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d at line %d of 'a_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
		quit("Error in 'a_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "a_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
		fd_write(fd, (char*)(a_head), a_head->head_size);

		/* Dump the "a_info" array */
		fd_write(fd, (char*)(a_info), a_head->info_size);

		/* Dump the "a_name" array */
		fd_write(fd, (char*)(a_name), a_head->name_size);

		/* Dump the "a_text" array */
		fd_write(fd, (char*)(a_text), a_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

	/* Free the "a_info" array */
	C_KILL(a_info, a_head->info_num, artifact_type);

	/* Hack -- Free the "fake" arrays */
	C_KILL(a_name, fake_name_size, char);
	C_KILL(a_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif	/* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "a_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd < 0) quit("Cannot open 'a_info.raw' file.");

	/* Attempt to parse the "raw" file */
	err = init_a_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
	if (err) quit("Cannot parse 'a_info.raw' file.");

	/* Success */
	return (0);
}



/*
 * Initialize the "e_info" array, by parsing a binary "image" file
 */
static errr init_e_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != e_head->v_major) ||
	    (test.v_minor != e_head->v_minor) ||
	    (test.v_patch != e_head->v_patch) ||
	    (test.v_extra != e_head->v_extra) ||
	    (test.info_num != e_head->info_num) ||
	    (test.info_len != e_head->info_len) ||
	    (test.head_size != e_head->head_size) ||
	    (test.info_size != e_head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*e_head) = test;


	/* Allocate the "e_info" array */
	C_MAKE(e_info, e_head->info_num, ego_item_type);

	/* Read the "e_info" array */
	fd_read(fd, (char*)(e_info), e_head->info_size);


	/* Allocate the "e_name" array */
	C_MAKE(e_name, e_head->name_size, char);

	/* Read the "e_name" array */
	fd_read(fd, (char*)(e_name), e_head->name_size);


#ifndef DELAY_LOAD_E_TEXT

	/* Allocate the "e_text" array */
	C_MAKE(e_text, e_head->text_size, char);

	/* Read the "e_text" array */
	fd_read(fd, (char*)(e_text), e_head->text_size);

#endif /* DELAY_LOAD_E_TEXT */


	/* Success */
	return (0);
}



/*
 * Initialize the "e_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_e_info(void)
{
	int fd;

	int mode = 0644;

	errr err = 0;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the "header" ***/

	/* Allocate the "header" */
	MAKE(e_head, header);

	/* Save the "version" */
	e_head->v_major = VERSION_MAJOR;
	e_head->v_minor = VERSION_MINOR;
	e_head->v_patch = VERSION_PATCH;
	e_head->v_extra = 0;

	/* Save the "record" information */
	e_head->info_num = max_e_idx;
	e_head->info_len = sizeof(ego_item_type);

	/* Save the size of "e_head" and "e_info" */
	e_head->head_size = sizeof(header);
	e_head->info_size = e_head->info_num * e_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "e_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{

#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, "e_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		
		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_e_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
		msg_print("Ignoring obsolete/defective 'e_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

	/* Fake the size of "e_name" and "e_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "e_info" array */
	C_MAKE(e_info, e_head->info_num, ego_item_type);

	/* Hack -- make "fake" arrays */
	C_MAKE(e_name, fake_name_size, char);
	C_MAKE(e_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "e_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
	if (!fp) quit("Cannot open 'e_info.txt' file.");

	/* Parse the file */
	err = init_e_info_txt(fp, buf);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d at line %d of 'e_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
		quit("Error in 'e_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "e_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
		fd_write(fd, (char*)(e_head), e_head->head_size);

		/* Dump the "e_info" array */
		fd_write(fd, (char*)(e_info), e_head->info_size);

		/* Dump the "e_name" array */
		fd_write(fd, (char*)(e_name), e_head->name_size);

		/* Dump the "e_text" array */
		fd_write(fd, (char*)(e_text), e_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

	/* Free the "e_info" array */
	C_KILL(e_info, e_head->info_num, ego_item_type);

	/* Hack -- Free the "fake" arrays */
	C_KILL(e_name, fake_name_size, char);
	C_KILL(e_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif	/* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "e_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd < 0) quit("Cannot load 'e_info.raw' file.");

	/* Attempt to parse the "raw" file */
	err = init_e_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
	if (err) quit("Cannot parse 'e_info.raw' file.");

	/* Success */
	return (0);
}



/*
 * Initialize the "r_info" array, by parsing a binary "image" file
 */
static errr init_r_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != r_head->v_major) ||
	    (test.v_minor != r_head->v_minor) ||
	    (test.v_patch != r_head->v_patch) ||
	    (test.v_extra != r_head->v_extra) ||
	    (test.info_num != r_head->info_num) ||
	    (test.info_len != r_head->info_len) ||
	    (test.head_size != r_head->head_size) ||
	    (test.info_size != r_head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*r_head) = test;


	/* Allocate the "r_info" array */
	C_MAKE(r_info, r_head->info_num, monster_race);

	/* Read the "r_info" array */
	fd_read(fd, (char*)(r_info), r_head->info_size);


	/* Allocate the "r_name" array */
	C_MAKE(r_name, r_head->name_size, char);

	/* Read the "r_name" array */
	fd_read(fd, (char*)(r_name), r_head->name_size);


#ifndef DELAY_LOAD_R_TEXT

	/* Allocate the "r_text" array */
	C_MAKE(r_text, r_head->text_size, char);

	/* Read the "r_text" array */
	fd_read(fd, (char*)(r_text), r_head->text_size);

#endif /* DELAY_LOAD_R_TEXT */


	/* Success */
	return (0);
}


/*
 * Initialize the "d_info" array, by parsing a binary "image" file
 */
static errr init_d_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
            (test.v_major != d_head->v_major) ||
            (test.v_minor != d_head->v_minor) ||
            (test.v_patch != d_head->v_patch) ||
            (test.v_extra != d_head->v_extra) ||
            (test.info_num != d_head->info_num) ||
            (test.info_len != d_head->info_len) ||
            (test.head_size != d_head->head_size) ||
            (test.info_size != d_head->info_size))
	{
		/* Error */
                return (-1);
	}


	/* Accept the header */
        (*d_head) = test;


        /* Allocate the "d_info" array */
        C_MAKE(d_info, d_head->info_num, dungeon_info_type);

        /* Read the "d_info" array */
        fd_read(fd, (char*)(d_info), d_head->info_size);


	/* Allocate the "r_name" array */
        C_MAKE(d_name, d_head->name_size, char);

        /* Read the "d_name" array */
        fd_read(fd, (char*)(d_name), d_head->name_size);

        /* Allocate the "d_text" array */
        C_MAKE(d_text, d_head->text_size, char);

        /* Read the "d_text" array */
        fd_read(fd, (char*)(d_text), d_head->text_size);


	/* Success */
	return (0);
}


/*
 * Initialize the "r_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_r_info(void)
{
	int fd;

	int mode = 0644;

	errr err = 0;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the header ***/

	/* Allocate the "header" */
	MAKE(r_head, header);

	/* Save the "version" */
	r_head->v_major = VERSION_MAJOR;
	r_head->v_minor = VERSION_MINOR;
	r_head->v_patch = VERSION_PATCH;
	r_head->v_extra = 0;

	/* Save the "record" information */
	r_head->info_num = max_r_idx;
	r_head->info_len = sizeof(monster_race);

	/* Save the size of "r_head" and "r_info" */
	r_head->head_size = sizeof(header);
	r_head->info_size = r_head->info_num * r_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "r_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, "r_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_r_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
		msg_print("Ignoring obsolete/defective 'r_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

	/* Assume the size of "r_name" and "r_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "r_info" array */
	C_MAKE(r_info, r_head->info_num, monster_race);

	/* Hack -- make "fake" arrays */
	C_MAKE(r_name, fake_name_size, char);
	C_MAKE(r_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "r_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
	if (!fp) quit("Cannot open 'r_info.txt' file.");

	/* Parse the file */
	err = init_r_info_txt(fp, buf);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d at line %d of 'r_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
		quit("Error in 'r_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "r_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
		fd_write(fd, (char*)(r_head), r_head->head_size);

		/* Dump the "r_info" array */
		fd_write(fd, (char*)(r_info), r_head->info_size);

		/* Dump the "r_name" array */
		fd_write(fd, (char*)(r_name), r_head->name_size);

		/* Dump the "r_text" array */
		fd_write(fd, (char*)(r_text), r_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

	/* Free the "r_info" array */
	C_KILL(r_info, r_head->info_num, monster_race);

	/* Hack -- Free the "fake" arrays */
	C_KILL(r_name, fake_name_size, char);
	C_KILL(r_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif	/* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "r_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd < 0) quit("Cannot load 'r_info.raw' file.");

	/* Attempt to parse the "raw" file */
	err = init_r_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
	if (err) quit("Cannot parse 'r_info.raw' file.");

	/* Success */
	return (0);
}


/*
 * Initialize the "d_info" array
 *
 * Note that we let each entry have a unique "name" and "short name" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_d_info(void)
{
	int fd;

	int mode = 0644;

	errr err = 0;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the header ***/

	/* Allocate the "header" */
        MAKE(d_head, header);

	/* Save the "version" */
        d_head->v_major = VERSION_MAJOR;
        d_head->v_minor = VERSION_MINOR;
        d_head->v_patch = VERSION_PATCH;
        d_head->v_extra = 0;

	/* Save the "record" information */
        d_head->info_num = max_d_idx;
        d_head->info_len = sizeof(dungeon_info_type);

        /* Save the size of "d_head" and "d_info" */
        d_head->head_size = sizeof(header);
        d_head->info_size = d_head->info_num * d_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
        path_build(buf, 1024, ANGBAND_DIR_DATA, "d_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

                err = check_modification_date(fd, "d_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
                        err = init_d_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
                msg_print("Ignoring obsolete/defective 'd_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

        /* Assume the size of "d_name" and "d_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "r_info" array */
        C_MAKE(d_info, d_head->info_num, dungeon_info_type);

	/* Hack -- make "fake" arrays */
        C_MAKE(d_name, fake_name_size, char);
        C_MAKE(d_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
        path_build(buf, 1024, ANGBAND_DIR_EDIT, "d_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
        if (!fp) quit("Cannot open 'd_info.txt' file.");

	/* Parse the file */
        err = init_d_info_txt(fp, buf);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
                msg_format("Error %d at line %d df 'd_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
                quit("Error in 'd_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
        path_build(buf, 1024, ANGBAND_DIR_DATA, "d_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
                fd_write(fd, (char*)(d_head), d_head->head_size);

		/* Dump the "r_info" array */
                fd_write(fd, (char*)(d_info), d_head->info_size);

		/* Dump the "r_name" array */
                fd_write(fd, (char*)(d_name), d_head->name_size);

		/* Dump the "r_text" array */
                fd_write(fd, (char*)(d_text), d_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

        /* Free the "d_info" array */
        C_KILL(d_info, d_head->info_num, dungeon_info_type);

	/* Hack -- Free the "fake" arrays */
        C_KILL(d_name, fake_name_size, char);
        C_KILL(d_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif	/* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
        path_build(buf, 1024, ANGBAND_DIR_DATA, "d_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
        if (fd < 0) quit("Cannot load 'd_info.raw' file.");

	/* Attempt to parse the "raw" file */
        err = init_d_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
        if (err) quit("Cannot parse 'd_info.raw' file.");

	/* Success */
	return (0);
}


/* Add various player ghost attributes depending on race. -LM- */
static void process_ghost_race(int ghost_race, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	byte n;

	switch(ghost_race)
	{
		/* Human */
		case 0:
		{
			/* No differences */
			break;
		}
		/* Half-Elf */
		case 1:
		{
			if (r_ptr->freq_spell) r_ptr->freq_spell += 3;
			r_ptr->aaf += 3;
			r_ptr->hdice = 6 * r_ptr->hdice / 7;
			break;
		}
		/* Elf */
		case 2:
		{
			if (r_ptr->freq_spell) r_ptr->freq_spell += 5;
			r_ptr->aaf += 5;
			r_ptr->hdice = 4 * r_ptr->hdice / 5;
			if (r_ptr->flags3 & (RF3_HURT_LITE)) 
				r_ptr->flags3 &= ~(RF3_HURT_LITE);
			break;
		}
                /* Dark Elf */
                case RACE_DARK_ELF:
		{
                        if (r_ptr->freq_spell) r_ptr->freq_spell += 7;
                        r_ptr->aaf += 6;
			r_ptr->hdice = 4 * r_ptr->hdice / 5;
			break;
		}
		/* Hobbit */
		case 3:
		{	
			r_ptr->hdice = 3 * r_ptr->hdice / 4;

			if (randint(3) == 1)
			{
				for (n = 0; n < 4; n++)
				{
					if (r_ptr->blow[n].effect == RBE_HURT)
					{
						if (randint(2) == 1) 
							r_ptr->blow[n].effect = RBE_EAT_GOLD;
						else r_ptr->blow[n].effect = RBE_EAT_ITEM;
	
						r_ptr->blow[n].d_side = 
							2 * r_ptr->blow[n].d_side / 3;
						break;
					}
				}
			}
			else
			{
				if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 8;

                                if (r_ptr->level <= 15) r_ptr->flags4 |= (RF4_ARROW_1);
                                else r_ptr->flags4 |= (RF4_ARROW_2);
			}

			break;
		}
		/* Gnome */
		case 4:
		{
			r_ptr->flags6 |= (RF6_BLINK);
			r_ptr->flags3 |= (RF3_NO_SLEEP);
			r_ptr->hdice = 4 * r_ptr->hdice / 5;
			break;
		}
		/* Dwarf */
		case 5:
                case RACE_NIBELUNG:
		{
			r_ptr->hdice = 6 * r_ptr->hdice / 5;
			break;
		}
		/* Half-Orc */
		case 6:
                case RACE_KOBOLD:
		{
			r_ptr->flags3 |= (RF3_ORC);
			break;
		}
		/* Half-Troll */
		case 7:
		{
			if (r_ptr->freq_spell > 5) 
			r_ptr->freq_spell /= 2;

			r_ptr->flags3 |= (RF3_TROLL);

			r_ptr->hdice = 3 * r_ptr->hdice / 2;
			r_ptr->aaf = 2;

			r_ptr->ac += r_ptr->level / 10 + 10;

                        if (r_ptr->speed < 111) r_ptr->speed -= 2;
                        else r_ptr->speed -= 4;

			for (n = 0; n < 4; n++)
			{
				r_ptr->blow[n].d_side = 4 * r_ptr->blow[n].d_side / 3;
			}

			break;
		}
		/* Dunadan */
		case 8:
		{
			r_ptr->ac += r_ptr->level / 10 + 5;

			for (n = 0; n < 4; n++)
			{
				if (randint(2) == 1) 
					r_ptr->blow[n].d_side = 6 * r_ptr->blow[n].d_side / 5;
			}
			break;
		}
		/* High-Elf */
		case 9:
		{
			r_ptr->ac += r_ptr->level / 10 + 2;

			if (r_ptr->freq_spell) r_ptr->freq_spell += 8;
			r_ptr->aaf += 6;
			if (r_ptr->flags3 & (RF3_HURT_LITE)) 
				r_ptr->flags3 &= ~(RF3_HURT_LITE);
			break;
		}
                case RACE_BARBARIAN:
                {
                        r_ptr->flags3 |= RF3_NO_FEAR;
                        break;
                }
                case RACE_VAMPIRE:
                {
                        r_ptr->flags3 |= RF3_RES_NETH;
                        r_ptr->flags3 |= RF3_IM_COLD;
                        r_ptr->flags3 |= RF3_IM_POIS;
                        r_ptr->flags3 |= RF3_HURT_LITE;
                        break;
                }
                case RACE_SPECTRE:
                {
                        r_ptr->flags3 |= RF3_RES_NETH;
                        r_ptr->flags3 |= RF3_IM_POIS;
                        r_ptr->flags3 |= RF3_IM_COLD;
                        r_ptr->flags2 |= RF2_INVISIBLE;
                        r_ptr->flags2 |= RF2_PASS_WALL;
                        break;
                }
                case RACE_RKNIGHT:
                {
                        r_ptr->speed += 10;
                        break;
                }
                case RACE_ENT:
                {
                        r_ptr->flags3 |= RF3_SUSCEP_FIRE;
                        r_ptr->speed -= 3;

			for (n = 0; n < 4; n++)
                                r_ptr->blow[n].d_side = 5 * r_ptr->blow[n].d_side / 3;
                        break;
                }
                case RACE_DRAGONRIDER:
                {
                        r_ptr->flags3 |= RF3_DRAGONRIDER;
                        r_ptr->flags3 |= RF3_IM_ACID;
                        r_ptr->flags3 |= RF3_IM_ELEC;
                        r_ptr->flags3 |= RF3_IM_FIRE;
                        r_ptr->flags3 |= RF3_IM_COLD;
                        r_ptr->flags3 |= RF3_IM_POIS;

                        r_ptr->flags4 |= RF4_BR_FIRE;
                        break;
                }
                case RACE_MOLD:
                {
                        r_ptr->flags1 |= RF1_NEVER_MOVE;
                        r_ptr->flags3 |= RF3_RES_NETH;
                        r_ptr->flags3 |= RF3_RES_NEXU;
                        break;
                }
	}
}

/* Add various attributes player ghost depending on class. -LM- */
static void process_ghost_class(int ghost_class, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	byte n;

	/* Note the care taken to make sure that all monsters that get spells 
	 * can also cast them, since not all racial templates include spells.
	 */
	switch(ghost_class)
	{
		/* Warrior */
		case 0:
                case CLASS_MIMIC:
		{
			if (r_ptr->freq_spell <= 10) r_ptr->freq_spell = 5;
			else r_ptr->freq_spell -= 5;

			r_ptr->hdice = 5 * r_ptr->hdice / 4;
			r_ptr->ac += r_ptr->level / 10 + 5;

			for (n = 0; n < 4; n++)
			{
				if (r_ptr->blow[n].effect != RBE_HURT)
				{
					r_ptr->blow[n].effect = RBE_HURT;

					r_ptr->blow[n].d_side = 3 * r_ptr->blow[n].d_side / 2;
					break;
				}
			}

			break;
		}
                /* Beastmaster */
                case CLASS_BEASTMASTER:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 12;
			else r_ptr->freq_spell += 10;

			r_ptr->hdice = 5 * r_ptr->hdice / 4;
                        r_ptr->ac += r_ptr->level / 10;

			for (n = 0; n < 4; n++)
			{
				if (r_ptr->blow[n].effect != RBE_HURT)
				{
					r_ptr->blow[n].effect = RBE_HURT;

					r_ptr->blow[n].d_side = 3 * r_ptr->blow[n].d_side / 2;
					break;
				}
			}

                        r_ptr->flags6 |= RF6_S_MONSTER;
                        r_ptr->flags6 |= RF6_S_MONSTERS;
                        r_ptr->flags6 |= RF6_S_ANT;
                        r_ptr->flags6 |= RF6_S_SPIDER;
                        r_ptr->flags6 |= RF6_S_RNG;
                        r_ptr->flags6 |= RF6_S_CYBER;
                        r_ptr->flags6 |= RF6_S_HOUND;
                        r_ptr->flags6 |= RF6_S_WRAITH;
                        r_ptr->flags6 |= RF6_S_DRAGON;
                        r_ptr->flags6 |= RF6_S_HI_DRAGON;

			break;
		}
		/* Mage */
		case 1:
                case CLASS_ALCHEMIST:
                case CLASS_HIGH_MAGE:
                case CLASS_POWERMAGE:
                case CLASS_SYMBIANT:
                case CLASS_RUNECRAFTER:
                case CLASS_HARPER:
                case CLASS_POSSESSOR:
                case CLASS_WIZARD:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 12;
			else r_ptr->freq_spell += 10;

                        if (r_ptr->level < 15) r_ptr->flags5 |= (RF5_MISSILE);
                        else if (r_ptr->level >= 15) r_ptr->flags5 |= (RF5_BA_POIS);
                        else if (r_ptr->level >= 25) r_ptr->flags5 |= (RF5_BA_ELEC);
                        else if (r_ptr->level >= 35) r_ptr->flags5 |= (RF5_BA_COLD);
                        else if (r_ptr->level >= 50) r_ptr->flags5 |= (RF5_BA_ACID);
                        else if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_BA_MANA);

                        if (r_ptr->level >= 20) r_ptr->flags6 |= (RF6_HASTE);
                        if (r_ptr->level > 40) r_ptr->speed += 5;
                        if (r_ptr->speed > 150) r_ptr->speed = 150;

			r_ptr->flags6 |= (RF6_BLINK);
                        if (r_ptr->level > 45) r_ptr->flags6 |= (RF6_TPORT);

			r_ptr->hdice = 2 * r_ptr->hdice / 3;

			for (n = 0; n < 4; n++)
			{
				if (randint(3) == 1) r_ptr->blow[n].d_side = 4 * r_ptr->blow[n].d_side / 5;
			}
			
			break;
		}
		/* Priest */
		case 2:
                case CLASS_PRIOR:
                case CLASS_MINDCRAFTER:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 10;
			else r_ptr->freq_spell += 5;

                        if (r_ptr->level <= 15) r_ptr->flags5 |= (RF5_CAUSE_1);
                        else if (r_ptr->level <= 30) r_ptr->flags5 |= (RF5_CAUSE_2);
                        else if (r_ptr->level <= 45) r_ptr->flags5 |= (RF5_CAUSE_3);
                        else if (r_ptr->level <= 60) r_ptr->flags5 |= (RF5_CAUSE_4);
			else r_ptr->flags6 |= (RF6_S_MONSTERS);

                        if (r_ptr->level > 50) r_ptr->flags6 |= (RF6_HEAL);

			r_ptr->hdice = 4 * r_ptr->hdice / 5;

			break;
		}
		/* Rogue */
		case 3:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 8;

                        if (r_ptr->level <= 30) r_ptr->flags6 |= (RF6_HASTE);
                        else if (r_ptr->level > 30) r_ptr->speed += 5;
                        if (r_ptr->speed > 130) r_ptr->speed = 130;

			r_ptr->hdice = 4 * r_ptr->hdice / 5;

			r_ptr->flags6 |= (RF6_TRAPS);

			for (n = 0; n < 4; n++)
			{
				if (r_ptr->blow[n].effect == RBE_HURT)
				{
					if (randint(2) == 1) r_ptr->blow[n].effect = RBE_EAT_GOLD;
					else r_ptr->blow[n].effect = RBE_EAT_ITEM;

					r_ptr->blow[n].d_side = 2 * r_ptr->blow[n].d_side / 3;
					break;
				}
			}

			break;
		}
		/* Ranger */
		case 4:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 8;

                        if (r_ptr->level <= 15) r_ptr->flags4 |= (RF4_ARROW_1);
                        else r_ptr->flags4 |= (RF4_ARROW_2);

			r_ptr->flags6 |= (RF6_BLINK);

			break;
		}
		/* Paladin */
		case 5:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 8;

			r_ptr->flags4 |= (RF4_SHRIEK);

                        if (r_ptr->level <= 25) r_ptr->flags5 |= (RF5_CAUSE_1);
                        else if (r_ptr->level <= 40) r_ptr->flags5 |= (RF5_CAUSE_2);
                        else if (r_ptr->level <= 55) r_ptr->flags5 |= (RF5_CAUSE_3);
                        else if (r_ptr->level <= 70) r_ptr->flags5 |= (RF5_CAUSE_4);

			r_ptr->flags3 |= (RF3_IM_FIRE | 
					RF3_IM_COLD | RF3_IM_ELEC | RF3_IM_ACID);
			
			break;
		}
                /* Monk */
                case CLASS_MONK:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 10;
			else r_ptr->freq_spell += 5;

                        if (r_ptr->level <= 20) r_ptr->flags5 |= (RF5_MISSILE);
                        else if (r_ptr->level >= 20) r_ptr->flags5 |= (RF5_BO_COLD);
                        else if (r_ptr->level >= 40) r_ptr->flags5 |= (RF5_BO_PLAS);
                        else if (r_ptr->level >= 50) r_ptr->flags5 |= (RF5_BO_WATE);
                        else if (r_ptr->level >= 70) r_ptr->flags5 |= (RF5_BA_WATE);

                        if (r_ptr->level >= 25) r_ptr->flags6 |= RF6_S_ANT;
                        else if (r_ptr->level >= 35) r_ptr->flags6 |= RF6_S_SPIDER;
                        else if (r_ptr->level >= 45) r_ptr->flags6 |= RF6_S_MONSTER;
                        else if (r_ptr->level >= 65) r_ptr->flags6 |= RF6_S_HOUND;

			r_ptr->hdice = 4 * r_ptr->hdice / 5;

			break;
		}
                case CLASS_SORCERER:
		{
                        if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 32;
                        else r_ptr->freq_spell += 30;

                        if (r_ptr->level < 15) r_ptr->flags5 |= (RF5_MISSILE);
                        if (r_ptr->level >= 15) r_ptr->flags5 |= (RF5_BA_POIS);
                        if (r_ptr->level >= 25) r_ptr->flags5 |= (RF5_BA_ELEC);
                        if (r_ptr->level >= 35) r_ptr->flags5 |= (RF5_BA_COLD);
                        if (r_ptr->level >= 50) r_ptr->flags5 |= (RF5_BA_ACID);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_BA_MANA);
                        if (r_ptr->level >= 75) r_ptr->flags4 |= (RF4_ROCKET);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_CONF);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_BLIND);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_SCARE);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_SLOW);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_HOLD);
                        if (r_ptr->level >= 75) r_ptr->flags6 |= (RF6_TELE_TO);
                        if (r_ptr->level >= 75) r_ptr->flags6 |= (RF6_TELE_AWAY);
                        if (r_ptr->level >= 75) r_ptr->flags6 |= (RF6_S_DRAGONRIDER);
                        if (r_ptr->level >= 75) r_ptr->flags6 |= (RF6_S_CYBER);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_BA_NETH);

                        if (r_ptr->level >= 20) r_ptr->flags6 |= (RF6_HASTE);
                        if (r_ptr->level > 40) r_ptr->speed += 5;
                        if (r_ptr->speed > 150) r_ptr->speed = 150;

			r_ptr->flags6 |= (RF6_BLINK);
                        if (r_ptr->level > 45) r_ptr->flags6 |= (RF6_TPORT);

			r_ptr->hdice = 2 * r_ptr->hdice / 3;

			for (n = 0; n < 4; n++)
			{
                                if (randint(3) == 1) r_ptr->blow[n].d_side = 4 * r_ptr->blow[n].d_side / 7;
			}
			
			break;
		}
		/* Mage */
                case CLASS_WARRIOR_MAGE:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 12;
			else r_ptr->freq_spell += 10;

                        if (r_ptr->level < 15) r_ptr->flags5 |= (RF5_MISSILE);
                        if (r_ptr->level >= 15) r_ptr->flags5 |= (RF5_BA_POIS);
                        if (r_ptr->level >= 25) r_ptr->flags5 |= (RF5_BA_ELEC);
                        else if (r_ptr->level >= 35) r_ptr->flags5 |= (RF5_BA_COLD);
                        else if (r_ptr->level >= 50) r_ptr->flags5 |= (RF5_BA_ACID);

                        if (r_ptr->level >= 20) r_ptr->flags6 |= (RF6_HASTE);
                        if (r_ptr->level > 40) r_ptr->speed += 5;
                        if (r_ptr->speed > 150) r_ptr->speed = 150;

			r_ptr->flags6 |= (RF6_BLINK);
                        if (r_ptr->level > 45) r_ptr->flags6 |= (RF6_TPORT);
			break;
		}
		/* Mage */
                case CLASS_CHAOS_WARRIOR:
		{
			if (r_ptr->freq_spell == 0) r_ptr->freq_spell = 12;
			else r_ptr->freq_spell += 10;

                        if (r_ptr->level < 15) r_ptr->flags5 |= (RF5_MISSILE);
                        if (r_ptr->level >= 50) r_ptr->flags4 |= (RF4_BA_CHAO);
                        if (r_ptr->level >= 75) r_ptr->flags5 |= (RF5_BA_MANA);

			r_ptr->flags6 |= (RF6_BLINK);
                        if (r_ptr->level > 45) r_ptr->flags6 |= (RF6_TPORT);
			break;
		}
	}
}

/*
 * Initialize the "ghost_info" array(part of the r_info one)
 */
errr init_ghost_info(int n)
{
	errr err = 0;

	FILE *fp;

        int i, j, k;

        monster_race *r_ptr;

	/* General buffer */
        char buf[1024], name[80];

        char ghost_name[80], damage[20];
        char ghost_desc[320];
        int ghost_sex, ghost_race, ghost_class, ghost_friend;
        int mhp, msp, max_dlv, max_exp, lev, to_a, to_h, to_d, invis,
            regen, ffall, speed, res_acid, res_elec, res_pois, res_fire,
            res_cold, res_conf, res_lite, res_dark, res_fear, res_chaos,
            res_disen, wpn_k_idx, name2, name1, num_blow, dd, ds, ac;

        {
                /* Try to find a good and unused file */
                i = 0;
                sprintf(buf, "%s", ANGBAND_DIR_BONE);
                while(i < 20)
                {                        
                        int rn;

                        sprintf(name, "%s", get_line("bones.txt", buf, -1));

                        rn = rand_int(atoi(name));

                        sprintf(name, "%s", get_line("bones.txt", buf, rn));

                        /* Find if the file is not used */
                        k = 0;
                        for(j = 0; j < MAX_GHOSTS; j++)
                        {
                                if(strcmp(ghost_file[j], name) == 0)
                                        k++;
                        }
                        if(!k) break;

                        i++;
                }
                /* Find nothing ? so return */
                if (i >= 20) return 1;

                /*** Load the bone file ***/

                /* Build the filename */
                path_build(buf, 1024, ANGBAND_DIR_BONE, name);

                /* Attempt to open the bone file */
                fp = my_fopen(buf, "r");

                /* Process existing bone file */
                if (!fp) return (1);

                sprintf(ghost_file[n], name);

                r_ptr = &r_info[GHOST_R_IDX_HEAD + n];

                /* Attempt to parse the bone file */
                err = (fscanf(fp, "%s\n%d\n%d\n%d\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%s\n%d\n%d\n%d",
                              ghost_name, &ghost_sex, &ghost_race, &ghost_class,
                              ghost_desc, &ghost_friend, &mhp, &msp, &max_dlv,
                              &max_exp, &lev, &ac, &to_a, &to_h, &to_d, &invis,
                              &regen, &ffall, &speed, &res_acid, &res_elec,
                              &res_pois, &res_fire, &res_cold, &res_conf,
                              &res_lite, &res_dark, &res_fear, &res_chaos,
                              &res_disen, &wpn_k_idx, &name2, &name1, damage,
                              &num_blow, &dd, &ds) != 38);

                /* Convert the _ into space, it's needed by the %s format of fscanf */
                i = 0;
                while(i < 320)
                {
                        if(ghost_desc[i] == '_') ghost_desc[i] = ' ';
                        i++;
                }

                num_blow = (num_blow > 4) ? 4 : num_blow;

                for(i = 0; i < num_blow; i++)
                {
                        r_ptr->blow[i].method = RBM_HIT;
                        r_ptr->blow[i].effect = RBE_HURT;
                        r_ptr->blow[i].d_dice = dd * num_blow;
                        r_ptr->blow[i].d_side = ds;
                }

                if (ghost_sex == 0) r_ptr->flags1 |= (RF1_FEMALE);
                if (ghost_sex == 1) r_ptr->flags1 |= (RF1_MALE);

                if(res_acid) r_ptr->flags3 |= RF3_IM_ACID;
                if(res_elec) r_ptr->flags3 |= RF3_IM_ELEC;
                if(res_fire) r_ptr->flags3 |= RF3_IM_FIRE;
                if(res_cold) r_ptr->flags3 |= RF3_IM_COLD;
                if(res_pois) r_ptr->flags3 |= RF3_IM_POIS;
                if(res_conf) r_ptr->flags3 |= RF3_NO_CONF;
                if(res_disen) r_ptr->flags3 |= RF3_RES_DISE;
                if(res_fear) r_ptr->flags3 |= RF3_NO_FEAR;
                r_ptr->freq_inate = 10;
                r_ptr->freq_spell = 10;

                r_ptr->level = (max_dlv <= 100)?max_dlv - rand_int(10):100 - rand_int(10);
                r_ptr->rarity = 40;
                r_ptr->speed = speed;
                r_ptr->mexp = max_exp / 1000;
                r_ptr->sleep = 0;
                r_ptr->aaf = 8;
                r_ptr->ac = ac + to_a;

                /* Give full hp */
                r_ptr->hdice = ((mhp / 10) < 1)?1:(mhp / 10);
                r_ptr->hside = mhp;

                sprintf(r_name + r_ptr->name, "%s, the %s %s",
                        ghost_name, race_info[ghost_race].title, class_info[ghost_class].title);

                sprintf(r_text + r_ptr->text, "%s", ghost_desc);

                if(ghost_friend) r_ptr->flags7 |= RF7_PET;

                process_ghost_race(ghost_class, GHOST_R_IDX_HEAD + n);
                process_ghost_class(ghost_class, GHOST_R_IDX_HEAD + n);

                /* Close it */
                my_fclose(fp);
        }

        /* Success */
	return (0);
}



/*
 * Initialize the "v_info" array, by parsing a binary "image" file
 */
static errr init_v_info_raw(int fd)
{
	header test;

	/* Read and Verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != v_head->v_major) ||
	    (test.v_minor != v_head->v_minor) ||
	    (test.v_patch != v_head->v_patch) ||
	    (test.v_extra != v_head->v_extra) ||
	    (test.info_num != v_head->info_num) ||
	    (test.info_len != v_head->info_len) ||
	    (test.head_size != v_head->head_size) ||
	    (test.info_size != v_head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	(*v_head) = test;


	/* Allocate the "v_info" array */
	C_MAKE(v_info, v_head->info_num, vault_type);

	/* Read the "v_info" array */
	fd_read(fd, (char*)(v_info), v_head->info_size);


	/* Allocate the "v_name" array */
	C_MAKE(v_name, v_head->name_size, char);

	/* Read the "v_name" array */
	fd_read(fd, (char*)(v_name), v_head->name_size);


#ifndef DELAY_LOAD_V_TEXT

	/* Allocate the "v_text" array */
	C_MAKE(v_text, v_head->text_size, char);

	/* Read the "v_text" array */
	fd_read(fd, (char*)(v_text), v_head->text_size);

#endif /* DELAY_LOAD_V_TEXT */

	/* Success */
	return (0);
}


/*
 * Initialize the "v_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
errr init_v_info(void)
{
	int fd;

	int mode = 0644;

	errr err;

	FILE *fp;

	/* General buffer */
	char buf[1024];


	/*** Make the header ***/

	/* Allocate the "header" */
	MAKE(v_head, header);

	/* Save the "version" */
	v_head->v_major = VERSION_MAJOR;
	v_head->v_minor = VERSION_MINOR;
	v_head->v_patch = VERSION_PATCH;
	v_head->v_extra = 0;

	/* Save the "record" information */
	v_head->info_num = max_v_idx;
	v_head->info_len = sizeof(vault_type);

	/* Save the size of "v_head" and "v_info" */
	v_head->head_size = sizeof(header);
	v_head->info_size = v_head->info_num * v_head->info_len;


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "v_info.raw");

#if 0
	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);
#else
	fd = -1;
#endif

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, "v_info.txt");

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_v_info_raw(fd);

		/* Close it */
		(void)fd_close(fd);

		/* Success */
		if (!err) return (0);

		/* Information */
		msg_print("Ignoring obsolete/defective 'v_info.raw' file.");
		msg_print(NULL);
	}


	/*** Make the fake arrays ***/

	/* Fake the size of "v_name" and "v_text" */
	fake_name_size = FAKE_NAME_SIZE;
	fake_text_size = FAKE_TEXT_SIZE;

	/* Allocate the "k_info" array */
	C_MAKE(v_info, v_head->info_num, vault_type);

	/* Hack -- make "fake" arrays */
	C_MAKE(v_name, fake_name_size, char);
	C_MAKE(v_text, fake_text_size, char);


	/*** Load the ascii template file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "v_info.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Parse it */
	if (!fp) quit("Cannot open 'v_info.txt' file.");

	/* Parse the file */
	err = init_v_info_txt(fp, buf, TRUE);

	/* Close it */
	my_fclose(fp);

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < 8)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d at line %d of 'v_info.txt'.", err, error_line);
		msg_format("Record %d contains a '%s' error.", error_idx, oops);
		msg_format("Parsing '%s'.", buf);
		msg_print(NULL);

		/* Quit */
		quit("Error in 'v_info.txt' file.");
	}


	/*** Dump the binary image file ***/

	/* File type is "DATA" */
	FILE_TYPE(FILE_TYPE_DATA);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "v_info.raw");

	/* Kill the old file */
	(void)fd_kill(buf);

	/* Attempt to create the raw file */
	fd = fd_make(buf, mode);

	/* Dump to the file */
	if (fd >= 0)
	{
		/* Dump it */
		fd_write(fd, (char*)(v_head), v_head->head_size);

		/* Dump the "v_info" array */
		fd_write(fd, (char*)(v_info), v_head->info_size);

		/* Dump the "v_name" array */
		fd_write(fd, (char*)(v_name), v_head->name_size);

		/* Dump the "v_text" array */
		fd_write(fd, (char*)(v_text), v_head->text_size);

		/* Close */
		(void)fd_close(fd);
	}


	/*** Kill the fake arrays ***/

	/* Free the "v_info" array */
	C_KILL(v_info, v_head->info_num, vault_type);

	/* Hack -- Free the "fake" arrays */
	C_KILL(v_name, fake_name_size, char);
	C_KILL(v_text, fake_text_size, char);

	/* Forget the array sizes */
	fake_name_size = 0;
	fake_text_size = 0;

#endif /* ALLOW_TEMPLATES */


	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, "v_info.raw");

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd < 0) quit("Cannot load 'v_info.raw' file.");

	/* Attempt to parse the "raw" file */
	err = init_v_info_raw(fd);

	/* Close it */
	(void)fd_close(fd);

	/* Error */
	if (err) quit("Cannot parse 'v_info.raw' file.");

	/* Success */
	return (0);
}



/*** Initialize others ***/

/*
 * Hack -- Objects sold in the stores -- by tval/sval pair.
 */
static byte store_table[MAX_STORES][STORE_CHOICES][2] =
{
	{
		/* General Store */

		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },

		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_BISCUIT },
		{ TV_FOOD, SV_FOOD_JERKY },
		{ TV_FOOD, SV_FOOD_JERKY },

		{ TV_FOOD, SV_FOOD_PINT_OF_WINE },
		{ TV_FOOD, SV_FOOD_PINT_OF_ALE },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },

		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_LANTERN },
		{ TV_LITE, SV_LITE_LANTERN },

		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },

		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_SPIKE, 0 },
		{ TV_SPIKE, 0 },

		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_DIGGING, SV_SHOVEL },

		{ TV_DIGGING, SV_PICK },
		{ TV_CLOAK, SV_CLOAK },
		{ TV_CLOAK, SV_CLOAK },
		{ TV_CLOAK, SV_FUR_CLOAK },

		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },
		{ TV_FOOD, SV_FOOD_RATION },

		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_LANTERN },
		{ TV_LITE, SV_LITE_LANTERN },

		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },

		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_DIGGING, SV_SHOVEL }
	},

	{
		/* Armoury */

		{ TV_BOOTS, SV_PAIR_OF_SOFT_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_SOFT_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },

		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_METAL_CAP },
		{ TV_HELM, SV_IRON_HELM },

		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },

		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_STUDDED_LEATHER },
		{ TV_SOFT_ARMOR, SV_HARD_STUDDED_LEATHER },

		{ TV_SOFT_ARMOR, SV_RHINO_HIDE_ARMOR },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL },
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },

		{ TV_HARD_ARMOR, SV_DOUBLE_RING_MAIL },
		{ TV_HARD_ARMOR, SV_AUGMENTED_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_BAR_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_DOUBLE_CHAIN_MAIL },

		{ TV_HARD_ARMOR, SV_METAL_BRIGANDINE_ARMOUR },
		{ TV_HARD_ARMOR, SV_SPLINT_MAIL },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },

		{ TV_GLOVES, SV_SET_OF_GAUNTLETS },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD },
		{ TV_SHIELD, SV_LARGE_LEATHER_SHIELD },
		{ TV_SHIELD, SV_SMALL_METAL_SHIELD },

		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_BOOTS, SV_PAIR_OF_HARD_LEATHER_BOOTS },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_HARD_LEATHER_CAP },

		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR },

		{ TV_SOFT_ARMOR, SV_LEATHER_JACK },
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },

		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_GAUNTLETS },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD },
		{ TV_SHIELD, SV_SMALL_LEATHER_SHIELD }
	},

	{
		/* Weaponsmith */

		{ TV_SWORD, SV_DAGGER },
		{ TV_SWORD, SV_MAIN_GAUCHE },
		{ TV_SWORD, SV_RAPIER },
		{ TV_SWORD, SV_SMALL_SWORD },

		{ TV_SWORD, SV_SHORT_SWORD },
		{ TV_SWORD, SV_SABRE },
		{ TV_SWORD, SV_CUTLASS },
		{ TV_SWORD, SV_TULWAR },

		{ TV_SWORD, SV_BROAD_SWORD },
		{ TV_SWORD, SV_LONG_SWORD },
		{ TV_SWORD, SV_SCIMITAR },
		{ TV_SWORD, SV_KATANA },

		{ TV_SWORD, SV_BASTARD_SWORD },
		{ TV_POLEARM, SV_SPEAR },
		{ TV_POLEARM, SV_AWL_PIKE },
		{ TV_POLEARM, SV_TRIDENT },

		{ TV_POLEARM, SV_PIKE },
		{ TV_POLEARM, SV_BEAKED_AXE },
		{ TV_POLEARM, SV_BROAD_AXE },
		{ TV_POLEARM, SV_LANCE },

		{ TV_POLEARM, SV_BATTLE_AXE },
		{ TV_POLEARM, SV_HATCHET },
		{ TV_BOW, SV_SLING },
		{ TV_BOW, SV_SHORT_BOW },

		{ TV_BOW, SV_LONG_BOW },
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_SHOT, SV_AMMO_NORMAL },

		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },

		{ TV_BOW, SV_LONG_BOW },
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_ARROW, SV_AMMO_NORMAL },
		{ TV_ARROW, SV_AMMO_NORMAL },

		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_BOLT, SV_AMMO_NORMAL },
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SWORD, SV_DAGGER },

		{ TV_SWORD, SV_TANTO },
		{ TV_SWORD, SV_RAPIER },
		{ TV_SWORD, SV_SMALL_SWORD },
		{ TV_SWORD, SV_SHORT_SWORD },

		{ TV_HAFTED, SV_WHIP },
		{ TV_SWORD, SV_BROAD_SWORD },
		{ TV_SWORD, SV_LONG_SWORD },
		{ TV_SWORD, SV_SCIMITAR }
	},

	{
		/* Temple */

		{ TV_HAFTED, SV_NUNCHAKU },
		{ TV_HAFTED, SV_QUARTERSTAFF },
		{ TV_HAFTED, SV_MACE },
		{ TV_HAFTED, SV_BO_STAFF },

		{ TV_HAFTED, SV_WAR_HAMMER },
		{ TV_HAFTED, SV_LUCERN_HAMMER },
		{ TV_HAFTED, SV_MORNING_STAR },
		{ TV_HAFTED, SV_FLAIL },

		{ TV_HAFTED, SV_LEAD_FILLED_MACE },
		{ TV_SCROLL, SV_SCROLL_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_BLESSING },
		{ TV_SCROLL, SV_SCROLL_HOLY_CHANT },

		{ TV_POTION, SV_POTION_HEROISM },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },

		{ TV_POTION, SV_POTION_CURE_LIGHT },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_SERIOUS },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },

		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },

                { TV_VALARIN_BOOK, 0 },
                { TV_VALARIN_BOOK, 0 },
                { TV_VALARIN_BOOK, 1 },
                { TV_VALARIN_BOOK, 1 },

                { TV_VALARIN_BOOK, 2 },
                { TV_VALARIN_BOOK, 2 },
                { TV_VALARIN_BOOK, 3 },
                { TV_VALARIN_BOOK, 3 },

		{ TV_HAFTED, SV_WHIP },
		{ TV_HAFTED, SV_MACE },
		{ TV_HAFTED, SV_BALL_AND_CHAIN },
		{ TV_HAFTED, SV_WAR_HAMMER },

		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_POTION, SV_POTION_CURE_CRITICAL },

		{ TV_POTION, SV_POTION_CURE_CRITICAL },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },
		{ TV_POTION, SV_POTION_RESTORE_EXP },

		{ TV_SCROLL, SV_SCROLL_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_REMOVE_CURSE },
		{ TV_SCROLL, SV_SCROLL_STAR_REMOVE_CURSE },
                { TV_SCROLL, SV_SCROLL_STAR_REMOVE_CURSE },

		{ TV_PRAYER_BOOK, 0 },
		{ TV_PRAYER_BOOK, 0 },
		{ TV_PRAYER_BOOK, 0 },
		{ TV_PRAYER_BOOK, 1 },

		{ TV_PRAYER_BOOK, 1 },
		{ TV_PRAYER_BOOK, 2 },
		{ TV_PRAYER_BOOK, 2 },
                { TV_PRAYER_BOOK, 3 },
        },

	{
		/* Alchemy shop */

		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_HIT },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_WEAPON_TO_DAM },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },

		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },

		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_PHASE_DOOR },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_SCROLL, SV_SCROLL_MONSTER_CONFUSION },

		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_SCROLL, SV_SCROLL_DETECT_GOLD },
		{ TV_SCROLL, SV_SCROLL_DETECT_ITEM },
		{ TV_SCROLL, SV_SCROLL_DETECT_TRAP },

		{ TV_SCROLL, SV_SCROLL_DETECT_INVIS },
		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_SATISFY_HUNGER },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },

		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_WORD_OF_RECALL },
		{ TV_SCROLL, SV_SCROLL_TELEPORT },

		{ TV_SCROLL, SV_SCROLL_TELEPORT },
		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_WIS },

		{ TV_POTION, SV_POTION_RES_DEX },
		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_SCROLL, SV_SCROLL_IDENTIFY },

		{ TV_SCROLL, SV_SCROLL_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_STAR_IDENTIFY },  /* Yep, occasionally! */
		{ TV_SCROLL, SV_SCROLL_STAR_IDENTIFY },
		{ TV_SCROLL, SV_SCROLL_LIGHT },

		{ TV_POTION, SV_POTION_RES_STR },
		{ TV_POTION, SV_POTION_RES_INT },
		{ TV_POTION, SV_POTION_RES_WIS },
		{ TV_POTION, SV_POTION_RES_DEX },

		{ TV_POTION, SV_POTION_RES_CON },
		{ TV_POTION, SV_POTION_RES_CHR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },
		{ TV_SCROLL, SV_SCROLL_ENCHANT_ARMOR },

		{ TV_SCROLL, SV_SCROLL_RECHARGING },
		{ TV_SCROLL, SV_SCROLL_SATISFY_HUNGER },
		{ TV_SCROLL, SV_SCROLL_SATISFY_HUNGER },
		{ TV_SCROLL, SV_SCROLL_SATISFY_HUNGER }

	},

	{
		/* Magic-User store */

		{ TV_RING, SV_RING_PROTECTION },
		{ TV_RING, SV_RING_FEATHER_FALL },
		{ TV_RING, SV_RING_PROTECTION },
		{ TV_RING, SV_RING_RESIST_FIRE },

		{ TV_RING, SV_RING_RESIST_COLD },
		{ TV_AMULET, SV_AMULET_CHARISMA },
		{ TV_AMULET, SV_AMULET_SLOW_DIGEST },
		{ TV_AMULET, SV_AMULET_RESIST_ACID },

		{ TV_AMULET, SV_AMULET_SEARCHING },
		{ TV_WAND, SV_WAND_SLOW_MONSTER },
		{ TV_WAND, SV_WAND_CONFUSE_MONSTER },
		{ TV_WAND, SV_WAND_SLEEP_MONSTER },

		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_WAND, SV_WAND_STINKING_CLOUD },
		{ TV_WAND, SV_WAND_WONDER },
		{ TV_WAND, SV_WAND_DISARMING },

		{ TV_STAFF, SV_STAFF_LITE },
		{ TV_STAFF, SV_STAFF_MAPPING },
		{ TV_STAFF, SV_STAFF_DETECT_TRAP },
		{ TV_STAFF, SV_STAFF_DETECT_DOOR },

		{ TV_STAFF, SV_STAFF_DETECT_GOLD },
		{ TV_STAFF, SV_STAFF_DETECT_ITEM },
		{ TV_STAFF, SV_STAFF_DETECT_INVIS },
		{ TV_STAFF, SV_STAFF_DETECT_EVIL },

		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },
		{ TV_STAFF, SV_STAFF_TELEPORTATION },

		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_IDENTIFY },

		{ TV_STAFF, SV_STAFF_IDENTIFY },
		{ TV_STAFF, SV_STAFF_REMOVE_CURSE },
		{ TV_STAFF, SV_STAFF_CURE_LIGHT },
		{ TV_STAFF, SV_STAFF_PROBING },

                { TV_MAGERY_BOOK, 0 },
                { TV_MAGERY_BOOK, 1 },
                { TV_MAGERY_BOOK, 2 },
                { TV_MAGERY_BOOK, 3 },

		{ TV_MAGIC_BOOK, 0 },
                { TV_MAGIC_BOOK, 1 },
                { TV_MAGIC_BOOK, 2 },
                { TV_MAGIC_BOOK, 3 },

                { TV_ILLUSION_BOOK, 0 },
                { TV_ILLUSION_BOOK, 1 },
                { TV_ILLUSION_BOOK, 2 },
                { TV_ILLUSION_BOOK, 3 }
	},

	{
		/* Black Market (unused) */
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
	},

	{
		/* Home (unused) */
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
	},

	{
		/* Bookstore */
                { TV_MAGERY_BOOK, 0 },
                { TV_MAGERY_BOOK, 1 },
                { TV_MAGERY_BOOK, 2 },
                { TV_MAGERY_BOOK, 3 },

                { TV_SHADOW_BOOK, 0 },
                { TV_SHADOW_BOOK, 1 },
                { TV_SHADOW_BOOK, 2 },
                { TV_SHADOW_BOOK, 3 },

		{ TV_CHAOS_BOOK, 0 },
                { TV_CHAOS_BOOK, 1 },

                { TV_TRIBAL_BOOK, 0 },
                { TV_TRIBAL_BOOK, 1 },

                { TV_NETHER_BOOK, 0 },
                { TV_NETHER_BOOK, 1 },
                { TV_NETHER_BOOK, 2 },
                { TV_NETHER_BOOK, 3 },

                { TV_CRUSADE_BOOK, 0 },           /* +16 */
                { TV_CRUSADE_BOOK, 0 },
                { TV_CRUSADE_BOOK, 1 },
                { TV_CRUSADE_BOOK, 1 },

                { TV_SIGALDRY_BOOK, 0 },
                { TV_SIGALDRY_BOOK, 0 },
                { TV_SIGALDRY_BOOK, 1 },
                { TV_SIGALDRY_BOOK, 1 },

                { TV_ILLUSION_BOOK, 0 },
                { TV_ILLUSION_BOOK, 1 },
                { TV_ILLUSION_BOOK, 2 },
                { TV_ILLUSION_BOOK, 3 },

                { TV_VALARIN_BOOK, 0 },
                { TV_VALARIN_BOOK, 1 },
                { TV_VALARIN_BOOK, 2 },
                { TV_VALARIN_BOOK, 3 },

                { TV_MUSIC_BOOK, 0 },
                { TV_MUSIC_BOOK, 0 },
                { TV_MUSIC_BOOK, 1 },
                { TV_MUSIC_BOOK, 1 },

                { TV_SYMBIOTIC_BOOK, 0 },
                { TV_SYMBIOTIC_BOOK, 0 },
        },

	{
                /* Pet Shop */
                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },

                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },

		{ TV_FOOD, SV_FOOD_BISCUIT },
		{ TV_FOOD, SV_FOOD_BISCUIT },
		{ TV_FOOD, SV_FOOD_BISCUIT },
		{ TV_FOOD, SV_FOOD_BISCUIT },

                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },

                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },
                { TV_EGG, 1 },

		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },

		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },

		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
	},
};


/*
 * Initialize misc. values
 */
static errr init_misc(void)
{
	int xstart = 0;
	int ystart = 0;

	/* Initialize the values */
	process_dungeon_file("misc.txt", &ystart, &xstart, 0, 0);

	return 0;
}


/*
 * Initialize town array
 */
static errr init_towns(void)
{
	int i, j, k;
	
	/*** Prepare the Towns ***/

	/* Allocate the towns */
	C_MAKE(town, max_towns, town_type);

	for (i = 1; i < max_towns; i++)
	{
		/*** Prepare the Stores ***/

		/* Allocate the stores */
		C_MAKE(town[i].store, MAX_STORES, store_type);

		/* Fill in each store */
		for (j = 0; j < MAX_STORES; j++)
		{
			/* Access the store */
			store_type *st_ptr = &town[i].store[j];

			/* Assume full stock */
			st_ptr->stock_size = STORE_INVEN_MAX;

			/* Allocate the stock */
			C_MAKE(st_ptr->stock, st_ptr->stock_size, object_type);

			/* No table for the black market or home */
			if ((j == STORE_BLACK) || (j == STORE_HOME)) continue;

			/* Assume full table */
			st_ptr->table_size = STORE_CHOICES;

			/* Allocate the stock */
			C_MAKE(st_ptr->table, st_ptr->table_size, s16b);

			/* Scan the choices */
			for (k = 0; k < STORE_CHOICES; k++)
			{
				int k_idx;

				/* Extract the tval/sval codes */
				int tv = store_table[j][k][0];
				int sv = store_table[j][k][1];

				/* Look for it */
				for (k_idx = 1; k_idx < max_k_idx; k_idx++)
				{
					object_kind *k_ptr = &k_info[k_idx];

					/* Found a match */
					if ((k_ptr->tval == tv) && (k_ptr->sval == sv)) break;
				}

				/* Catch errors */
				if (k_idx == max_k_idx) continue;

				/* Add that item index to the table */
				st_ptr->table[st_ptr->table_num++] = k_idx;
			}
		}
	}

	return 0;
}

/*
 * Initialize buildings
 */
errr init_buildings(void)
{
	int i, j;

	for (i = 0; i < MAX_BLDG; i++)
	{
		building[i].name[0] = '\0';
		building[i].owner_name[0] = '\0';
		building[i].owner_race[0] = '\0';
		
		for (j = 0; j < 6; j++)
		{
			building[i].act_names[j][0] = '\0';
			building[i].member_costs[j] = 0;
			building[i].other_costs[j] = 0;
			building[i].letters[j] = 0;
			building[i].actions[j] = 0;
			building[i].action_restr[j] = 0;
		}

		for (j = 0; j < MAX_CLASS; j++)
		{
			building[i].member_class[j] = 0;
		}

		for (j = 0; j < MAX_RACES; j++)
		{
			building[i].member_race[j] = 0;
		}

		for (j = 0; j < MAX_REALM+1; j++)
		{
			building[i].member_realm[j] = 0;
		}
	}

	return (0);
}


/*
 * Initialize quest array
 */
static errr init_quests(void)
{
	int i;
	
	/*** Prepare the quests ***/

	/* Allocate the quests */
	C_MAKE(quest, max_quests, quest_type);

	/* Set all quest to "untaken" */
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = 0;
	}

	return 0;
}


/*
 * Pointer to wilderness_type
 */
typedef wilderness_type *wilderness_type_ptr;

/*
 * Initialize wilderness array
 */
static errr init_wilderness(void)
{
	int i;

	/* Allocate the wilderness (two-dimension array) */
	C_MAKE(wilderness, max_wild_y, wilderness_type_ptr);
	C_MAKE(wilderness[0], max_wild_x * max_wild_y, wilderness_type);

	/* Init the other pointers */
	for (i = 1; i < max_wild_y; i++)
		wilderness[i] = wilderness[0] + i * max_wild_x;

	return 0;
}

/*
 * Initialize some other arrays
 */
static errr init_other(void)
{
	int i, n;


	/*** Prepare the "dungeon" information ***/

	/* Allocate and Wipe the object list */
	C_MAKE(o_list, max_o_idx, object_type);

	/* Allocate and Wipe the monster list */
	C_MAKE(m_list, max_m_idx, monster_type);

        /* Allocate and Wipe the max dungeon level */
        C_MAKE(p_ptr->max_dlv, max_d_idx, s16b);

        for (i = 0; i < MAX_DUNGEON_DEPTH; i++)
	{
                /* Allocate and Wipe the special level history */
                C_MAKE(spec_history[i], max_d_idx, byte);
        }

	/* Allocate and wipe each line of the cave */
	for (i = 0; i < MAX_HGT; i++)
	{
		/* Allocate one row of the cave */
		C_MAKE(cave[i], MAX_WID, cave_type);
	}


	/*** Prepare the various "bizarre" arrays ***/

	/* Macro variables */
	C_MAKE(macro__pat, MACRO_MAX, cptr);
	C_MAKE(macro__act, MACRO_MAX, cptr);
	C_MAKE(macro__cmd, MACRO_MAX, bool);

	/* Macro action buffer */
	C_MAKE(macro__buf, 1024, char);

	/* Quark variables */
	C_MAKE(quark__str, QUARK_MAX, cptr);

	/* Message variables */
	C_MAKE(message__ptr, MESSAGE_MAX, u16b);
	C_MAKE(message__buf, MESSAGE_BUF, char);

	/* Hack -- No messages yet */
	message__tail = MESSAGE_BUF;


	/*** Prepare the Player inventory ***/

	/* Allocate it */
	C_MAKE(inventory, INVEN_TOTAL, object_type);


	/*** Pre-allocate the basic "auto-inscriptions" ***/

	/* The "basic" feelings */
	(void)quark_add("cursed");
	(void)quark_add("broken");
	(void)quark_add("average");
	(void)quark_add("good");

	/* The "extra" feelings */
	(void)quark_add("excellent");
	(void)quark_add("worthless");
	(void)quark_add("special");
	(void)quark_add("terrible");

	/* Some extra strings */
	(void)quark_add("uncursed");
	(void)quark_add("on sale");


	/*** Prepare the options ***/

	/* Scan the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Accept */
			option_mask[os] |= (1L << ob);
			
			/* Set */
			if (option_info[i].o_norm)
			{
				/* Set */
				option_flag[os] |= (1L << ob);
			}
			
			/* Clear */
			else
			{
				/* Clear */
				option_flag[os] &= ~(1L << ob);
			}
		}
	}

	/* Analyze the windows */
	for (n = 0; n < 8; n++)
	{
		/* Analyze the options */
		for (i = 0; i < 32; i++)
		{
			/* Accept */
			if (window_flag_desc[i])
			{
				/* Accept */
				window_mask[n] |= (1L << i);
			}
		}
	}


	/*** Pre-allocate space for the "format()" buffer ***/

	/* Hack -- Just call the "format()" function */
        (void)format("%s (%s).", "Dark God <dark.god@infonie.fr>", MAINTAINER);

	/* Success */
	return (0);
}



/*
 * Initialize some other arrays
 */
static errr init_alloc(void)
{
	int i, j;

	object_kind *k_ptr;

	monster_race *r_ptr;

	alloc_entry *table;

	s16b num[MAX_DEPTH];

	s16b aux[MAX_DEPTH];


	/*** Analyze object allocation info ***/

	/* Clear the "aux" array */
	C_WIPE(&aux, MAX_DEPTH, s16b);

	/* Clear the "num" array */
	C_WIPE(&num, MAX_DEPTH, s16b);

	/* Size of "alloc_kind_table" */
	alloc_kind_size = 0;

	/* Scan the objects */
	for (i = 1; i < max_k_idx; i++)
	{
		k_ptr = &k_info[i];

		/* Scan allocation pairs */
		for (j = 0; j < 4; j++)
		{
			/* Count the "legal" entries */
			if (k_ptr->chance[j])
			{
				/* Count the entries */
				alloc_kind_size++;

				/* Group by level */
				num[k_ptr->locale[j]]++;
			}
		}
	}

	/* Collect the level indexes */
	for (i = 1; i < MAX_DEPTH; i++)
	{
		/* Group by level */
		num[i] += num[i-1];
	}

	/* Paranoia */
	if (!num[0]) quit("No town objects!");


	/*** Initialize object allocation info ***/

	/* Allocate the alloc_kind_table */
	C_MAKE(alloc_kind_table, alloc_kind_size, alloc_entry);

	/* Access the table entry */
	table = alloc_kind_table;

	/* Scan the objects */
	for (i = 1; i < max_k_idx; i++)
	{
		k_ptr = &k_info[i];

		/* Scan allocation pairs */
		for (j = 0; j < 4; j++)
		{
			/* Count the "legal" entries */
			if (k_ptr->chance[j])
			{
				int p, x, y, z;

				/* Extract the base level */
				x = k_ptr->locale[j];

				/* Extract the base probability */
				p = (100 / k_ptr->chance[j]);

				/* Skip entries preceding our locale */
				y = (x > 0) ? num[x-1] : 0;

				/* Skip previous entries at this locale */
				z = y + aux[x];

				/* Load the entry */
				table[z].index = i;
				table[z].level = x;
				table[z].prob1 = p;
				table[z].prob2 = p;
				table[z].prob3 = p;

				/* Another entry complete for this locale */
				aux[x]++;
			}
		}
	}


	/*** Analyze monster allocation info ***/

	/* Clear the "aux" array */
	C_WIPE(&aux, MAX_DEPTH, s16b);

	/* Clear the "num" array */
	C_WIPE(&num, MAX_DEPTH, s16b);

	/* Size of "alloc_race_table" */
	alloc_race_size = 0;

	/* Scan the monsters (not the ghost) */
	for (i = 1; i < max_r_idx - 1; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[i];

		/* Legal monsters */
		if (r_ptr->rarity)
		{
			/* Count the entries */
			alloc_race_size++;

			/* Group by level */
			num[r_ptr->level]++;
		}
	}

	/* Collect the level indexes */
	for (i = 1; i < MAX_DEPTH; i++)
	{
		/* Group by level */
		num[i] += num[i-1];
	}

	/* Paranoia */
	if (!num[0]) quit("No town monsters!");


	/*** Initialize monster allocation info ***/

	/* Allocate the alloc_race_table */
	C_MAKE(alloc_race_table, alloc_race_size, alloc_entry);

	/* Access the table entry */
	table = alloc_race_table;

	/* Scan the monsters (not the ghost) */
	for (i = 1; i < max_r_idx - 1; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[i];

		/* Count valid pairs */
		if (r_ptr->rarity)
		{
			int p, x, y, z;

			/* Extract the base level */
			x = r_ptr->level;

			/* Extract the base probability */
			p = (100 / r_ptr->rarity);

			/* Skip entries preceding our locale */
			y = (x > 0) ? num[x-1] : 0;

			/* Skip previous entries at this locale */
			z = y + aux[x];

			/* Load the entry */
			table[z].index = i;
			table[z].level = x;
			table[z].prob1 = p;
			table[z].prob2 = p;
			table[z].prob3 = p;

			/* Another entry complete for this locale */
			aux[x]++;
		}
	}


	/* Success */
	return (0);
}


/*
 * Hack -- Explain a broken "lib" folder and quit (see below).
 *
 * XXX XXX XXX This function is "messy" because various things
 * may or may not be initialized, but the "plog()" and "quit()"
 * functions are "supposed" to work under any conditions.
 */
static void init_angband_aux(cptr why)
{
	/* Why */
	plog(why);

	/* Explain */
	plog("The 'lib' directory is probably missing or broken.");

	/* More details */
	plog("Perhaps the archive was not extracted correctly.");

	/* Explain */
	plog("See the 'README' file for more information.");

	/* Quit with error */
	quit("Fatal Error.");
}

#ifdef USE_PYTHON
/*
 * Initialize the Python interpreter that we'll be using, and also insert
 * some modules into it that we provide, so that Python code can call
 * on Angband functions.
 */
static errr init_python(void)
{
        char buf[1024], path[128];

        Py_SetProgramName((char*)argv0);

#if __djgpp__
        setenv("PYTHONPATH", ANGBAND_DIR_SCPT, 1);
        setenv("PYTHONHOME", ANGBAND_DIR_SCPT, 1);
#endif /* __djgpp__ */

	/* Initialize the Python interpreter */
	Py_Initialize();

        /* Add the "script" directory to the module search path */
        ascii_to_text(path, ANGBAND_DIR_SCPT);
        sprintf(buf, "import sys; sys.path.insert(0, '%s')", path);
	PyRun_SimpleString(buf);

        /* Add the PernAngband modules */
        initio();
        initcave();
        initmisc();
        initkind();
        initevent();
        initobject();
        initmonster();
        initplayer();
        initspell();
        initquest();
        initrace();
        initdungeon();

	/* Import initial module */
	sprintf(buf, "import init");
	PyRun_SimpleString(buf);

	/* Success */
	return (0);
}
#endif

/*
 * Hack -- main Angband initialization entry point
 *
 * Verify some files, display the "news.txt" file, create
 * the high score file, initialize all internal arrays, and
 * load the basic "user pref files".
 *
 * Be very careful to keep track of the order in which things
 * are initialized, in particular, the only thing *known* to
 * be available when this function is called is the "z-term.c"
 * package, and that may not be fully initialized until the
 * end of this function, when the default "user pref files"
 * are loaded and "Term_xtra(TERM_XTRA_REACT,0)" is called.
 *
 * Note that this function attempts to verify the "news" file,
 * and the game aborts (cleanly) on failure, since without the
 * "news" file, it is likely that the "lib" folder has not been
 * correctly located.  Otherwise, the news file is displayed for
 * the user.
 *
 * Note that this function attempts to verify (or create) the
 * "high score" file, and the game aborts (cleanly) on failure,
 * since one of the most common "extraction" failures involves
 * failing to extract all sub-directories (even empty ones), such
 * as by failing to use the "-d" option of "pkunzip", or failing
 * to use the "save empty directories" option with "Compact Pro".
 * This error will often be caught by the "high score" creation
 * code below, since the "lib/apex" directory, being empty in the
 * standard distributions, is most likely to be "lost", making it
 * impossible to create the high score file.
 *
 * Note that various things are initialized by this function,
 * including everything that was once done by "init_some_arrays".
 *
 * This initialization involves the parsing of special files
 * in the "lib/data" and sometimes the "lib/edit" directories.
 *
 * Note that the "template" files are initialized first, since they
 * often contain errors.  This means that macros and message recall
 * and things like that are not available until after they are done.
 *
 * We load the default "user pref files" here in case any "color"
 * changes are needed before character creation.
 *
 * Note that the "graf-xxx.prf" file must be loaded separately,
 * if needed, in the first (?) pass through "TERM_XTRA_REACT".
 */
void init_angband(void)
{
	int fd = -1;

	int mode = 0644;

	FILE *fp;

	char buf[1024];


	/*** Verify the "news" file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "news.txt");

	/* Attempt to open the file */
	fd = fd_open(buf, O_RDONLY);

	/* Failure */
	if (fd < 0)
	{
		char why[1024];

		/* Message */
		sprintf(why, "Cannot access the '%s' file!", buf);

		/* Crash and burn */
		init_angband_aux(why);
	}

	/* Close it */
	(void)fd_close(fd);


	/*** Display the "news" file ***/

	/* Clear screen */
	Term_clear();

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "news.txt");

	/* Open the News file */
	fp = my_fopen(buf, "r");

	/* Dump */
	if (fp)
	{
		int i = 0;

		/* Dump the file to the screen */
		while (0 == my_fgets(fp, buf, 1024))
		{
			/* Display and advance */
			Term_putstr(0, i++, -1, TERM_WHITE, buf);
		}

		/* Close */
		my_fclose(fp);
	}

	/* Flush it */
	Term_fresh();


	/*** Verify (or create) the "high score" file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_APEX, "scores.raw");

	/* Attempt to open the high score file */
	fd = fd_open(buf, O_RDONLY);

	/* Failure */
	if (fd < 0)
	{
		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Create a new high score file */
		fd = fd_make(buf, mode);

		/* Failure */
		if (fd < 0)
		{
			char why[1024];

			/* Message */
			sprintf(why, "Cannot create the '%s' file!", buf);

			/* Crash and burn */
			init_angband_aux(why);
		}
	}

	/* Close it */
	(void)fd_close(fd);


	/*** Initialize some arrays ***/

	/* Initialize misc. values */
	note("[Initializing values... (misc)]");
	if (init_misc()) quit("Cannot initialize misc. values");

#ifdef USE_PYTHON
        note("[Initializing Python scripts... ]");
        if (init_python()) quit("Cannot initialize Python scripts");
#endif

	/* Initialize feature info */
	note("[Initializing arrays... (features)]");
	if (init_f_info()) quit("Cannot initialize features");

	/* Initialize object info */
	note("[Initializing arrays... (objects)]");
	if (init_k_info()) quit("Cannot initialize objects");

	/* Initialize artifact info */
	note("[Initializing arrays... (artifacts)]");
	if (init_a_info()) quit("Cannot initialize artifacts");

	/* Initialize ego-item info */
	note("[Initializing arrays... (ego-items)]");
	if (init_e_info()) quit("Cannot initialize ego-items");

	/* Initialize monster info */
	note("[Initializing arrays... (monsters)]");
	if (init_r_info()) quit("Cannot initialize monsters");

        /* Initialize dungeon type info */
        note("[Initializing arrays... (dungeon types)]");
        if (init_d_info()) quit("Cannot initialize dungeon types");

	/* Initialize town array */
	note("[Initializing arrays... (towns)]");
	if (init_towns()) quit("Cannot initialize towns");

	/* Initialize wilderness array */
	note("[Initializing arrays... (wilderness)]");
	if (init_wilderness()) quit("Cannot initialize wilderness");

	/* Initialize building array */
	note("[Initializing arrays... (buildings)]");
	if (init_buildings()) quit("Cannot initialize buildings");

	/* Initialize quest array */
	note("[Initializing arrays... (quests)]");
	if (init_quests()) quit("Cannot initialize quests");

	/* Initialize some other arrays */
	note("[Initializing arrays... (other)]");
	if (init_other()) quit("Cannot initialize other stuff");

	/* Initialize some other arrays */
	note("[Initializing arrays... (alloc)]");
	if (init_alloc()) quit("Cannot initialize alloc stuff");


	/*** Load default user pref files ***/

	/* Initialize feature info */
	note("[Initializing user pref files...]");

	/* Access the "basic" pref file */
	strcpy(buf, "pref.prf");

	/* Process that file */
	process_pref_file(buf);

	/* Access the "user" pref file */
	sprintf(buf, "user.prf");

	/* Process that file */
	process_pref_file(buf);

	/* Access the "basic" system pref file */
	sprintf(buf, "pref-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "user" system pref file */
	sprintf(buf, "user-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/* Done */
	note("[Initialization complete]");
}



