<div align="center">
<img src= "https://user-images.githubusercontent.com/94376190/230129990-71a2933a-c49e-4117-97ca-30a3271d2a4e.png" width = "100px"
/>
</div>
<h2 align="center"> Sistemas Operacionais: Primeiro Seminário 
</h2>
<h3 align="center"> 👾 Servidor de Jogos para Match 👾</h3>
<br></br>

## Como compilar o Jogo da Velha no terminal do Windows (VSCODE):

gcc -o jogo_da_velha jogo_da_velha.c

.\jogo_da_velha
<br></br>
## Como compilar o Jogo da Forca no Terminal Linux:

/mnt/c/Users/reisw/OneDrive/Documentos/

MarciaGabrielle_WandressaReis_ws_OS_RR_2023$

gcc forca.c -o forca

.\forca
<br></br>
## Como instalar a biblioteca pthread na WSL:

sudo apt update

sudo apt install build-essential

sudo apt install libpthread-stubs0-dev
<br></br>
## Para certificar-se de que a biblioteca pthread está utilizável no código

 gcc -pthread -o forca forca.c