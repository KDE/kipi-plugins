// Local
#include "gpmessages.h"

GPMessages* GPMessages::gpMessages=0;

GPMessages* GPMessages::gpMessagesWrapper() {
    if (!gpMessages) {
        gpMessages = new GPMessages;
    }
    return gpMessages;
}

void GPMessages::deleteMessagesWrapper() {
    if (gpMessages) {
        delete gpMessages;
        gpMessages=0;
    }
}
