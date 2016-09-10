//
// Created by wolverindev on 28.08.16.
//

#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <execinfo.h>
#include <signal.h>
#include "config/Configuration.h"
#include "protocoll/Buffers/DataBuffer.h"
#include "utils/SocketUtil.h"
#include "connection/player/PlayerConnection.h"
#include "protocoll/packet/ClientPacketHandler.h"
#include "server/ServerInfo.h"
#include "encription/Cipper.h"
#include "encription/RSAUtil.h"
#include <stdlib.h>
#include "utils/Base64Utils.h"
#include "cpr/cpr.h"
#include <curl/curl.h>
#include "utils/HexUtils.h"
#include <openssl/sha.h>
using namespace std;

void error(const char* message){
    cerr << message << endl;
    exit(-1);
}

int ssockfd = 0;
sockaddr_in* cli_addr = nullptr;

void shutdownHook(void){
    cout << "Closing socket" << endl;
    close(ssockfd);
    if(cli_addr != nullptr)
        delete cli_addr;
}

void clientConnect(){
    try{
        /*
        int sockfd, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            return;
        }
        server = gethostbyname("localhost");
        if (server == NULL) {
            perror("ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(25565);
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(0);
        }
        */


        ssockfd = SocketUtil::createTCPServerSocket(Configuration::instance->config["network"]["port"].as<int>());
        if(ssockfd < 0){
            ssockfd = SocketUtil::createTCPServerSocket(Configuration::instance->config["network"]["port"].as<int>()+1); //TEST MODE!
            if(ssockfd < 0){
                error("Cant create socket.");
            }
        }
        while (1) {
            cli_addr = new sockaddr_in();
            socklen_t clilen = sizeof(*cli_addr);
            int newsockfd = accept(ssockfd, (struct sockaddr *) cli_addr, &clilen);
            if (newsockfd < 0)
                error("ERROR on accept");

            Socket *connection = new Socket(newsockfd);
            PlayerConnection *playerConnection = new PlayerConnection(cli_addr ,connection);
            playerConnection->start();
            //pthread_join((handler->getThreadHandle()),NULL);
        }
    }catch (Exception* e){
        cout << "Exception message: " << e->what() << endl;
    }
}

int main(int argc, char** argv) {

    char* input = "WolverinDEV";
    char* input2 = "Test";
    cout << "X: " << SHA_DIGEST_LENGTH << endl;
    char* output = (char*) malloc(SHA_DIGEST_LENGTH);
    memset(output,0x00,16);
    SHA_CTX context;
    SHA1_Init(&context);
    SHA1_Update(&context, (unsigned char*)input, strlen(input));
    //SHA1_Update(&context, (unsigned char*)input2, strlen(input2));
    SHA1_Final((unsigned char*) output,&context);

    string outs = HexUtils::hexStr((unsigned char*) output,SHA_DIGEST_LENGTH);
    cout << "Having out" << endl;
    cout << outs << " / " << (int) output[0] << " / " << (int) output[1] << endl;

    atexit(shutdownHook);

    cout << "Generate!" << endl;
    cout << Cipper::publicKey << "/" << sizeof(*(Cipper::publicKey)) << "/" << (Cipper::publicKey) << endl;
    cout << "Generate done!" << endl;
    cout << "Length:" << Cipper::publicKey->engine << endl;

    KeyEncripted* resp = RSAUtil::getPrivateEncriptedKey(Cipper::publicKey);
    cout << "One line: " << resp->getBase64Buffer() << endl;
    DataBuffer* in = new DataBuffer();
    in->writeString("Hello world");
    char* cbuffer = new char[20];
    Cipper* cript = new Cipper((unsigned  char*) Cipper::publicKey,false);
    Cipper* dript = new Cipper((unsigned  char*) Cipper::publicKey,true);

    cript->init();
    dript->init();

    DataBuffer* out = cript->cipher(in,false);

    DataBuffer* out2 = dript->cipher(out,false);
    cout << "Data: ";
    cout << out2->readString() << "Lengthg:";
    cout << out2->getBufferLength() << endl;

    //Cipper::createPublicKey((char*) Cipper::publicKey,sizeof(*Cipper::publicKey));



    if(true && false)
        return 0;
    try {
        string filename = string("config.yml");
        Configuration::instance = new Configuration(filename);
        Configuration::instance->loadConfig();
        cout << "Loading configuration" << endl;
        if(!Configuration::instance->isValid()){
            cout << "Configuration not valid!" << endl;
            vector<string> errors = Configuration::instance->getErrors();
            if(errors.size() == 1)
                cout << "Error: ";
            else
                cout << "Errors (" << errors.size() << "):" << endl;
            for(vector<string>::iterator it = errors.begin();it!=errors.end();it++){
                cout << (errors.size() != 1 ? " - " : "") << *it << endl;
            }
            return 0;
        }
        ServerInfo::loadServers();
        cout << "Configuration valid!" << endl;
        pthread_t threadHandle;
        pthread_create(&threadHandle,NULL,(void* (*)(void*)) &clientConnect,NULL);
        //pthread_join(threadHandle,NULL);


        string line;
        while (1){
            getline(cin, line);
            cout << "Having line: " << line << endl;
            if(strcmp(line.c_str(),"end") == 0){
                cout << "Stpping nativecord" << endl;
                vector<PlayerConnection*> ccopy(PlayerConnection::connections);
                for(vector<PlayerConnection*>::iterator it =ccopy.begin(); it != ccopy.end();it++)
                    (*it)->disconnect(new ChatMessage("§cNativecord is shuting down."));
                break;
            }
        }
        PlayerConnection::connections.clear();
        PlayerConnection::activeConnections.clear();
        pthread_cancel(threadHandle);
        pthread_join(threadHandle, NULL);

        ServerInfo::reset();
        cout << "BUffers: " << DataBuffer::creations << endl;
        cout << "Chat instances: " << ChatMessage::count << endl;
    }catch(Exception* ex){
        cout << "Exception: " << ex->what() << endl;
    }
    return 0;
}