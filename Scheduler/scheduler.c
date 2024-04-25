#include "../Clock/headers.h"


int main(int argc, char * argv[])
{
    initClk();
    getClk();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    destroyClk(true);
}
