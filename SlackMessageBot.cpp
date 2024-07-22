#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>

#pragma comment(lib, "wininet.lib")

using namespace std;

int main() {
    // Initialize variables and configure connection
    HINTERNET hInternet = NULL, hConnect = NULL, hRequest = NULL;
    DWORD dwSize = 0;
    BOOL bResults = FALSE;
    wstring requestURL = L"slack.com";  
    wstring requestEndpoint = L"/api/chat.postMessage";  
    wstring accessToken = L"<my_oauth_token>"; 
    wstring customHeaders = L"Content-Type: application/x-www-form-urlencoded\r\n";
    
    // Get console input for message
    cout << "Enter your desired message to post to Slack:" << "\n";
    string message;
    getline(cin, message);
    string postData = "channel=<my_channel>&text=" + message; 
 

    // Initialize WinINet
    hInternet = InternetOpen(L"WinINet Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        cout << "InternetOpen failed: " << GetLastError() << endl;
        return 1;
    }

    // Connect to server
    hConnect = InternetConnect(hInternet, requestURL.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
    if (hConnect == NULL) {
        cout << "InternetConnect failed: " << GetLastError() << endl;
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Open HTTPS request
    hRequest = HttpOpenRequest(hConnect, L"POST", requestEndpoint.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID, 1);
    if (hRequest == NULL) {
        cout << "HttpOpenRequest failed: " << GetLastError() << endl;
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Convert accessToken to wide character array (LPCWSTR)
    wstring wideAccessToken(accessToken.begin(), accessToken.end());

    // Add authorization header
    wstring authorizationHeader = L"Authorization: Bearer " + wideAccessToken;
    bResults = HttpAddRequestHeaders(hRequest, authorizationHeader.c_str(), authorizationHeader.length(), HTTP_ADDREQ_FLAG_ADD);
    if (!bResults) {
        cout << "HttpAddRequestHeaders failed: " << GetLastError() << endl;
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Add custom headers
    bResults = HttpAddRequestHeaders(hRequest, customHeaders.c_str(), customHeaders.length(), HTTP_ADDREQ_FLAG_ADD);
    if (!bResults) {
        cout << "HttpAddRequestHeaders failed: " << GetLastError() << endl;
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Send POST data
    bResults = HttpSendRequest(hRequest, NULL, 0, (LPVOID)postData.c_str(), postData.length());
    if (!bResults) {
        cout << "HttpSendRequest failed: " << GetLastError() << endl;
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Read response
    char szBuffer[1024];
    DWORD dwRead = 0;
    while (InternetReadFile(hRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead > 0) {
        szBuffer[dwRead] = '\0';
        cout << szBuffer;
    }

    // Cleanup
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return 0;
}
