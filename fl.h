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
