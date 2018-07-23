## REQUISITOS ##
- Compilador C++ (gcc)
- OpenCV
- Visual Studio (Pra Windows)

## INSTALAÇÃO ##
- Windows:
* Baixar a versão do OpenCV 2.4.13 para Windows
* Instalar
* Configurar variaveis de ambiente do OpenCV
Dica: https://docs.opencv.org/2.4/doc/tutorials/introduction/windows_install/windows_install.html
Dica (Aqui também ensina configurar variaveis de ambiente): https://www.opencv-srf.com/2017/11/install-opencv-with-visual-studio.html

- Linux:
* Instalar os pacotes:
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
* CLonar o repositorio do opencv (selecionar a branch 2.4)
cd ~/<my_working _directory>
git clone https://github.com/opencv/opencv.git
* Criar o make
cd ~/opencv
mkdir release
cd release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
* Compilar/instalar o oepncv
make
sudo make install

Dica: https://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html

## EXECUÇÃO ##
- Opção 1: executar através do arquvio Make com os seguintes comandos:
make all
./lib.o
*** se tiver problemas: make clean

- Opção 2: Criar um projeto no Visual Studio referenciando o OpenCV e importar o arquivo lib.cpp para o projeto
* https://www.opencv-srf.com/2017/11/install-opencv-with-visual-studio.html
