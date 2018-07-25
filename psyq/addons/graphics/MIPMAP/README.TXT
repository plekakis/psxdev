GsTMDfastTF4LM        	mip-map Flat Texture Square (Light Source Calc.)
GsTMDfastTF4LFGM      	mip-map Flat Texture Square (Light Source Calc.) {FOG)
GsTMDfastTF4NLM       	mip-map Flat Texture Square (Without Light Source Calc.)
GsTMDfastTNF4M       	mip-map Flat Texture Square (Without Light Source Calc.)
GsTMDfastTG4LM      	mip-map Gouraud Texture Square(Light Source Calc.)
GsTMDfastTG4LFGM      	mip-map Gouraud Texture Square(Light Source Calc.){FOG)
GsTMDfastTG4NLM       	mip-map Gouraud Texture Square(Without Light Source Calc.)
GsTMDfastTNG4M        	mip-map Gouraud Texture Square(Without Light Source Calc.)
GsTMDdivTF4LM        	mip-map Flat Texture Square(Fixed Division) {Light Source Calc.)
GsTMDdivTF4LFGM      	mip-map Flat Texture Square
				(Fixed Division) {Light Source Calc.){FOG)
GsTMDdivTF4NLM        	mip-map Flat Texture Square
				(Fixed Division){Without Light Source Calc.)
GsTMDdivTNF4M         	mip-map Flat Texture Square
				(Fixed Division){Without Light Source Calc.)
GsTMDdivTG4LM         	mip-map Gouraud Texture Square
				(Fixed Division){Light Source Calc.)
GsTMDdivTG4LFGM       	mip-map Gouraud Texture Square
				(Fixed Division){Light Source Calc.){FOG)
GsTMDdivTG4NLM        	mip-map Gouraud Texture Square
				(Fixed Division){Without Light Source Calc.)
GsTMDdivTNG4M        	mip-map Gouraud Texture Square
				(Fixed Division){Without Light Source Calc.)
GsA4divTF4LM          	mip-map Flat Texture Square
				(Automatic Division){Light Source Calc.)
GsA4divTF4LFGM        	mip-map Flat Texture Square
				(Automatic Division){Light Source Calc.){FOG)
GsA4divTF4NLM         	mip-map Flat Texture Square
				(Automatic Division){Without Light Source Calc.)
GsA4divTNF4M          	mip-map Flat Texture Square
				(Automatic Division){Without Light Source Calc.)
GsA4divTG4LM          	mip-map Gouraud Texture Square
				(Automatic Division){Light Source Calc.)
GsA4divTG4LFGM        	mip-map Gouraud Texture Square
				(Automatic Division){Light Source Calc.){FOG)
GsA4divTG4NLM         	mip-map Gouraud Texture Square
				(Automatic Division){Without Light Source Calc.)
GsA4divTNG4M         	mip-map Gouraud Texture Square
				(Automatic Division){Without Light Source Calc.)
        Syntax 
		   PACKET *GsTMDfastTF4LM(TMD_P_TF4 *op, VERT *vp, VERT *np,
              		PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTF4LFGM(TMD_P_TF4 *op, VERT *vp, VERT *np,
                   	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTF4NLM(TMD_P_TF4 *op, VERT *vp, VERT *np,
                   	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTNF4M(TMD_P_TF4 *op, VERT *vp, 
                   	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTG4LM(TMD_P_TG4 *op, VERT *vp, VERT *np,
                   	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTG4LFGM(TMD_P_TG4 *op, VERT *vp, VERT *np,
               	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTG4NLM(TMD_P_TG4 *op, VERT *vp, VERT *np,
                   	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDfastTNG4M(TMD_P_TG4 *op, VERT *vp, VERT *np,
                   	PACKET *pk,int n,int shift, GsOT *ot)
               PACKET *GsTMDdivTF4LM(TMD_P_TF4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,
			DIVPOLYGON4 *divp)
               PACKET *GsTMDdivTF4LFGM(TMD_P_TF4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
               PACKET *GsTMDdivTF4NLM(TMD_P_TF4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
              	PACKET *GsTMDdivTNF4M(TMD_P_TF4 *op, VERT *vp,
                    	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
               PACKET *GsTMDdivTG4LM(TMD_P_TG4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
               PACKET *GsTMDdivTG4LFGM(TMD_P_TG4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
               PACKET *GsTMDdivTG4NLM(TMD_P_TG4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
               PACKET *GsTMDdivTNG4M(TMD_P_TG4 *op, VERT *vp,
                   	PACKET *pk,int n,int shift, GsOT *ot,	DIVPOLYGON4 *divp)
               PACKET *GsA4divTF4LM(TMD_P_TF4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot,	u_long *scratch)
               PACKET *GsA4divTF4LFGM(TMD_P_TF4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)
               PACKET *GsA4divTF4NLM(TMD_P_TF4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)
               PACKET *GsA4divTNF4M(TMD_P_TF4 *op, VERT *vp,
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)
               PACKET *GsA4divTG4LM(TMD_P_TG4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)
               PACKET *GsA4divTG4LFGM(TMD_P_TG4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)
               PACKET *GsA4divTG4NLM(TMD_P_TG4 *op, VERT *vp, VERT *np, 
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)
               PACKET *GsA4divTNG4M(TMD_P_TG4 *op, VERT *vp,
                   	PACKET *pk,int n,int shift, GsOT *ot, u_long *scratch)

        Argument	op      	TMD PRIMITIVE Starting Address   
                	vp      	TMD VERTEXS Starting Address 
                	np      	TMD NORMAL Starting Address
                	pk      	GPU Packet Buffer Starting Address
                	n       	Number of PRIMITIVEs 
                	shift    	Number of bits to be shifted when sorting to OT
                	ot      	Pointer to GsOT
                	scratch 	Non-used scratch pad Starting Address
        Explanation    
		Low level function group of GsSortObject4J()
            Need to be registered into GsFCALL4 as a low level function 
		before using.

        	This function performs mip-map, texture switching based 
		on the polygon size , to the flat texture squares included in
		the TMD data.

		Locate the texture on the V-RAM as below;
               
                +--------------+-------+
                |              |       |
                |              |       |
                |              |       |
                |              +---+---+
                |              | +-+   |
                |              | +-+---+
                |              | 
                +--------------+
                There are four texture sizes,  1, 1/4, 1/16, and 1/64.
                Which texture size should be used is determined by the 			polygon's  outer product.	
 	
                Polygon vertices must be in the order below;
                
                        0        1
                        +--------+
                        |        |
                        |        |
                        |        |
                        |        |
                        +--------+
                        2        3
        Return Value  
			Non-used Packet Area Starting Address
