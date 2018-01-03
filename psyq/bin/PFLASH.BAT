freset -r 1
fload cleanmem
fload -b00200000 kernel.bin
fload -b00264000 kromgdat.bin
fload -b00266000 kromdat.bin
fload -b00220000 SN.bin
fload pflash
fquit
freset 1
