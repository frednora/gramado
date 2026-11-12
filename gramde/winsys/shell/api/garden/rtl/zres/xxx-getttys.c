static char *
getttys(f)  //fd ?
{
	static char line[32];
	register char *lp;

	lp = line;
	for (;;) {
		if (read(f, lp, 1) != 1)
			return(NULL);
		if (*lp =='\n') {
			*lp = '\0';
			return(line+2);
		}
		if (lp >= &line[32])
			return(line+2);
		lp++;
	}
}
