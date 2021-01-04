#include <stdlib.h>

#include <nfc/nfc.h>

static void print_hex(const uint8_t *pbtData, const size_t szBytes) {
    size_t  szPos;
 
    for (szPos = 0; szPos < szBytes; szPos++) {
        printf("%02x  ", pbtData[szPos]);
    }
    printf("\n");
}

int main(int argc, const char *argv[]) {
    nfc_device *device;
    nfc_target target;
    nfc_context *context;

    size_t deviceCount;
    nfc_connstring devices[8];

    const nfc_modulation desModulation = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
    };

    nfc_init(&context);

    if (context == NULL) {
        printf("Can't init libnfc\n");
        exit(EXIT_FAILURE);
    }
    printf("libNFC inited\n");

    deviceCount = nfc_list_devices(context, devices, 8);

    if (deviceCount <= 0) {
        printf("No NFC Reader found\n");
        exit(EXIT_FAILURE);
    } else if (deviceCount > 1) {
        printf("You have more than 1 NFC Reader plugged in, this is not suppoerted yet.\nPlease unplug all but 1\n");
        exit(EXIT_FAILURE);
    }
    printf("NFC Reader found\n");

    device = nfc_open(context, NULL);

    if (device == NULL) {
        printf("Can't open NFC Reader\n");
        exit(EXIT_FAILURE);
    }
    printf("NFC Reader opened\n");

    if (nfc_initiator_init(device) < 0) {
        nfc_perror(device, "nfc_initiator_init");
        exit(EXIT_FAILURE);
    }
    printf("NFC Reader inited\n");

    nfc_device_set_property_bool(device, NP_ACTIVATE_FIELD, false);
    nfc_device_set_property_bool(device, NP_HANDLE_CRC, true);
    nfc_device_set_property_bool(device, NP_HANDLE_PARITY, true);
    nfc_device_set_property_bool(device, NP_AUTO_ISO14443_4, true);
    nfc_device_set_property_bool(device, NP_ACTIVATE_FIELD, true);
    printf("NFC Reader configured\n");

    if (nfc_initiator_list_passive_targets(device, desModulation, &target, 1) > 1) {
        printf("More than 1 chip in field, for now only 1 is supported\n");
        exit(EXIT_FAILURE);
    }

    if (target.nm.nmt == NMT_ISO14443A && target.nti.nai.btSak == 0x20 && target.nti.nai.szAtsLen >= 5) {
        printf("Yes, it's a DESFire\n");
    }

    nfc_close(device);
    printf("Closing NFC Reader\n");
    nfc_exit(context);

    exit(EXIT_SUCCESS);
}