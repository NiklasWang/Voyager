#ifndef _VOYAGER_PROTOCOL_H_
#define _VOYAGER_PROTOCOL_H_

#define SOCKET_SUFFIX_AFTER_SERVER_NAME    "_socket_server"

#define SOCKET_CLIENT_GREETING_STR         "Greetings."

#define SOCKET_SEND_SHARE_OVERALL_CONTROL  "Fine, prepare overall control."

#define SOCKET_REPLY_OVERALL_CONTROL       "Ready for overall control."

#define SOCKET_DONE_OVERALL_CONTROL        "Done overall control."

#define SOCKET_START_REQUEST_CONNECTION    "Good, start request connection."


#define SOCKET_CLIENT_SEND_REQUEST         "Tansfer request type <%TYPE%> private <%PRIVATE%>"

#define SOCKET_SERVER_REPLY_REQUEST_OK     "OK, waitting."

#define SOCKET_SERVER_REPLY_REQUEST_NO     "Denied, not requested."

#define SOCKET_CLIENT_SEND_REQUEST_DONE    "Client done."

#define SOCKT_SERVER_RECEIVED              "Transfer OK."
#define SOCKT_SERVER_FAILED_RECEIVE        "Transfer Failed."

#define SOCKET_SERVER_PROCESSED            "Process OK."
#define SOCKET_SERVER_PROCESS_FAILED       "Process Failed."

#define SOCKET_CLIENT_EVENT_FORMAT         "Event:%EVENT%:%ARG1%:%ARG2%"

#endif
