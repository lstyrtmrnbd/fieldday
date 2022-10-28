/**
 * fieldday
 * relax and have fun
 */

#include "readerwriterqueue.h"
#include "common_forms.hpp" //necessary?
#include "rend.hpp"
#include "phyz.hpp"

using rwq = moodycamel::ReaderWriterQueue;

int main() {

  //init queues
  rwq<poke> input(369);
  rwq<state> output(369);
  
  //init phyz
  auto phyz = phyziks(input, output);

    
  //init render
  auto my_type_request = requestMyType();
  auto my_descriptor = initRend(my_type_request);

  //dispatch_thread(phyz, args);
  
  //give main thread to render
  render(my_descriptor);

  cout << "we did it yay" << endl;
  return 0;
}


