#define CAPACITY 5

struct Msg_Queue{
	int max_size;
	int front, tail;
	int queue[CAPACITY+1];
	
	Msg_Queue();

	void init();
	
	bool empty();
	
	bool full();
	
	bool push(int item);
	
	bool pop();
	
	void show();
};
