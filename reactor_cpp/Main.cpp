#include "DiagnosticsServer.h"

// Include the appropriate reactor implementation
#ifdef USE_SELECT_REACTOR
#include "SelectReactor.h"
#else
#include "PollReactor.h"
#endif

int main(void)
{
    /* Create the reactor instance */
#ifdef USE_SELECT_REACTOR
    SelectReactor reactor;
#else
    PollReactor reactor;
#endif

    /* Create a diagnostics server listening on the configured port */
    const unsigned int serverPort = SERV_PORT;
    DiagnosticsServer server(serverPort, &reactor);

    /* Enter the eternal reactive event loop */
    for(;;){
        reactor.handleEvents();
    }

    return 0;
}
