#include "msg_queue.h"
#include "stdio.h"

Msg_Queue::Msg_Queue(){
	init();
}

void Msg_Queue::init(){
	max_size = CAPACITY + 1;
	// queue = new int[max_size];
	front = 0;
	tail = 0;
	//printf("%d\n", queue[0]);
}

bool Msg_Queue::empty(){
	return tail == front;
}

bool Msg_Queue::full(){
	return (tail + 1) % max_size == front;
}

bool Msg_Queue::push(int item){
	if((tail + 1) % max_size == front)
		return false;
	queue[tail] = item;
	tail = (tail + 1) % max_size;
	return true;
}

bool Msg_Queue::pop(){
	if(tail == front)
		return false;
	front = (front + 1) % max_size;
	return true;
}

void Msg_Queue::show(){
	int i = front;
	for(; i != tail; i=(i+1)%max_size){
		printf("%d ", queue[i]);
	}
	printf("\n");
}
