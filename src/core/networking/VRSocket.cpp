#include "VRSocket.h"
#include "VRPing.h"
#include "mongoose/mongoose.h"
#include "core/objects/object/VRObject.h"
#include "core/scene/VRScene.h"
#include "core/scene/VRSceneManager.h"
#include "core/setup/devices/VRDevice.h"
#include "core/utils/toString.h"
#include "core/utils/VRLogger.h"
#include "core/utils/system/VRSystem.h"

#include <algorithm>
#ifndef WIN32
#include <curl/curl.h> // TODO: windows port
#endif
#include <stdint.h>
#include <libxml++/nodes/element.h>
#include <boost/bind.hpp>

OSG_BEGIN_NAMESPACE
using namespace std;


//mongoose server-------------------------------------------------------------

HTTP_args::HTTP_args() {
    params = shared_ptr<map<string, string>>( new map<string, string>() );
    pages = shared_ptr<map<string, string>>( new map<string, string>() );
    callbacks = shared_ptr<map<string, VRServerCbWeakPtr>>( new map<string, VRServerCbWeakPtr>() );
}

HTTP_args::~HTTP_args() {}

void HTTP_args::print() {
    stringstream ss;
    ss << "HTTP args: " << path << endl;
    if (params != 0)
        for (auto p : *params) ss << "  " << p.first << " : " << p.second << endl;
    VRLog::log("net", ss.str());
}

HTTP_args* HTTP_args::copy() {
    HTTP_args* res = new HTTP_args();
    res->cb = cb;
    *res->params = *params;
    *res->pages = *pages;
    *res->callbacks = *callbacks;
    res->path = path;
    res->websocket = websocket;
    res->ws_data = ws_data;
    res->ws_id = ws_id;
    res->serv = serv;
    return res;
}

void server_answer_job(HTTP_args* args) {
    if (VRLog::tag("net")) {
        stringstream ss; ss << "server_answer_job: " << args->cb << endl;
        VRLog::log("net", ss.str());
    }
    //cout << " ---- server_answer_job: " << args->ws_data << endl;
    //args->print();
    if (args->cb) (*args->cb)(args);
    delete args;
}

static void server_answer_to_connection_m(struct mg_connection *conn, int ev, void *ev_data);
static struct mg_serve_http_opts s_http_server_opts;

class HTTPServer {
    private:
        VRThreadCbPtr serverThread;

    public:
        //server----------------------------------------------------------------
        //struct MHD_Daemon* server = 0;
        struct mg_mgr* server = 0;
        struct mg_bind_opts bind_opts;
        struct mg_connection *nc;
        const char *err_str;
        int threadID = 0;
        HTTP_args* data = 0;

        map<mg_connection*, int> websocket_ids;
        map<int, mg_connection*> websockets;

        HTTPServer() {
            data = new HTTP_args();
            data->serv = this;
        }

        ~HTTPServer() {
            delete data;
        }

        void loop(VRThreadWeakPtr wt) {
            if (server) mg_mgr_poll(server, 100);
            if (auto t = wt.lock()) if (t->control_flag == false) return;
        }

        void initServer(VRHTTP_cb* fkt, int port) {
            data->cb = fkt;
            server = new mg_mgr();
            mg_mgr_init(server, data);
            //server = mg_create_server(data, server_answer_to_connection_m);
            memset(&bind_opts, 0, sizeof(bind_opts));
            bind_opts.error_string = &err_str;
            nc = mg_bind_opt(server, toString(port).c_str(), server_answer_to_connection_m, bind_opts);
            mg_set_protocol_http_websocket(nc);
            //s_http_server_opts.document_root = ".";
            s_http_server_opts.enable_directory_listing = "yes";
            //mg_set_option(server, "listening_port", toString(port).c_str());

            serverThread = VRFunction<VRThreadWeakPtr>::create("mongoose loop", boost::bind(&HTTPServer::loop, this, _1));
            threadID = VRSceneManager::get()->initThread(serverThread, "mongoose", true);

            //server = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL, &server_answer_to_connection, data, MHD_OPTION_END);
        }

