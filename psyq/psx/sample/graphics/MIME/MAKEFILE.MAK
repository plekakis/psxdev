all:
        cd joint
        psymake
        cd .\..

        cd vertex
        psymake
        cd .\..

        cd vjmime
        psymake
        cd .\..
