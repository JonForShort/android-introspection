package com.github.jonforshort.vpn;

interface IVpnService {

    void initialize();

    void start();

    void stop();

    void uninitialize();
}
