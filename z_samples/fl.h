#include	"ws13.h"
/*
 *	some constants
 */

#define	MAXFLD	40
#define	MAXVAL	120
#define	RECORD_DELIM '\n'
#define ENTITY_DELIM ';'
#define PAIR_DELIM '='
#define WRITE_NONE 0
#define WRITE_TAG 1
#define WRITE_VAL 2

/*
 *	function declarlations
 */

int	get_record(symtab_t *, FILE *);
void	mailmerge( symtab_t *, FILE *);
int	process(FILE*, FILE*);
void	fatal(char *, char *);
