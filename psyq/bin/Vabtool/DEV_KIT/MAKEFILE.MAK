all:
        ccpsx -G 0 -O3 -Xm -Xo$80010000 main.c mouse.c sound.c -osndtool.cpe,sndtool.sym,sndtool.map
load:
        pqbload mouse.tim      80090000
        pqbload keyboard.tim   80090100
        pqbload progbutn.tim   8009A000
        pqbload sample.vab     80100000