        void addPage(string path, string page) {
            (*data->pages)[path] = page;
        }

        void remPage(string path) {
            if (data->pages->count(path)) {
                data->pages->erase(path);
            }
        }

        void addCallback(string path, VRServerCbPtr cb) {
            if (data->callbacks->count(path) == 0) (*data->callbacks)[path] = cb;
        }

        void remCallback(string path) {
            if (data->callbacks->count(path)) {
                data->callbacks->erase(path);
            }
        }

        void close() {
            if (server) {
                VRSceneManager::get()->stopThread(threadID);
                mg_mgr_free(server);
                delete server;
            }
            server = 0;
        }

        void websocket_send(int id, string message) {
            if (websockets.count(id) && websockets[id]) mg_send_websocket_frame(websockets[id], WEBSOCKET_OP_TEXT, message.c_str(), message.size());
        }

        int websocket_open(string address, string protocols) {
            int newID = websockets.size();
            auto c = mg_connect_ws(server, server_answer_to_connection_m, address.c_str(), protocols.c_str(), NULL);
            if (c) websockets[newID] = c;
            else newID = -1;
            return newID;
        }
};

static void server_answer_to_connection_m(struct mg_connection *conn, int ev, void *ev_data) {
    //VRLog::setTag("net",1);
    bool v = VRLog::tag("net");
    if (v) {
        if (ev == MG_EV_CONNECT) { VRLog::log("net", "MG_EV_CONNECT\n"); return; }
        if (ev == MG_EV_RECV) { VRLog::log("net", "MG_EV_RECV\n"); return; }
        if (ev == MG_EV_POLL) { return; }
        if (ev == MG_EV_ACCEPT) { VRLog::log("net", "MG_EV_ACCEPT\n"); return; }
        if (ev == MG_EV_SEND) { VRLog::log("net", "MG_EV_SEND\n"); return; }
        if (ev == MG_EV_TIMER) { VRLog::log("net", "MG_EV_TIMER\n"); return; }
        if (ev == MG_EV_WEBSOCKET_HANDSHAKE_DONE) { VRLog::log("net", "MG_EV_WEBSOCKET_HANDSHAKE_DONE\n"); return; }
        if (ev == MG_EV_WEBSOCKET_CONTROL_FRAME) { VRLog::log("net", "MG_EV_WEBSOCKET_CONTROL_FRAME\n"); return; }
    }

    HTTP_args* sad = (HTTP_args*) conn->mgr->user_data;

    if (ev == MG_EV_CLOSE) {
        VRLog::log("net", "MG_EV_CLOSE\n");
        //HTTP_args* sad = (HTTP_args*) conn->mgr_data;
        if (sad->serv->websocket_ids.count(conn)) {
            int wsid = sad->serv->websocket_ids[conn];
            sad->serv->websockets.erase(wsid);
            sad->serv->websocket_ids.erase(conn);
        }
        return;
    }

    if (ev == MG_EV_WEBSOCKET_FRAME) {
        sad->websocket = true;
        sad->path = "";
        sad->params->clear();

        VRLog::log("net", "MG_EV_WEBSOCKET_FRAME\n");
        if (v) sad->print();

        struct websocket_message* wm = (struct websocket_message*) ev_data;
        sad->ws_data = string(reinterpret_cast<char const*>(wm->data), wm->size);

        int wslid = sad->serv->websocket_ids.size();
        if (!sad->serv->websocket_ids.count(conn)) { sad->serv->websocket_ids[conn] = wslid; sad->serv->websockets[wslid] = conn; }
        sad->ws_id = sad->serv->websocket_ids[conn];

        auto fkt = VRUpdateCb::create("HTTP_answer_job", boost::bind(server_answer_job, sad->copy()));
        VRSceneManager::get()->queueJob(fkt);
        return;
    }

    if (ev == MG_EV_HTTP_REQUEST) {
        VRLog::log("net", "MG_EV_HTTP_REQUEST\n");
        //HTTP_args* sad = (HTTP_args*) conn->mgr_data;
        sad->websocket = false;

        struct http_message *hm = (struct http_message *)ev_data;
        string method_s(hm->method.p, hm->method.len);//GET, POST, ...
        string section(hm->uri.p+1, hm->uri.len-1); //path
        sad->path = section;
        sad->params->clear();

        string params;
        if (hm->query_string.p) params = string(hm->query_string.p, hm->query_string.len);
        for (auto pp : splitString(params, '&')) {
            vector<string> d = splitString(pp, '=');
            if (d.size() != 2) continue;
            (*sad->params)[d[0]] = d[1];
        }

        if (v) VRLog::log("net", "HTTP Request\n");
        if (v) sad->print();

        auto sendString = [&](string data) {
            mg_send_head(conn, 200, data.size(), "Transfer-Encoding: chunked");
            mg_send_http_chunk(conn, data.c_str(), data.size());
            mg_send_http_chunk(conn, "", 0);
        };

        //--- respond to client ------
        if (sad->path == "") {
            if (v) VRLog::log("net", "Send empty string\n");
            sendString("");
        }

        if (sad->path != "") {
            if (sad->pages->count(sad->path)) { // return local site
                string spage = (*sad->pages)[sad->path];
                sendString(spage);
                if (v) VRLog::log("net", "Send local site\n");
            } else if(sad->callbacks->count(sad->path)) { // return callback
                VRServerCbPtr cb = (*sad->callbacks)[sad->path].lock();
                if (cb) {
                    string res = (*cb)(*sad->params);
                    sendString(res);
                    if (v) VRLog::log("net", "Send callback response\n");
                }
            } else { // return ressources
                if (!exists( sad->path )) {
                    if (v) VRLog::wrn("net", "Did not find ressource: " + sad->path + "\n");
                    if (v) VRLog::log("net", "Send empty string\n");
                    sendString("");
                }
                else {
                    if (v) VRLog::log("net", "Serve ressource\n");
                    mg_serve_http(conn, hm, s_http_server_opts);
                    // mg_http_serve_file
                    return;
                }
            }
        }

        //--- process request --------
        auto fkt = VRUpdateCb::create("HTTP_answer_job", boost::bind(server_answer_job, sad->copy()));
        VRSceneManager::get()->queueJob(fkt);
        return;
    }

    VRLog::log("net", "unhandled event: " + toString(ev));
}


