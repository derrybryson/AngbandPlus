

#include "angband.h"
#include "langband.h"


#if defined(USE_GTK)
errr init_gtk(int argc, char **argv);
#endif

#if defined(USE_X11)
errr init_x11(int argc, char **argv);
#endif

#if defined(USE_GCU)
errr init_gcu(int argc, char **argv);
#endif

/** defaults to true as cmucl is default */
int lisp_will_use_callback = 1;

/** set default illegal value. */
LISP_SYSTEMS current_lisp_system = LISPSYS_BAD;

/** the base path for config files */
const char *base_config_dir = "./";

/**
 * Hack -- take notes on line 23
 */
static void note(cptr str)
{
    Term_erase(0, 23, 255);
    Term_putstr(20, 23, -1, TERM_WHITE, str);
    Term_fresh();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 * XXX XXX Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(void)
{
    /* If nothing important has happened, just quit */
    if (!character_generated || character_saved) quit("panic");

	/* Mega-Hack -- see "msg_print()" */
    msg_flag = FALSE;

    /* Clear the top line */
    prt("", 0, 0);

    /* Hack -- turn off some things */
    disturb(1, 0);

    /* Hack -- Delay death XXX XXX XXX */
    if (p_ptr->chp < 0) p_ptr->is_dead = FALSE;

    /* Hardcode panic save */
    p_ptr->panic_save = 1;

    /* Forbid suspend */
    signals_ignore_tstp();

    /* Indicate panic save */
    strcpy(p_ptr->died_from, "(panic save)");

    /* Panic save, or get worried */
    if (!save_player()) quit("panic save failed!");

    /* Successful panic save */
    quit("panic save succeeded!");
}



/*
 * Display a string on the screen using an attribute, and clear
 * to the end of the line.
 */
void c_prt(byte attr, cptr str, int row, int col)
{
    /* Clear line, position cursor */
    Term_erase(col, row, 255);

	/* Dump the attr/text */
    Term_addstr(-1, attr, str);
}


/*
 * As above, but in "white"
 */
void prt(cptr str, int row, int col)
{
    /* Spawn */
    c_prt(TERM_WHITE, str, row, col);
}


void
disturb(int stop_search, int unused_flag) {

    (void)(stop_search + unused_flag); // to avoid warning
    note("disturb");
}

bool
save_player(void) {

    note("saving on C-side");
    return 0;
}


int which_ui_used = -1;

int current_ui() { return which_ui_used; }


errr
init_c_side(const char *ui, const char *basePath, int debugging_level) {

    int possible_to_go_X = 0;
    int default_mode = 0; // 0 is x, 1 is gcu, 2 is gtk.. hackish
    int wanted_ui = 0;
        
    // leak
    int argc = 1;
    char **argv = malloc(100);
    argv[0] = "langband";
    argv[1] = NULL;

    if (!ui) {
	ui = "X11";
    }

    if (basePath && (strlen(basePath)>0)) {
	base_config_dir = basePath;
    }
    

    /* verify that we have decent value */
    if (!strcmp(ui, "DEFAULT") ||
	!strcmp(ui, "default")) {
	wanted_ui = default_mode;
    }
    else if (!strcmp(ui, "X11") ||
	!strcmp(ui, "x11") ||
	!strcmp(ui, "X")) {
	wanted_ui = 0;
    }
    else if (!strcmp(ui, "gcu") ||
	  !strcmp(ui, "curses") ||
	  !strcmp(ui, "GCU")) {
	wanted_ui = 1;
    }
    else if (!strcmp(ui, "gtk") ||
	  !strcmp(ui, "gtk+") ||
	  !strcmp(ui, "GTK")) {
	wanted_ui = 2;
    }
    else {
	fprintf(stderr, "Unable to find compatible UI with spec '%s'\n", ui);
	return -1;
    }

    if (wanted_ui >= 0 && wanted_ui <= 2) {

    }
    else {
	fprintf(stderr, "The UI-value is set to an illegal value: %d\n", wanted_ui);
	return -1;
    }

    
#ifdef USE_SOUND
    // hackish
    use_sound = 1;
    sound_init();
#endif


    /* let us check if we can go X */
    
#if defined(USE_GTK) || defined(USE_X11)    
    {
	char *val = getenv("DISPLAY");
	if (val && strlen(val)) {
	    possible_to_go_X = 1;
	}
    }
#endif
    

    if (1==0) { }
    
#if defined(USE_X11)
    else if (possible_to_go_X && wanted_ui == 0) {
	which_ui_used = 0;
	init_x11(0,NULL); /* proper value set */
    }
#endif
    
#if defined(USE_GTK)
    else if (possible_to_go_X && wanted_ui == 2) {
	which_ui_used = 2;
	init_gtk(argc,argv); /* proper value set */
    }
#endif
    
#if defined (USE_GCU)
    else if (wanted_ui == 1 || !possible_to_go_X) {
	which_ui_used = 1;
	init_gcu(argc,argv);
    }
#endif

    else {
	if (!possible_to_go_X && (wanted_ui == 0 || wanted_ui == 2)) {
	    fprintf(stderr, "Wanted an X-dependent UI, but unable to find X (no DISPLAY env).\n");
	}
	else {
	    fprintf(stderr,"Unable to find a suitable UI to use [%s,%d].\n", ui, wanted_ui);
	}
	return -1;
    }
	
#if defined(USE_X11) || defined(USE_GCU)
    /* Initialize */
    init_angband();
    pause_line(23);
    play_game(TRUE);
#endif

    return 0;
}


void
init_angband(void) {

//       int fd = -1;

//        int mode = 0644;

    FILE *fp;

    char buf[1024];
    char fname[1024];
 
    /* Open the News file */
    sprintf(fname, "%snews.txt", base_config_dir);
    /*printf("Trying to open |%s|\n", fname);*/
    fp = my_fopen(fname, "r");

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

    macro_init();

#ifdef USE_X11
//	process_pref_file("./lib/file/user.prf");
//	puts("prof..");
#endif
	
    note("[Initialization complete]");

}


/*
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
char hexsym[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


void window_stuff(void) {

    printf("window stuff\n");

}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
    /* Disable handler */
    (void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

    /* Flush output */
    Term_fresh();

    /* Suspend the "Term" */
    Term_xtra(TERM_XTRA_ALIVE, 0);

    /* Suspend ourself */
    (void)kill(0, SIGSTOP);

    /* Resume the "Term" */
    Term_xtra(TERM_XTRA_ALIVE, 1);

    /* Redraw the term */
    Term_redraw();

    /* Flush the term */
    Term_fresh();

#endif

    /* Restore handler */
    (void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
    /* Disable handler */
    (void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
    if (!character_generated || character_saved) quit(NULL);


    /* Count the signals */
    signal_count++;


    /* Terminate dead characters */
    if (p_ptr->is_dead)
    {
	/* Mark the savefile */
	strcpy(p_ptr->died_from, "Abortion");

		/* Close stuff */
	close_game();

	/* Quit */
	quit("interrupt");
    }

    /* Allow suicide (after 5) */
    else if (signal_count >= 5)
    {
	/* Cause of "death" */
	strcpy(p_ptr->died_from, "Interrupting");

		/* Commit suicide */
	p_ptr->is_dead = TRUE;

	/* Stop playing */
	p_ptr->playing = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;

	/* Close stuff */
	close_game();

	/* Quit */
	quit("interrupt");
    }

    /* Give warning (after 4) */
    else if (signal_count >= 4)
    {
	/* Make a noise */
	Term_xtra(TERM_XTRA_NOISE, 0);

		/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Display the cause */
	Term_putstr(0, 0, -1, TERM_WHITE, "Contemplating suicide!");

	/* Flush */
	Term_fresh();
    }

    /* Give warning (after 2) */
    else if (signal_count >= 2)
    {
	/* Make a noise */
	Term_xtra(TERM_XTRA_NOISE, 0);
    }

    /* Restore handler */
    (void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void
handle_signal_abort(int sig) {

    /* Disable handler */
    (void)signal(sig, SIG_IGN);


    /* Nothing to save, just quit */
    if (!character_generated || character_saved) quit(NULL);


    /* Clear the bottom line */
    Term_erase(0, 23, 255);

    /* Give a warning */
    Term_putstr(0, 23, -1, TERM_RED,
		"A gruesome software bug LEAPS out at you!");

    /* Message */
    Term_putstr(45, 23, -1, TERM_RED, "Panic save...");

    /* Flush output */
    Term_fresh();

    /* Panic Save */
    p_ptr->panic_save = 1;

    /* Panic save */
    strcpy(p_ptr->died_from, "(panic save)");

    /* Forbid suspend */
    signals_ignore_tstp();

    /* Attempt to save */
    if (save_player())
    {
	Term_putstr(45, 23, -1, TERM_RED, "Panic save succeeded!");
    }

    /* Save failed */
    else
    {
	Term_putstr(45, 23, -1, TERM_RED, "Panic save failed!");
    }

    /* Flush output */
    Term_fresh();

    /* Quit */
    quit("software bug");
}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{

#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif


#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif


#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif


#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}


#else	/* HANDLE_SIGNALS */


/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}


#endif	/* HANDLE_SIGNALS */



/*
 * Close up the current game (player may or may not be dead)
 *
 * This function is called only from "main.c" and "signals.c".
 *
 * Note that the savefile is not saved until the tombstone is
 * actually displayed and the player has a chance to examine
 * the inventory and such.  This allows cheating if the game
 * is equipped with a "quit without save" method.  XXX XXX XXX
 */
void close_game(void)
{
//	char buf[1024];


    /* Handle stuff */
    handle_stuff();

    /* Flush the messages */
    msg_print(NULL);

    /* Flush the input */
    flush();


    /* No suspending now */
    signals_ignore_tstp();


    /* ... */


	/* Allow suspending now */
    signals_handle_tstp();
}

void handle_stuff(void) {

}

void
play_game(bool new_game) {

    new_game = 0;  // to avoid warning
    
    /* Hack -- Increase "icky" depth */
    character_icky++;

    /* Verify main term */
    if (!angband_term[0])
    {
	quit("main window does not exist");
    }

    /* Make sure main term is active */
    Term_activate(angband_term[0]);

    /* Verify minimum size */
    if ((Term->hgt < 24) || (Term->wid < 80))
    {
	quit("main window is too small");
    }

    /* Forbid resizing */
    Term->fixed_shape = TRUE;

    /* Hack -- Turn off the cursor */
    (void)Term_set_cursor(0);

    if (lisp_will_use_callback) {
	// this is a callback
	play_game_lisp();
    }
	
}

/* Here we try to do a few things on the C-side to avoid allocating
   things on the Lisp-side. */

#define MAX_BUF_SZ 1024
static char *hidden_buffer = NULL;

static void
clean_hidden_buffer() {
    if (!hidden_buffer) {
	C_MAKE(hidden_buffer, MAX_BUF_SZ, char);
    }
    memset(hidden_buffer,MAX_BUF_SZ,0);
}

/*
 * Converts stat num into a six-char (right justified) string
 */
static void
cnv_stat(int val, char *out_val) {

    /* Above 18 */
    if (val > 18) {
	int bonus = (val - 18);

	if (bonus >= 100) {
	    sprintf(out_val, "18/%03d", bonus);
	}
	else {
	    sprintf(out_val, " 18/%02d", bonus);
	}
    }

    /* From 3 to 18 */
    else {
	sprintf(out_val, "    %2d", val);
    }
}


void
print_coloured_stat (byte attr,
		     int stat,
		     int row,
		     int col) {
    
    clean_hidden_buffer();
    cnv_stat(stat,hidden_buffer);
    c_put_str(attr,hidden_buffer, row, col);

}

static const char *token_list[] = {
    "", // 0
    "Name",
    "Cur MP",
    "Max MP",
    "Level",
    "LEVEL", // 5
    "Exp",
    "EXP",
    "Cur HP",
    "Max HP",
    "Cur AC", // 10
    "AU",
    "Str",
    "Int",
    "Wis",
    "Dex", // 15
    "Con",
    "Chr",
    "STR",
    "INT",
    "WIS", //20
    "DEX",
    "CON",
    "CHR",
    "            "
};

void
print_coloured_token (byte attr,
		      int token,
		      int row,
		      int col) {

//    printf("Going for token %d\n", token);
//    printf("This token is %s\n", token_list[token]);
    c_put_str(attr,token_list[token], row, col);
    
}

void
print_coloured_number (byte attr,
			long number,
			int padding,
			int row,
			int col) {

    char *format_str = "%ld";
    clean_hidden_buffer();
    if (padding == 9) {
	format_str = "%9ld";
    }
    else if (padding == 5) {
	format_str = "%5ld";
    }
    else if (padding == 8) {
	format_str = "%8ld";
    }
    else if (padding == 6) {
	format_str = "%6ld";
    }
    else {
	printf("no print_col_number for %d\n", padding);
    }
    sprintf(hidden_buffer, format_str, number);
    c_put_str(attr,hidden_buffer, row, col);

}

#ifdef SMALL_BOYS_FOR_BREAKFAST
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
#endif


int
test_calling(const char *arr, const char *alt) {

    if (arr == NULL) {
//	fprintf(stderr, "Arr is NULL\n");
    }
    if (alt == NULL) {
//	fprintf(stderr, "Alt is NULL\n");
    }
    if (alt == NULL && arr == NULL) {
	return 0;
    }

    if (arr && alt) {
//	fprintf(stderr, "Have two strings\n");
	if (arr == alt) {
//	    fprintf(stderr, "Strings are eq: [%s]\n", alt);
	    return 0;
	}
	else if (!strcmp(arr,alt)) {
//	    fprintf(stderr, "Strings are equal: [%s]\n", alt);
	    return 0;
	}
	else if (!strcasecmp(arr,alt)) {
//	    fprintf(stderr, "Strings are equalp, and [%s] [%s]\n", arr, alt);
	    return 1;
	}
	else {
//	    fprintf(stderr, "Strings aren't even equalp: [%s] vs [%s]\n", arr, alt);
	    return -1;
	}
    }
    
    return -1;
    
}


int
test_calling_2(const char *arr) {

    if (arr) {
	int len = strlen(arr);
	if (len > 10) {
	    printf("My function got |%s|\n", arr);
	}
	return len;
    }
    else {
	return -1;
    }
}
