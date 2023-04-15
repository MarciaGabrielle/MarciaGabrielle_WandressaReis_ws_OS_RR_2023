<div align="center">
<img src= "https://user-images.githubusercontent.com/94376190/230129990-71a2933a-c49e-4117-97ca-30a3271d2a4e.png" width = "100px"/>
<h2> Sistemas Operacionais: Primeiro Seminário</h2>
<h3> 👾 Servidor de Jogos para Match 👾</h3>
</div>

## Como compilar o Jogo da Forca no terminal:

```bash
git clone git@github.com:MarciaGabrielle/MarciaGabrielle_WandressaReis_ws_OS_RR_2023.git
cd MarciaGabrielle_WandressaReis_ws_OS_RR_2023
make
```

**OBS: se estiver usando WSL ou se a biblioteca pthread não for encontrada, instale-a com o script:**

```bash
sudo apt update
sudo apt install build-essential libpthread-stubs0-dev
```

## Como executar o Jogo da Forca no terminal:

### Servidor

```bash
./bin/server
```

### Cliente

```bash
./bin/client
```