VRSocket::VRSocket(string name) {
    tcp_fkt = 0;
    http_fkt = 0;
    socketID = 0;
    run = false;
    http_args = 0;
    http_serv = 0;

    setOverrideCallbacks(true);
    queued_signal = VRUpdateCb::create("signal_trigger", boost::bind(&VRSocket::trigger, this));
    sig = VRSignal::create();
    setNameSpace("Sockets");
    setName(name);
    http_serv = new HTTPServer();

    store("type", &type);
    store("port", &port);
    store("ip", &IP);
    store("signal", &signal);
}

VRSocket::~VRSocket() {
    run = false;
    //shutdown(socketID, SHUT_RDWR);
    VRSceneManager::get()->stopThread(threadID);
    if (http_args) delete http_args;
    if (http_serv) delete http_serv;
}

std::shared_ptr<VRSocket> VRSocket::create(string name) { return std::shared_ptr<VRSocket>(new VRSocket(name)); }

int VRSocket::openWebSocket(string address, string protocols) {
    if (http_serv) return http_serv->websocket_open(address, protocols);
    return -1;
}

void VRSocket::answerWebSocket(int id, string msg) {
    if (http_serv) http_serv->websocket_send(id, msg);
}

void VRSocket::trigger() {
    if (tcp_fkt) (*tcp_fkt)(tcp_msg);
    if (http_fkt) (*http_fkt)(http_args);
}

void VRSocket::handle(string s) {
    auto scene = VRScene::getCurrent();
    if (scene == 0) return;
    tcp_msg = s;
    scene->queueJob(queued_signal);
}

//CURL HTTP client--------------------------------------------------------------
size_t httpwritefkt( char *ptr, size_t size, size_t nmemb, void *userdata) {
    string* s = (string*)userdata;
    s->append(ptr, size*nmemb);
    return size*nmemb;
}

