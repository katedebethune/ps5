#include	"ws13.h"
/*
 *	some constants
 */

#define	MAXFLD	40
#define	MAXVAL	120

#define PAIR_DELIM '='
#define DEFAULT_REC_DELIM '\n'
#define	FMT_DELIM '%'
#define UN_FMT_DELIM '!'
#define SYM_TAB_END_OF_RECORD "complete"
#define WRITE_NONE 0
#define WRITE_TAG 1
#define WRITE_VAL 2
#define WRITE_FMT_OPN 3
#define WRITE_FMT_CLS 4
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

struct arr_builder {
		char tag[MAXFLD + 1];
		char val[MAXVAL + 1];
};

/*
 *	function declarlations
 */

int	get_record(symtab_t *, FILE *, char ent_delim, char rec_delim);
void	mailmerge( symtab_t *, FILE *);
int	process(FILE*, FILE*, char ent_delim, char rec_delim);
void	fatal(char *, char *);
struct arr_builder build_table(char c, int write_flag);
int set_write_flag(int c, int prevchar, int write_flag, char ent_delim, char rec_delim);
int write_fmt_tag(int c, int write_flag, symtab_t *tp);
