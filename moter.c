
int distance =0;
int pulse = 0;
float val;
pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);
softPwmCreate(SERVO, 0 , 200);
pinMode(LED1,OUTPUT);
pinMode(LED2, INPUT);

while(1){
	digitalWrite(trigPin, LOW);
	usleep(2);
	digitalWrite(trigPin, HIGH);
	usleep(20);
	digitalWrite(trigPin, LOW);
