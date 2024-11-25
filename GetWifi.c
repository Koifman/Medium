#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wlanapi.h>
#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib
#pragma comment(lib, "wlanapi.lib")

int getWifi(){
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2; // The highest version of the client-supported WLAN API
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    // Open a handle to the WLAN API
    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        printf("Error: WlanOpenHandle failed with error: %ld\n", dwResult);
        return 1;
    }

    // Enumerate all wireless LAN interfaces
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        printf("Error: WlanEnumInterfaces failed with error: %ld\n", dwResult);
        WlanCloseHandle(hClient, NULL);
        return 1;
    }

    printf("Number of wireless interfaces: %lu\n\n", pIfList->dwNumberOfItems);

    // Iterate through each wireless LAN interface
    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
        PWLAN_INTERFACE_INFO pIfInfo = &pIfList->InterfaceInfo[i];

        printf("Interface Index: %lu\n", i);
        printf("Interface Description: %ws\n", pIfInfo->strInterfaceDescription);

        // Get the list of available networks
        PWLAN_AVAILABLE_NETWORK_LIST pNetworkList = NULL;
        dwResult = WlanGetAvailableNetworkList(
            hClient,
            &pIfInfo->InterfaceGuid,
            0,
            NULL,
            &pNetworkList
        );

        if (dwResult != ERROR_SUCCESS) {
            printf("Error: WlanGetAvailableNetworkList failed with error: %ld\n", dwResult);
            continue;
        }

        printf("Available Wi-Fi Networks:\n");
        for (DWORD j = 0; j < pNetworkList->dwNumberOfItems; j++) {
            PWLAN_AVAILABLE_NETWORK pNetwork = &pNetworkList->Network[j];

            // Print the SSID
            char ssid[DOT11_SSID_MAX_LENGTH + 1] = { 0 };
            memcpy(ssid, pNetwork->dot11Ssid.ucSSID, pNetwork->dot11Ssid.uSSIDLength);
            printf("  %lu. SSID: %s\n", j + 1, ssid);
        }

        // Free the network list memory
        if (pNetworkList != NULL) {
            WlanFreeMemory(pNetworkList);
        }

        printf("\n");
    }

    // Free the interface list memory
    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
    }

    // Close the WLAN API handle
    WlanCloseHandle(hClient, NULL);

    return 0;

}
int main() {
    getWifi();
    return 0;
}
