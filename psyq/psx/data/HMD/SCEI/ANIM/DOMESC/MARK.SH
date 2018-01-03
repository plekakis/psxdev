#!/bin/sh

awk '
/^(PolyPrim_|SharedPolyPrim_)/ {
	print;
	getline; print;	# DEV_ID,...
	getline; print;	# PRIM_TYPE,...
	pt = substr($1, 2, length($1) - 1);
	getline; print;	# size, data
	n = substr($2, 5, length($2) - 7) + 0;
	getline; print;	# idx
	lab = substr($1, 2);
	type[lab] = pt;
	num[lab] = n;
	next;
}
/^(Poly_|SharedPoly_)/ {
	lab = substr($1, 1, length($1) - 1);
	printf("%s\t//%s %d\n", $0, type[lab], num[lab]);
	next;
}
{
	print;
}
' $@
