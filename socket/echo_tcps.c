#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void err_func(char *msg){
	//エラー出力関数
	perror(msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	int sockfd, len, new_sockfd;
	char buf[BUFSIZ];
	//sockaddr_in構造体 <netinet/in.h>で定義されている
	struct sockaddr_in serv, clt;
	socklen_t sin_siz;
	unsigned short port;

	if(argc != 3){
		printf("usage: progname ip port\n");
		exit(EXIT_FAILURE);
	}

	//第１引数:IPv4で通信しますよ  第２引数:TCPで通信しますよ
	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		err_func("socket");
	}

	//ドメイン設定=IPv4という意味
	serv.sin_family = PF_INET;
	//atoiは文字列を数値に変換 unsigned shortにキャスト
	port = (unsigned short)atoi(argv[2]);
	//htonsでネットワークバイトオーダに変換している
	serv.sin_port = htons(port);
	inet_aton(argv[1], &(serv.sin_addr));
	sin_siz = sizeof(struct sockaddr_in);

	if(bind(sockfd, (struct sockaddr *)&serv, sin_siz) < 0){
		err_func("bind");
	}

	if(listen(sockfd, SOMAXCONN) < 0){
		err_func("listen");
	}

	while(1){
		if((new_sockfd = accept(sockfd, (struct sockaddr *)&clt, &sin_siz)) < 0){
			err_func("accept");
		}
		printf("connect from %s : %u\n",inet_ntoa(clt.sin_addr),ntohs(clt.sin_port));
		memset(buf, 0, BUFSIZ);

		while(strncmp(buf, "EXIT\r\n", 6) != 0 && strncmp(buf, "EXIT\n",5) != 0){
			len = recv(new_sockfd,buf,BUFSIZ,0);
			buf[len] = 0x1a;
			printf("<- %s",buf);
			len = send(new_sockfd,buf,len,0);
		}

		close(new_sockfd);
	}
	close(sockfd);
	return 0;
}
