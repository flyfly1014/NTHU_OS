#include <assert.h>
#include <stdlib.h>
#include "ts_queue.hpp"
#include "item.hpp"
#include "reader.hpp"
#include "writer.hpp"
#include "producer.hpp"
#include "consumer_controller.hpp"

#define READER_QUEUE_SIZE 200
#define WORKER_QUEUE_SIZE 200
#define WRITER_QUEUE_SIZE 4000
#define CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE 20
#define CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE 80
#define CONSUMER_CONTROLLER_CHECK_PERIOD 1000000

int main(int argc, char** argv) {
	assert(argc == 4);

	int n = atoi(argv[1]);
	std::string input_file_name(argv[2]);
	std::string output_file_name(argv[3]);

	// TODO: implements main function

	TSQueue<Item*> *input_queue=new TSQueue<Item*>(READER_QUEUE_SIZE);
	TSQueue<Item*> *worker_queue=new TSQueue<Item*>(WORKER_QUEUE_SIZE);
	TSQueue<Item*> *output_queue=new TSQueue<Item*>(WRITER_QUEUE_SIZE);

	Transformer *transformer=new Transformer();

	Reader *reader=new Reader(n, input_file_name, input_queue);
	Writer *writer=new Writer(n, output_file_name, output_queue);

	Producer* p1=new Producer(input_queue, worker_queue, transformer);
	Producer* p2=new Producer(input_queue, worker_queue, transformer);
	Producer* p3=new Producer(input_queue, worker_queue, transformer);
	Producer* p4=new Producer(input_queue, worker_queue, transformer);
	
	ConsumerController* controller=new ConsumerController(worker_queue,output_queue,
	transformer,CONSUMER_CONTROLLER_CHECK_PERIOD,CONSUMER_CONTROLLER_LOW_THRESHOLD_PERCENTAGE
	,CONSUMER_CONTROLLER_HIGH_THRESHOLD_PERCENTAGE);

	//1 reader thread
	reader->start();
	//1 writer thread
	writer->start();
	//1 controller thread
	controller->start();
	
	//4 producer threads
	p1->start();
	p2->start();
	p3->start();
	p4->start();
	
	reader->join();
	writer->join();
	
	//terminated	
	delete p1;delete p2;delete p3;delete p4;
	delete controller;	
	delete reader;
	delete writer;
	delete transformer;
	delete input_queue;delete worker_queue;delete output_queue;
	return 0;
}
