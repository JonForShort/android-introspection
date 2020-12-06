package com.github.jonforshort.vpn;

import android.os.ParcelFileDescriptor;

interface IVpnService {

    void start(in IBinder listener, in ParcelFileDescriptor vpnSocket);

    void stop();
}
