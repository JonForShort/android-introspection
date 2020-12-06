package com.github.jonforshort.vpn;

import android.os.ParcelFileDescriptor;

interface IVpnService {

    void start(in ParcelFileDescriptor fd);

    void stop();
}
