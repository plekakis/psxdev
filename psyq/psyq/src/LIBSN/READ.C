#define MAX 32768

extern int _SN_read (int op, int fd, unsigned int n, char *buf);

int PCread (int fd, char *buf, unsigned int n)
{
	int total = 0, actual, amount;

	while (n) {
		if (n > MAX)
			amount = MAX;
		else
			amount = n;

		actual = _SN_read(0, fd, amount, buf);

		if (actual == -1) return -1;	/* error */

		total += actual;
		buf += actual;
		n -= actual;

		if (actual < amount) return (total);	/* end of file */
	}
	return( total );
}

