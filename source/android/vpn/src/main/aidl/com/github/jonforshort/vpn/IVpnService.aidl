package com.github.jonforshort.vpn;

import android.os.ParcelFileDescriptor;

interface IVpnService {

    void initialize(in IBinder listener, in ParcelFileDescriptor vpnSocket);

    void start();

    void stop();

    void uninitialize();
}
