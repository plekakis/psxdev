        opt C+
       	section datasection

        global gtmd
        global bg
        global TIMaddr
        global Zbuffer
        global tex

        cnop 0,4
gtmd:
        incbin "gt.tmd"

		cnop 0,4
bg:
		cnop $31368,4
TIMaddr:
        incbin "lroom.tim"

        cnop 0,4
Zbuffer:
        incbin "sroomd"

        cnop 0,4
tex:
        incbin "ceramic6.tim"
        cnop 0,4
