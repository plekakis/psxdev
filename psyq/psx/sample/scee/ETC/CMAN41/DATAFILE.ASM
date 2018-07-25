    opt     c+
   	section datasection

        global IconData
        global vabHeader
        global vabBody
        global seqAddr

		cnop 0,4

IconData:
    incbin "IconAll.tim"

vabHeader:
    incbin "sample.vh"

vabBody:
    incbin "sample.vb"

seqAddr:
    incbin "mozart.seq"
