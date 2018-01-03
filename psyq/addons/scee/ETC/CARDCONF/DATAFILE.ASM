   	section datasection
        global icontim
        global vabheader
        global vabbody
        global seqdata
        global marvindata
      
        cnop 0,4
icontim:
	incbin "icon.tim"

        cnop 0,4
vabheader:
	incbin "sample.vh"

        cnop 0,4
vabbody:
	incbin "sample.vb"

        cnop 0,4
seqdata:
	incbin "mozart.seq"

        cnop 0,4
marvindata:
	incbin "marvin.tim"