void VRSocket::sendHTTPGet(string uri) {
    auto curl = curl_easy_init();
    //curl_easy_setopt(curl, CURLOPT_GET, 1);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    auto res = curl_easy_perform(curl);
    if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
}

/*void VRSocket::sendMessage(string msg) {
    if (type == "http post") {
        curl = curl_easy_init();
        server = IP+":"+toString(port); // TODO: add uri args
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, server.c_str());//scheme://host:port/path
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, server.c_str());//use this to parse the header lines!
    }

    cout << "\nSOCKET SEND " << msg << endl;

    unsigned int sockfd, n;
    struct sockaddr_in serv_addr;
    struct sockaddr* serv;
    struct hostent *server;

    //char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("ERROR opening socket"); return; }

    server = gethostbyname(IP.c_str());
    if (server == NULL) { fprintf(stderr,"ERROR, no such host\n"); return; }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    //fcntl(sockfd, F_SETFL, O_NONBLOCK);
    serv = (struct sockaddr*)&serv_addr;
    if (connect(sockfd, serv, sizeof(serv_addr)) < 0) { perror("ERROR connecting"); return; }

    n = write(sockfd, msg.c_str(), msg.size());
    if (n < 0) { perror("ERROR writing to socket"); return; }

    close(sockfd);

    // Now read server response
    //n = read(sockfd, buffer, 255);
    //if (n < 0) { perror("ERROR reading from socket"); return; }
    //printf("%s\n",buffer);
}*/

void VRSocket::scanUnix(VRThreadWeakPtr thread) {
    //scan what new stuff is in the socket
    /*unsigned int s, len, contype;
    struct sockaddr_un local_u, remote_u;
    struct sockaddr* local;
    struct sockaddr* remote;
    unsigned int t;

    contype = AF_UNIX;

    int _s;
    if ((_s = socket(contype, SOCK_STREAM, 0)) < 0) { cout << "\nERROR: socket\n"; return; }
    s = _s;

    ifconf ic;
    ioctl(s, SIOCGIFCONF, ic);
    IP = string(ic.ifc_buf);

    local_u.sun_family = AF_UNIX; // unix
    strcpy(local_u.sun_path, "/tmp/vrf_soc"); // unix
    unlink(local_u.sun_path); // unix
    len = strlen(local_u.sun_path) + sizeof(local_u.sun_family); // unix
    local = (struct sockaddr *)&local_u;
    remote = (struct sockaddr *)&remote_u;
    t = sizeof(remote_u);

    if (bind(s, local, len) == -1) { cout << "\nERROR: socket bind\n"; return; }
    if (listen(s, 5) == -1) { cout << "\nERROR: socket listen\n"; return; }

    string cmd;

    while(true) {
        //printf("\n\nWaiting for a connection...\n");
        if ((_s = accept(s, remote, &t)) < 0) { cout << "\nERROR: socket accept\n"; return; }
        socketID = _s;

        //const int soc_len = 8192;//auch im client socket muss die laenge passen!
        const int soc_len = 1024;
        char str[soc_len];
        int cmd_len;

        //printf("Connected.\n");

        cmd_len = recv(socketID, str, soc_len, 0);

        if (cmd_len < 0) cout << "\n recv error\n";
        if (cmd_len <= 0) return;

        cmd = string(str);
        handle(cmd);

        strcpy(str, cmd.c_str());
        //cout << "\nsend: " << str << flush;
        if ( send(socketID, str, strlen(str), 0) < 0) { perror("send"); return; }

        close(socketID);
    }*/
}

