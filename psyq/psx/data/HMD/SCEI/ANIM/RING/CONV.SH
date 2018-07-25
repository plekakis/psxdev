#!/bin/sh
#
#	This script generates "tiled.lab" from LAB file generated
#	by D-STORM AnimationSaver 1.41.
#	This version of AnimationSaver does not support "COL" type,
#	thus, this script generates data of that type from object name.
#
#	Ver 1.00	July 9, 1998	by N. Yoshioka
#

#
#	First of all, convert line feed char from DOS to UNIX style.
#
tr -d '' <$@ |
#
#	Copy type related object names to each primitives
#	to make the next job easy.
#
awk '
/^(PolyPrimSet_|SharedPrimSet_)/ {
	print;		# label its self
	#name = substr($4, 2, 16);
	name = substr($4, 2, length($4) - 2);
	#if (name !~ /^tme/) {
	#	next;
	#}

	getline; print;	# next prim
	getline; print;	# prim header
	getline; print;	# num of types
	num = substr($1, 3, length($1) - 4);
	for (i = 0; i < num; i++) {
		getline; print;	# empty line
		getline;	# label
		printf("%s\t/* %s */\n", $1, name);
		getline; print;	# DEV_ID, CTG and DRV
		getline; print;	# PRIM_TYPE
		getline; print;	# size, data
		size = substr($1, 3, length($1) - 4);
		for (j = 0; j < size - 1; j++) {
			getline; print;	# data
		}
	}
	next;
}
{
	print;
}
' |
#
# You should do the same process as the above awk script manually.
# Because we have no way to determin Shared Primitives actual type
# automatically.  The following sed script depends on "ring.lab".
#
sed	\
-e 's@^\(SharedPolyPrim_0000\).*@\1:	/* tme1col0iip1lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0001\).*@\1:	/* tme0col0iip1lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0002\).*@\1:	/* tme1col0iip0lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0003\).*@\1:	/* tme0col0iip0lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0004\).*@\1:	/* tme1col1iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0005\).*@\1:	/* tme0col1iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0006\).*@\1:	/* tme1col0iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0007\).*@\1:	/* tme0col0iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0008\).*@\1:	/* tme1col1iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0009\).*@\1:	/* tme0col1iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0010\).*@\1:	/* tme1col0iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0011\).*@\1:	/* tme0col0iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0012\).*@\1:	/* tme1col0iip1lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0013\).*@\1:	/* tme0col0iip1lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0014\).*@\1:	/* tme1col0iip0lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0015\).*@\1:	/* tme0col0iip0lmd1 */@'	\
-e 's@^\(SharedPolyPrim_0016\).*@\1:	/* tme1col1iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0017\).*@\1:	/* tme0col1iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0018\).*@\1:	/* tme1col0iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0019\).*@\1:	/* tme0col0iip1lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0020\).*@\1:	/* tme1col1iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0021\).*@\1:	/* tme0col1iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0022\).*@\1:	/* tme1col0iip0lmd0 */@'	\
-e 's@^\(SharedPolyPrim_0023\).*@\1:	/* tme0col0iip0lmd0 */@' |
#
#	Copy PRIM_TYPE and polygon count to after each polygon data label
#	and remove unnecessary comments from there.
#
awk '
/^(PolyPrim_|SharedPolyPrim_)/ {
	print;		# label its self
	name = $3;
	getline; print;	# DEV_ID, CTG and DRV
	getline; print;	# PRIM_TYPE
	type = substr($1, 12, length($1) - 13);
	getline; print;	# size, data
	num = substr($2, 5, length($2) - 7);
	getline; print;	# polygon index
	idx = substr($1, 2);	# remove "("
	types[idx] = type;
	nums[idx] = num;
	names[idx] = name;
	next;
}
/^(Poly_|SharedPoly_)/ {
	idx = substr($1, 1, length($1) - 1);	# remove ":"
	printf("%s\t/* %s ; %d polys */\t/* %s */\n",
		$1, names[idx], nums[idx], types[idx]);
	next;
}
{
	print;
}
' |
#
#	Re-generate PRIM_TYPE from object name.
#
awk '
/^(PolyPrim_|SharedPolyPrim_)/ {
	print;		# label its self
	name = $3;
	if (name !~ /^tme/) {
		next;
	}

	getline;	# DEV_ID, CTG and DRV
	split($1, s, "|");
	devid = substr(s[1], 8, length(s[1]) - 8);
	ctg = substr(s[2], 5, length(s[2]) - 5);
	drv = substr(s[3], 5, length(s[3]) - 5);
	getline;	# PRIM_TYPE
	type = substr($1, 12, length($1) - 13);
	split(type, s, "|");
	kind = s[1];
	type = "";
	if (name ~ /tme1/) {
		type = type"|TILE";
	}
	if (name ~ /col1/) {
		type = type"|COL";
	}
	if (name ~ /lmd1/) {
		type = type"|LMD";
		drv = "LGT";
	}
	if (name ~ /iip1/) {
		type = type"|IIP";
	}
	if (name ~ /tme1/) {
		type = type"|TME";
	}
	printf("\tDEV_ID(%s)|CTG(%s)|DRV(%s|XDRV)\n", devid, ctg, drv);
	printf("\t\t|PRIM_TYPE(%s%s);\n", kind, type);
	getline; print;	# size, data
	getline; print;	# polygon index

	next;
}
{
	print;
}
' |
#
#	Do a job that is similar to the first one.
#	This time, comment removing is not performed.
#
awk '
/^(PolyPrim_|SharedPolyPrim_)/ {
	print;		# label its self
	getline; print;	# DEV_ID, CTG and DRV
	getline; print;	# PRIM_TYPE
	type = substr($1, 12, length($1) - 13);
	getline; print;	# size, data
	getline; print;	# polygon index
	idx = substr($1, 2);	# remove "("
	types[idx] = type;
	next;
}
/^(Poly_|SharedPoly_)/ {
	idx = substr($1, 1, length($1) - 1);	# remove ":"
	printf("%s\t/* %s */\n", $0, types[idx]);
	next;
}
{
	print;
}
' |
#
#	Add DIVNUM for DIV or ADV.
#
awk '
/^(PolyPrim_|SharedPolyPrim_)/ {
	print;		# label its self
	name = $3;
	if (name !~ /^tme/) {
		next;
	}

	getline; print;	# DEV_ID, CTG and DRV
	getline; print;	# PRIM_TYPE
	type = substr($1, 12, length($1) - 13);
	getline; print;	# size, data
	getline;	# polygon index
	s = substr($0, 2, length($0) - 2);	# remove "\t" and ";"
	printf("\tDIVNUM|(%s);\n", s);
	next;
}
{
	print;
}
' |
#
#	Check difference and mark "HERE".
#
awk '
/^(Poly_|SharedPoly_)/ {
	label = $1;
	name = $3;
	num = $5;
	type1 = $9;
	type2 = $12;
	if (type1 == type2) {
		printf("%s\t/* %s ; %d polys ; %s */\n",
			label, name, num, type1);
		next;
	}
	if ($1 ~ /SharedPoly_/ && type2 !~ /IIP/ && type2 !~ /LMD/) {
		printf("%s\t/* %s ; %d polys ; %s */\n",
			label, name, num, type1);
		print("\t/* not defined; this data has no meaning */");
		next;
	}
	printf("%s\t/* %s ; %d polys ; %s -> %s */\n",
		label, name, num, type1, type2);
	next;
}
{
	print;
}
' |
#
#	Here we go!
#
awk '
/->/ {
	num = $5;
	type1 = $8;
	type2 = $10;
	print;
	for (i = 0; i < num; i++) {
		#printf("\t/* %d */\n", i);
		if (type2 == "TRI|COL") {
			getline;	# avoid org color
			printf("\tRGB3(0x30);\n");
		} else if (type2 == "TRI|COL|IIP") {
			getline;	# avoid org color
			printf("\tRGB3(0x30);\n");
		} else if (type2 == "QUAD|COL") {
			getline;	# avoid org color
			printf("\tRGB4(0x38);\n");
		} else if (type2 == "QUAD|COL|IIP") {
			getline;	# avoid org color
			printf("\tRGB4(0x38);\n");
		} else if (type2 == "TRI|TILE|COL|TME") {
			getline; print;
			printf("\tRGB3(0x34);\n");
		} else if (type2 == "TRI|TILE|COL|IIP|TME") {
			getline; print;
			printf("\tRGB3(0x34);\n");
		} else if (type2 == "QUAD|TILE|COL|TME") {
			getline; print;
			printf("\tRGB4(0x3c);\n");
		} else if (type2 == "QUAD|TILE|COL|IIP|TME") {
			getline; print;
			printf("\tRGB4(0x3c);\n");
		} else if (type2 == "TRI|TILE|LMD|TME") {
			getline; print;
			getline; printf("\t%s %s %s B(0x24);\n", $1, $2, $3);
			getline;	# avoid 2nd color
			getline;	# avoid 3rd color
		} else if (type2 == "TRI|LMD") {
			getline; printf("\t%s %s %s B(0x20);\n", $1, $2, $3);
			getline;	# avoid 2nd color
			getline;	# avoid 3rd color
		} else if (type2 == "QUAD|TILE|LMD|TME") {
			getline; print;
			getline; printf("\t%s %s %s B(0x2c);\n", $1, $2, $3);
			getline;	# avoid 2nd color
			getline;	# avoid 3rd color
			getline;	# avoid 4th color
		} else if (type2 == "QUAD|LMD") {
			getline; printf("\t%s %s %s B(0x28);\n", $1, $2, $3);
			getline;	# avoid 2nd color
			getline;	# avoid 3rd color
			getline;	# avoid 4th color
		}
		while (getline) {
			print;
			if ($0 ~ /^[ 	]*$/) {
				break;
			}
		}
	}
	next;
}
{
	print;
}
' |
sed 's@#include <hmd/hmd.def>@#include "mac.def"@'
