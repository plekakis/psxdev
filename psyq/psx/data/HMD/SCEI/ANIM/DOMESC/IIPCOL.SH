#!/bin/sh

awk '
/^(Poly_|SharedPoly_)/ {
	print;
	while (getline) {
		line1 = $0;
		if (line1 !~ /^(Poly_|SharedPoly_)/ && line1 ~ /^[A-Z]/) {
			print line1;
			next;
		}
		if (line1 ~ /CODE/ || line1 ~ /code/) {
			code = substr($4, 3, 4);
			getline; line2 = $0;
			if (line2 ~ /CODE/ || line2 ~ /code/) {
				getline; line3 = $0;
				getline; line4 = $0;
				if (line4 ~ /CODE/ || line4 ~ /code/) {
					printf("\tCQ(%s);\n", code);
				} else {
					printf("\tCT(%s);\n", code);
					print line4;
				}
			} else {
				print line1;
				print line2;
				next;
			}
		} else {
			print line1;
		}
	}
}
{
	print;
}
' $@
