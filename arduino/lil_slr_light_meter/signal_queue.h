// signal queue
typedef struct Signal_item {
  Signal sig;
  int data;
  boolean consume; // indicates that this signal must be consumed (i.e. will only be removed from the queue if a state transition consumes it)
} Signal_item;


#define Q_LEN 10
Signal_item q[Q_LEN];
int q_enter = 0;
int q_size = 0;
Signal_item none;

boolean is_in_queue (struct Signal_item s) {
  for (int i = 0; i < q_size; i++) {
    struct Signal_item this_s;
    int q_i = (i + q_enter + 1) % Q_LEN;
    this_s = q[q_i];
    if (this_s.sig == s.sig && this_s.data == s.data) {
      return true;
    }
  }
  return false;
}

boolean enqueue (int s, int data, boolean must_be_consumed, boolean no_duplicates) {
  if (q_size >= Q_LEN) {
    // we have a problem in that the queue is full.
    return false;
  }
  //if (no_duplicates && is_in_queue(s)) {
  //  return false;
  //}
  q[q_enter].sig = (Signal)s;
  q[q_enter].data = data;
  q[q_enter].consume = must_be_consumed;
  q_enter += -1;
  if (q_enter < 0) {q_enter += Q_LEN;}
  q_size += 1;
  return true;
}

struct Signal_item dequeue () {
  if (q_size == 0) {
    return none;
  }
  int q_exit = (q_enter + q_size) % Q_LEN;
  q_size -= 1;
  return (q[q_exit]);
}

int queue_size () {
  return (q_size);
}

#ifdef DEBUG_QUEUE
void test_signal_queue() {
  int failed_tests = 0;
  delay(3000);
  Signal_item test;
  enqueue(btn_down, 5, false, false);
  if (queue_size() != 1) {
    // failed test
    Serial.println("queue_size failed to report 1 after enqueue();");
    failed_tests++;
  }
  enqueue(btn_down, 88, false, false);
  if (queue_size() != 2) {
    // failed test
    Serial.println("queue_size failed to report 2 after second enqueue();");
    failed_tests++;
  }
  test = dequeue();
  if (queue_size() != 1) {
    // failed test
    Serial.println("queue_size failed to report 1 after dequeue();");
    failed_tests++;
  }
  if (test.data != 5) {
    // failed test
    Serial.println("dequeue() did not return an item with data == 5");
    failed_tests++;
  }
  test = dequeue();
  if (queue_size() != 0) {
    // failed test
    Serial.println("queue_size failed to report 0 after dequeue();");
    failed_tests++;
  }
  if (test.data != 88) {
    // failed test
    Serial.println("dequeue() did not return an item with data == 88");
    failed_tests++;
  }
  if (!failed_tests) {
    Serial.println("passed tests!");
  }
}
#endif

