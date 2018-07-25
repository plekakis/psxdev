#!/bin/sh

awk '
/PRIM_TYPE/ {
	if ($1 ~ /PRIM_TYPE/) {
		pt = substr($1, 12, length($1) - 13);
		if ($1 !~ /LMD/) {
			if (pt ~ /TRI/ || pt ~ /QUAD/) {
				printf("\t\t|PRIM_TYPE(%s|COL);\n", pt);
			} else {
				printf("\t\t|PRIM_TYPE(%s);\n", pt);
			}
		} else {
			printf("\t\t|PRIM_TYPE(%s);\n", pt);
		}
	} else {
		print;
		pt = $6;
		if (pt !~ /LMD/) {
			if (pt ~ /TRI/) {
				m = "CT";
				if (pt ~ /TME/) {
					code = "0x34";
				} else {
					code = "0x30";
				}
			} else if (pt ~ /QUAD/) {
				m = "CQ";
				if (pt ~ /TME/) {
					code = "0x3c";
				} else {
					code = "0x38";
				}
			} else {
				next;
			}

			n = $7 + 0;
			for (i = 0; i < n; i++) {
				printf("\t%s(%s);\n", m, code);
				while (getline) {
					if ($0 !~ /CODE/ && $0 !~ /code/) {
						print;
					}
					if ($0 ~ /^$/) {
						break;
					}
				}
			}
		}
	}
	next;
}
{
	print;
}
' $@|
sed 's@#include <hmd/hmd.def>@#include "mac.def"@'