void VRSocket::scanTCP(VRThreadWeakPtr thread) {
    //scan what new stuff is in the socket
    /*unsigned int socketAcc, len, contype;
    struct sockaddr_in local_i, remote_i;
    struct sockaddr* local;
    struct sockaddr* remote;
    unsigned int t;

    contype = AF_INET;

    int s;
    if ((s = socket(contype, SOCK_STREAM, 0)) < 0) { cout << "\nERROR: socket\n"; return; }
    socketID = s;

    ifconf ic;
    ioctl(socketID, SIOCGIFCONF, ic);
    IP = string(ic.ifc_buf);

    local_i.sin_family = AF_INET;
    local_i.sin_port = htons( port );
    local_i.sin_addr.s_addr = INADDR_ANY;
    len = sizeof(local_i);
    local = (struct sockaddr *)&local_i;
    remote = (struct sockaddr *)&remote_i;
    t = sizeof(remote_i);

    if (bind(socketID, local, len) < 0) { cout << "\nERROR: socket bind\n"; return; }
    if (listen(socketID, 5) < 0) { cout << "\nERROR: socket listen\n"; return; }

    string cmd;

    while(run) {
        printf("\n\nWaiting for a connection...\n");
        if ((s = accept(socketID, remote, &t)) == -1) { cout << "\nERROR: socket accept\n"; break; }
        socketAcc = s;

        //const int soc_len = 8192;//auch im client socket muss die laenge passen!
        const int soc_len = 1024;
        char str[soc_len];
        int cmd_len;

        cmd_len = recv(socketAcc, str, soc_len, 0);
        cout << "\nSOCKET CONNECTED " << str << endl;

        if (cmd_len < 0) cout << "\n recv error\n";
        if (cmd_len <= 0) break;

        cmd = string(str);
        handle(cmd);

        strcpy(str, cmd.c_str());
        //cout << "\nsend: " << str << flush;
        if ( send(socketAcc, str, strlen(str), 0) < 0) { perror("send"); break; }

        close(socketAcc);
    }
    //close(sListen);
    //close(sBind);
    close(socketID);*/
}

void VRSocket::initServer(CONNECTION_TYPE t, int _port) {
    port = _port;
    if (t == UNIX) socketThread = VRFunction<VRThreadWeakPtr>::create("UNIXSocket", boost::bind(&VRSocket::scanUnix, this, _1));
    if (t == TCP) socketThread = VRFunction<VRThreadWeakPtr>::create("TCPSocket", boost::bind(&VRSocket::scanTCP, this, _1));
    run = true;
    threadID = VRSceneManager::get()->initThread(socketThread, "socket", true);
}

void VRSocket::update() {
    run = false;
    //shutdown(socketID, SHUT_RDWR);
    VRSceneManager::get()->stopThread(threadID);
    if (http_serv) http_serv->close();

    sig->setName("on_" + name + "_" + type);

    if (type == "tcpip receive") if (tcp_fkt) initServer(TCP, port);
    if (type == "http receive") if (http_serv && http_fkt) http_serv->initServer(http_fkt, port);
}

bool VRSocket::isClient() {
    if (type == "tcpip send") return true;
    if (type == "http post") return true;
    if (type == "http get") return true;
    return false;
}

void VRSocket::setName(string n) { name = n; update(); }
void VRSocket::setType(string t) { type = t; update(); }
void VRSocket::setTCPCallback(VRTCP_cb* cb) { tcp_fkt = cb; update(); }
void VRSocket::setHTTPCallback(VRHTTP_cb* cb) { http_fkt = cb; update(); }
void VRSocket::setIP(string s) { IP = s; }
void VRSocket::setSignal(string s) { signal = s; update(); }
void VRSocket::setPort(int i) { port = i; update(); }
void VRSocket::unsetCallbacks() { tcp_fkt = 0; http_fkt = 0; update(); }
void VRSocket::addHTTPPage(string path, string page) { if (http_serv) http_serv->addPage(path, page); }
void VRSocket::remHTTPPage(string path) { if (http_serv) http_serv->remPage(path); }
void VRSocket::addHTTPCallback(string path, VRServerCbPtr cb) { if (http_serv) http_serv->addCallback(path, cb); }
void VRSocket::remHTTPCallback(string path) { if (http_serv) http_serv->remCallback(path); }

string VRSocket::getType() { return type; }
string VRSocket::getIP() { return IP; }
string VRSocket::getCallback() { return callback; }
VRSignalPtr VRSocket::getSignal() { return sig; }
int VRSocket::getPort() { return port; }

bool VRSocket::ping(string IP, string port) {
    VRPing ping;
    return ping.start(IP, port, 0);
}

OSG_END_NAMESPACE
