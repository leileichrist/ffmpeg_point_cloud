
// #include <boost/array.hpp>
#include <boost/thread.hpp>
// #include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


const int MAXDATASIZE = 1024;
typedef std::vector<uint8_t> buffer_type;


typedef struct Buffer_t{
  bool busy;
  int id;
  buffer_type data;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
}Buffer;

Buffer buf[2];

void play_video_func(){
  boost::posix_time::seconds workTime(1);
  std::cout << "Play thread: playing video " << std::endl;
  // Pretend to do something useful...
  boost::this_thread::sleep(workTime);
  std::cout << "Play thread: finished" << std::endl;
}

void flip(int &current){
  if(current==0){
    current=1;
  }else{
    current=0;
  }
}

void* network_thread(void *ptr){
  int sockfd = *(int*)(ptr);
  int next = 0;


  return NULL;
}

void* display_thread(void *ptr){
  int next = 0;

  //there are 60 video clip to display (the client already knows this info)
  for(int i=0; i<60; i++){

  }

  return NULL;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int connect_to_host(char *hostname, char *port){
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
    printf("connecting to %s at port %s\n", hostname, port);
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return -1;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  return sockfd;

}

int main(int argc, char* argv[])
{
  char host[100];
  char port[100];
  if(argc!=3){
    sprintf(host, "%s", "10.0.0.9");
    sprintf(port, "%s", "8000"); 
  }else{
    sprintf(host, "%s", argv[1]);
    sprintf(port, "%s", argv[2]); 
  }

  //init buffer:
  for(int i=0; i<2; i++){
    buf[i].id = i;
    buf[i].busy=false;
    pthread_mutex_init(&buf[i].mutex, NULL);
    pthread_cond_init(&buf[i].cond, NULL);
  }

  int sockfd = connect_to_host(host, port);
  pthread_t network_t;
  pthread_t display_t;

  pthread_create(&network_t, NULL, network_thread, (void*)(&sockfd));
  pthread_create(&display_t, NULL, display_thread, NULL);

  pthread_join(network_t, NULL);
  pthread_join(display_t, NULL);

  for(int i=0; i<2; i++){
    pthread_mutex_destroy(&buf[i].mutex);
    pthread_cond_destroy(&buf[i].cond);
  }

  return 0;
}