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
#include <softPwm.h>
#define trigPin 5
#define echoPin 4
#define LED1 0
#define SERVO 1
#define SWITCH 4

static int isThread = 1;
static int sock;
char img[] = "sudo raspistill -o /home/pi/Pictures/img.jpg";
char email[] = "mpack -s 'IMAGE' /home/pi/Pictures/img.jpg jinyeong5359@daum.net";

void * ThreadFunction(void * arg){
char message2[100];


while(isThread){
if(read(sock, message2, sizeof(message2)-1) == -1){
printf("read error\n");
exit(0);
}
printf("host : %s", message2); 
digitalWrite(LED1, 0); 

if(strcmp(message2, "exit\n") == 0){
isThread = 0;
exit(0);
}
}
printf("end");
pthread_exit(0);
}


int GetDistance()
{
	printf("22");
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

return (int)distance; 
}


void main(){

wiringPiSetup();
pinMode(trigPin, OUTPUT); 
pinMode(echoPin, INPUT); 
pinMode(LED1, OUTPUT); 
pinMode(SWITCH,INPUT);
//softPwmCreate(SERVO, 0 ,200);
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

printf("connected!!1\n");
digitalWrite(LED1, 0);
printf("connected!!2\n"); 
isThread = 1;
printf("connected!!3\n"); 
pthread_create(&tid, &attr, &ThreadFunction, (void*)0);
printf("connected!!4\n"); 
int count = 0;
printf("connected!!5\n"); 
while(1){
int dis = GetDistance();
printf("connected!!6\n"); 
char message[100] = "car detection\n"; 
 

if(dis <= 10){
	printf("here/n");
system(img); 
system(email); 
write(sock, message, sizeof(message)); 
printf("prsent here /n");
digitalWrite(LED1, 1); 

while(digitalRead(SWITCH) == HIGH){printf("prsent here too/n");}
	
	//softPwmWrite(SERVO,24);
	digitalWrite(LED1, 0); 
	count = 0;
}


if(strcmp(message, "exit\n") == 0){
isThread = 0;
break;
}
}

close(sock);
}
