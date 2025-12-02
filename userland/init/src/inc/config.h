// config.h

#ifndef __INIT_CONFIG_H
#define __INIT_CONFIG_H    1

// Test the callback support or not.
// (Step 1): Install the handler at the initialization
// (Step 2): During the input loop we put the thread
//           into the alertable state.
//           The kernel will consume this state,
//           so the loop needs to put it again.
#define CONFIG_TEST_CALLBACK    0


#endif  

