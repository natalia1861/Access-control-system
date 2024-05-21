#include "principal.h"
#include "rfid.h"
#include "servomotor.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Principal
 *---------------------------------------------------------------------------*/
//variables locales
bool mensaje_Recibido = false;

//timer
//timer
static osTimerId_t id_timer_5s;
static void timer_5s_callback (void *arg);
static void init_timer_5s(void);
static void timer_5s_start(void);
void timer_5s_stop(void);

  //thread
osThreadId_t tid_Thread_principal;                        // thread id
void Thread_principal (void *argument);                   // thread function

//RFID
static osMessageQueueId_t rfid_queue;
static MSGQUEUE_RC522_t msg_rfid;

//SERVO
static osThreadId_t tid_servo;

int Init_Thread_principal (void) {
  tid_Thread_principal = osThreadNew(Thread_principal, NULL, NULL);
  if (tid_Thread_principal == NULL) {
    return(-1);
  }
	return 0;
}

void Thread_principal (void *argument) {
  rfid_queue = Init_ThMFRC522();
  tid_servo = Init_ThServo();
  init_timer_5s();
  
  while (1) {
    osThreadFlagsWait(RFID_READID, osFlagsWaitAny, osWaitForever);
    osMessageQueueGet(rfid_queue, &msg_rfid, NULL, osWaitForever);
    mensaje_Recibido = true;
    osThreadFlagsSet(tid_servo, OPEN_FLAG);
    det_rfid_off();
    timer_5s_start();
  }
}



//TRAS 5 SEG SE CIERRA LA PUERTA
static void timer_5s_start(void) {
  osTimerStart(id_timer_5s, 5000);
}

static void init_timer_5s(void) {
  id_timer_5s = osTimerNew(timer_5s_callback, osTimerOnce, NULL, NULL);
}

static void timer_5s_callback (void *arg) {
	osThreadFlagsSet(tid_servo, CLOSE_FLAG);
  det_rfid_on();
}

void timer_5s_stop(void) {
  osTimerStop(id_timer_5s);
}

