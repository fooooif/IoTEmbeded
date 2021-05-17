#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>

#define trigPin 4 // 초음파센서 사용을 위한 트리그핀 설정
#define echoPin 5 // 초음파센서 사용을 위한 에코핀 설정
#define LED1 0 // 빨간색 LED 센서 사용을 위한 핀번호 설정


static int isThread = 1;
static int sock;
char img[] = "sudo raspistill -o /home/pi/Pictures/img.jpg"; // 사진 찍는 명령을 실행하는 명령어를 배열에 저장
char email[] = "mpack -s 'IMAGE' /home/pi/Pictures/img.jpg jinyeong5359@daum.net"; // 메일보내기 명령을 실행하는 명령어를 배열에 저장

void * ThreadFunction(void * arg){
char message2[100];

// 서버 측에서 데이터가 들어왔을 시
while(isThread){
if(read(sock, message2, sizeof(message2)-1) == -1){
printf("read error\n");
exit(0);
}
printf("host : %s", message2); // pc서버 측에서 호스트가 메일을 보냈다는 메시지를 확인하고 접근자가 발생했다는 것을 인지하였다는 메시지 수신

digitalWrite(LED1, 0); // host가 확인하였으니 접근자가 오면 켜지는 빨간불을 꺼줌

if(strcmp(message2, "exit\n") == 0){
isThread = 0;
exit(0);
}
}
printf("end");
pthread_exit(0);
}

// 초음파센서 거리측정해주는 함수
int GetDistance()
{
digitalWrite(trigPin, LOW);
usleep(2);
digitalWrite(trigPin, HIGH);
usleep(20);
digitalWrite(trigPin, LOW);

if(wiringPiSetup() == -1) return 0;

while(digitalRead(echoPin) == LOW);
long startTime = micros();

while(digitalRead(echoPin) == HIGH);
long travelTime = micros() - startTime;

int distance = travelTime / 58;
delay(100);

return (int)distance; // return 값으로 거리값 반환
}
//메인 함수
void main(){

wiringPiSetup(); // 초음파센서와 LED 센서를 사용하기 위해 작성
pinMode(trigPin, OUTPUT); // 출력 지정
pinMode(echoPin, INPUT); // 입력 지정
pinMode(LED1, OUTPUT); // 출력 지정


pthread_t tid;
pthread_attr_t attr;
pthread_attr_init(&attr);

struct sockaddr_in serv_addr;
int str_len;

if((sock = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP)) == -1){
printf("socket error\n");
return;
}

memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr("192.168.0.4");
serv_addr.sin_port = htons(23000);

if(connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
printf("connect error\n");
return;
}

printf("connected!!\n");


digitalWrite(LED1, 0); // 평소에는 빨간불이 꺼지도록 함

isThread = 1;
pthread_create(&tid, &attr, &ThreadFunction, (void*)0);
// 서버측으로 데이터를 보낼 시
while(1){
int dis = GetDistance();
char message[100] = "There is an intruder!\nWarning!!!Warning!!!Warning!!!\nPlease respond!\n"; // 접근자 발생 경고문을 배열에 지정
 
// 침입자의 거리가 10cm이하일 시
if(dis <= 10){
system(img); // 사진 찍는 명령을 실행
system(email); // 메일 보내는 명령을 실행

digitalWrite(LED1, 1); // 접근자가 오면 켜지는 빨간불을 켜줌
write(sock, message, sizeof(message)); // 위의 접근자 발생 경고문을 서버 측에 보내줌
}

if(strcmp(message, "exit\n") == 0){
isThread = 0;
break;
}
}
close(sock);
}
