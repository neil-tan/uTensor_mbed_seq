#include <SensorQueue>

template <class T>
SensorQueue::SensorQueue(uint16_t length, uint16_t blk_length, uint26_t max_pool_blks)
  : blk_length {blk_length}, max_pool_blks {max_pool_blks} {
  
  total_blks = length / blk_length;
  if(length > (total_blks * blk_length)) total_blks += 1;
  blk_offset = 0;
  buffered_blocks = 0;

  //Init Blocks
  for(int i = 0; i < total_blks; i++) {
    newBlock();
  }

  callback_func = NULL;  
}

template <class T>
T* SensorQueue::newBlock(void) {
  blk_offset = 0;
  T* new_blk = malloc(sizeof(T) * blk_length)
  block_input.push_back(new_blk);

  return new_blk;
}

SensorQueue::setCallBack(void (*_fun_ptr)) {
  callback_func = _fun_ptr;
}

template <class T>
void SensorQueue::append_helper(T elem) {
  if(blk_offset >= blk_length) {
    if(buffered_blocks < max_pool_blks) {
      (*callback_func)();
      newBlock();  //new block and resets blk_offset
    } else {
      //printf("error: max_pool_blks exceeded\r\n");
      exit(-1);
    }
  }

  T* current_blk = block_input.back();
  current_blk[blk_offset] = elem;
  blk_offset += 1;
}

template <class T>
void SensorQueue::append(T &elem) {
  queue.call(&append_helper, elem);
  queue.dispatch(0); //returns immediately
}
template <class T>
void copyTo_helper(*void ptr, bool adv_frame) {
  //TODO:  move the following to a blocking event queue call
  for(int i = 0; i < total_blks; i++) {
    memcpy((*void)(ptr + i * blk_length * sizeof(T)), (*void) block_input[i], sizeof(T) * blk_length);
  }

  if(adv_frame && block_input.size() >= blk_length) {
    free(block_input[0]);
    block_input.erase(block_input.begin(), block_input.begin()+1);  //vector is not the best choice for this
  }
}

template <class T>
void copyTo(*void ptr, bool adv_frame) {
  queue.call(&copyTo_helper, ptr, adv_frame);
  queue.dispatch();
}

