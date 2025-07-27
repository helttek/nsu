#include <iostream>
#include <libusb-1.0/libusb.h>
#include <cstdio>
using namespace std;

void printdev(libusb_device* dev) {
    libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    libusb_device_handle* handle;
    libusb_open(dev, &handle);
    unsigned char str[500];
    if (desc.bDeviceClass && handle) {
        libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, str, 500);
    }
    cout << "Class: " << str << endl;
    libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, str, 500);
    cout << "Manufacturer: " << str << endl;
    libusb_get_string_descriptor_ascii(handle, desc.iProduct, str, 500);
    cout << "Product: " << str << endl;
    cout << "------------------------------------------\n";
}

int main() {
    libusb_device** devs;
    libusb_context* ctx = nullptr;
    ssize_t cnt;
    int r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr,
                "Error: initialization not completed, code: %d.\n", r);
        return 1;
    }
    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        fprintf(stderr,
                "Error: couldn't get usb device list.\n");
        return 1;
    }
    printf("found %ld devices\n", cnt);
    for (ssize_t i = 0; i < cnt; i++) {
        printdev(devs[i]);
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return 0;
}
