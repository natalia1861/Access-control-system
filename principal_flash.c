#include "principal.h"
#include "flash.h"
#include "rfid.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Principal
 *---------------------------------------------------------------------------*/
 
 #define OPERACION 				0x04

bool mensaje_Recibido = false;
  //thread
osThreadId_t tid_Thread_principal;                        // thread id
void test_flash_users (void *argument);                   // thread function
void test_flash_events (void*argument);
void test_generic_password (void *argument);

//colas
static osMessageQueueId_t rfid_queue;
static osMessageQueueId_t flash_queue;
extern osThreadId_t tid_flash;
//mensajes
static MSGQUEUE_RC522_t msg_rfid;
static MSGQUEUE_USER_t user;
static MSGQUEUE_EVENT_t event;
static MSGQUEUE_GEN_PASS_t generic_password;

int Init_Thread_principal (void) {
   const static osThreadAttr_t th_attr = {.stack_size = 7000};
  tid_Thread_principal = osThreadNew(test_generic_password, NULL, &th_attr);
  if (tid_Thread_principal == NULL) {
    return(-1);
  }
	return 0;
}

void test_flash_users (void *argument) {
	uint32_t flags;
  //rfid_queue = Init_ThMFRC522();
  flash_queue = Init_ThFlash();
  while (1) {
		//add 3 different users
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user1(); //natalia
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user2(); //ainara
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user3(); //lorena
		//add the same 3 users to see if they are really added (must not)
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user1();
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user2();
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user3();
		//we get all the users, to see if there are only 3 new users
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		osThreadFlagsSet(tid_flash, GET_ALL_USERS);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //natalia
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //ainara
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //lorena
    
    //osMessageQueueGet(flash_queue, &user, NULL, osWaitForever);
		//osMessageQueueGet(flash_queue, &user, NULL, osWaitForever);
		//we delete user 1
    uint8_t id[5];
    id[0] = 0xD3;
    id[1] = 0xB0;
    id[2] = 0x02;
    id[3] = 0xF8;
    id[4] = 0x99;
	
    memcpy(user.id, id, 5);
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		osMessageQueuePut(flash_queue, &user, NULL, osWaitForever);
		osThreadFlagsSet(tid_flash, DELETE_USER); //eliminamos natalia
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		//we get the 2 remain users
		osThreadFlagsSet(tid_flash, GET_ALL_USERS);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //ainara
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //lorena
		//we add user 4 (it must be in position 1)
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user4(); //amaya
		//we get all the users, to see if there are 3 again
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		osThreadFlagsSet(tid_flash, GET_ALL_USERS);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //amaya
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //ainara
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //lorena
		//we get 1 user information
	id[0] = 0xD5;
	id[1] = 0xB2;
	id[2] = 0x02;
	id[3] = 0xF8;
	id[4] = 0x99;
		memcpy(user.id, id, 5);
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		osMessageQueuePut(flash_queue, &user, NULL, osWaitForever);
		osThreadFlagsSet(tid_flash, GET_USER);
		osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //amaya
    //we add user 1
		add_user1();
    osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    osThreadFlagsSet(tid_flash, GET_ALL_USERS);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //amaya
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //ainara
    osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //lorena
    osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //natalia
		//we wait
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever);
//    osMessageQueueGet(rfid_queue, &msg_rfid, NULL, osWaitForever);
//    mensaje_Recibido = true;
  }
}

void test_flash_events (void *argument) {
  	uint32_t flags;
  //rfid_queue = Init_ThMFRC522();
  flash_queue = Init_ThFlash();
  while (1) {
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    add_user1();
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    add_event1();
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    add_event1();
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    osThreadFlagsSet(tid_flash, GET_ALL_EVENTS);
		osMessageQueueGet(flash_queue, &event, NULL, osWaitForever);
		osMessageQueueGet(flash_queue, &event, NULL, osWaitForever);
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    osThreadFlagsSet(tid_flash, DELETE_EVENTS);
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    add_event1();
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    osMessageQueueGet(flash_queue, &event, NULL, osWaitForever);
  }
}


void test_generic_password (void *argument) {
  uint32_t flags;
  flash_queue = Init_ThFlash();
  while (1) {
    osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		add_user1(); //natalia
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    osThreadFlagsSet(tid_flash,  FLAG_POWER_DOWN);
    osDelay(2000);
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    add_user2();
    flags = osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
    osThreadFlagsSet(tid_flash,  FLAG_POWER_UP);
    osThreadFlagsWait(READY, osFlagsWaitAny, osWaitForever);
		osThreadFlagsSet(tid_flash, GET_ALL_USERS);
		osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //natalia
    osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //natalia
    osMessageQueueGet(flash_queue, &user, NULL, osWaitForever); //natalia
  }
}
