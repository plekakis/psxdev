
all:
        cd qmesh
        psymake
        cd .\..

        cd rmesh
        psymake
        cd .\..

        cd smesh
        psymake
        cd .\..
