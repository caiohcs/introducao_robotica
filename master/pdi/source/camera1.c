#include "../headers/cabecalho.h"

int main()
{
	/* Criação da matriz para conter os pixeis da imagem
		para não precisar usar o mmap*/
	
	int fd;
	if((fd = open("/dev/video0", O_RDWR)) == -1){	//abre o file descriptor
		perror("open");
		exit(1);
	}


	/*A chamada de controle VIDIOC_QUERYCAP significa uma chamada acerca do dispositivo
        de vídeo para questionar quais são suas capacidades.
	O VIDIOC_QUERYCAP requer um ponteiro para uma estrutura do tipo v4l2_capability
	Para armazenar o resulado do armazenamento.*/
	struct v4l2_capability cap;
	if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1){
		perror("VIDIOC_QUERYCAP");
		exit(1);
	}
	printf("cap:%u\n",cap.capabilities);
	

	/*A estrutura cap possui atributos diversos, sendo um deles capabilities, um inteiro de 32 bits
	contendo todas as informações acerca das capacidades do dispositivo. As rotinas abaixo comparam uma determinada
	capacidade do dispositivo (V4L2_CAP_VIDEO_CAPTURE e V4L2_CAP_STREAMING) através de um operador & bitwise*/
	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
		fprintf(stderr, "Não ha suporte para single-planaer video capture.\n");
		exit(1);
	}


	/*olha as capacidades do dispositivo e testa se tem single-planar vc e streaming*/

	if(!(cap.capabilities & V4L2_CAP_STREAMING)){	
		fprintf(stderr, "Não ha suporte para streaming.\n");
		exit(1);
	}

	/*Seleção do formato para o dispositivo de vídeo. Os formatos disponíveis
	são vistos através do comando "v4l2-ctl -d /dev/video0 --list-formats-ext".
	Uma vez escolhido o formato, ele é salvo em uma estrutura v4l2_format*/
	
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;	//Formato do frame
	format.fmt.pix.width = 640;			
	format.fmt.pix.height = 480;
	
	
	/*Seta o formato de gravação do dispositivo para o descritor
	através do IOCTL VIDIOC_S_FMT -> set_format*/

	if(ioctl(fd, VIDIOC_S_FMT, &format) == -1){	//passa o formato pro fd
		perror("VIDIOC_S_FMT");
		exit(1);
	}

	/*Aqui dizemos como o dispositivo vai utilizar os buffers.
	De que maneira os dados serão armazenados (Nmap) e quantos
	serão os buffers disponíveis (1)*/
	
	struct v4l2_requestbuffers bufreq;
	bufreq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufreq.memory = V4L2_MEMORY_MMAP;
	bufreq.count = 1;				

	if(ioctl(fd, VIDIOC_REQBUFS, &bufreq) == -1){
		perror("VIDIOC_REQBUFS");
		exit(1);
	}

	/*A quantidade de memória utilizada pelo dispositivo para cada frame
	é calculada aqui e retornada pelo próprio dispositivo. A informação é
	armazenada em uma estrutura do tipo v4l2_buffer e está sendo
	questionada pelo IOCTL VIDIOC_QUERYBUF.*/	

	struct v4l2_buffer bufinfo;
	memset(&bufinfo, 0, sizeof(bufinfo));	//Limpando a memória do buffer para remover o lixo

	bufinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	
	bufinfo.memory = V4L2_MEMORY_MMAP;
	bufinfo.index = 0;

	if(ioctl(fd, VIDIOC_QUERYBUF, &bufinfo) == -1){
		perror("VIDIOC_QUERYBUF");
		exit(1);
	}

	/*Realização do mapeamento da memória. Essa parte não é tão trivial.*/

	unsigned char* bufstart = mmap(NULL, bufinfo.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bufinfo.m.offset);

	if(bufstart == MAP_FAILED){
		perror("mmap");
		exit(1);
	}

	memset(bufstart, 0, bufinfo.length);

	/*Adicionando o buffer já criado na fila de entrada do dispositivo.*/	

	memset(&bufinfo, 0, sizeof(bufinfo));
	
	bufinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufinfo.memory = V4L2_MEMORY_MMAP;
	bufinfo.index = 0;

	/*Inicia o streamming do dispositivo */

	int type = bufinfo.type;
	if(ioctl(fd, VIDIOC_STREAMON, &type) == -1){
		perror("VIDIOC_STREAMON");		
		exit(1);
	}

	/*Adiciona o buffer à fila de entrada */

	if(ioctl(fd, VIDIOC_QBUF, &bufinfo) == -1){
		perror("VIDIOC_QBUF");			
		exit(1);
	}

	/*Retira o buffer da fila de saída */

	if(ioctl(fd, VIDIOC_DQBUF, &bufinfo) == -1){	
		perror("VIDIOC_DQBUF");
		exit(1);
	}

	/* Desativa o streamming do dispositivo */

	if(ioctl(fd, VIDIOC_STREAMOFF, &type) == -1){	
		perror("VIDIOC_STREAMOFF");
		exit(1);
	}


	/*Escreve o arquivo MJPEG, que na verdade é uma sequência de JPEGs,
	no formato JPEG.*/

	int jpgfile;
	if((jpgfile = open("myimage.yuv", O_WRONLY | O_CREAT, 0660)) < 0){
	    perror("open");				
	    exit(1);
	}
	 
	write(jpgfile, bufstart, bufinfo.length);
	close(jpgfile);

	close(fd);
	return 0;
}
