#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "myhtml/api.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t size){

        char * data = reinterpret_cast<char*>(const_cast<uint8_t*>(Data));
        if(size < 6){
                return 0;
        }

        myhtml_t * myhtml = myhtml_create();

	/*TODO */

        myhtml_destroy(myhtml);
        return 0;
}
