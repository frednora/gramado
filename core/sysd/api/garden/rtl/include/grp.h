// grp.h


/* see getgrent(3) */
struct group 
{
    char *gr_name;
	char *gr_passwd;
	int	gr_gid;
	char **gr_mem;
};

