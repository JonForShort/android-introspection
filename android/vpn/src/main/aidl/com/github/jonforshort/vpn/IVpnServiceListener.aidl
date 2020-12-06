package com.github.jonforshort.vpn;

interface IVpnServiceListener {

    void onSessionCreated(int socket);

    void onSessionDestroyed(int socket);
}
