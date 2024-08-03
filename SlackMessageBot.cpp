#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>

#pragma comment(lib, "wininet.lib")

using namespace std;

void static cleanup(HINTERNET hRequest, HINTERNET hConnect, HINTERNET hInternet){
    // Clean-up on exit
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

int main() {
    // Initialize variables and configure connection
    HINTERNET hInternet = NULL, hConnect = NULL, hRequest = NULL;
    DWORD dwSize = 0;
    BOOL bResults = FALSE;
    string channelId = "<channel_id_here>";
    wstring requestURL = L"slack.com";  
    wstring requestEndpoint = L"/api/chat.postMessage";  
    wstring accessToken = L"<slack_oauth_token_here>"; 
    wstring customHeaders = L"Content-Type: application/x-www-form-urlencoded\r\n";
    
    // Get console input for message
    cout << "Enter your desired message to post to Slack:" << "\n";
    string message;
    getline(cin, message);
    string postData = "channel=" + channelId + "&text=" + message; 
 

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
        cleanup(hRequest, hConnect, hInternet);
        return 1;
    }

    // Open HTTPS request
    hRequest = HttpOpenRequest(hConnect, L"POST", requestEndpoint.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID, 1);
    if (hRequest == NULL) {
        cout << "HttpOpenRequest failed: " << GetLastError() << endl;
        cleanup(hRequest, hConnect, hInternet);
        return 1;
    }

    // Convert accessToken to wide character array (LPCWSTR)
    wstring wideAccessToken(accessToken.begin(), accessToken.end());

    // Add authorization header
    wstring authorizationHeader = L"Authorization: Bearer " + wideAccessToken;
    bResults = HttpAddRequestHeaders(hRequest, authorizationHeader.c_str(), authorizationHeader.length(), HTTP_ADDREQ_FLAG_ADD);
    if (!bResults) {
        cout << "HttpAddRequestHeaders failed: " << GetLastError() << endl;
        cleanup(hRequest, hConnect, hInternet);
        return 1;
    }

    // Add custom headers
    bResults = HttpAddRequestHeaders(hRequest, customHeaders.c_str(), customHeaders.length(), HTTP_ADDREQ_FLAG_ADD);
    if (!bResults) {
        cout << "HttpAddRequestHeaders failed: " << GetLastError() << endl;
        cleanup(hRequest, hConnect, hInternet);
        return 1;
    }

    // Send POST data
    bResults = HttpSendRequest(hRequest, NULL, 0, (LPVOID)postData.c_str(), postData.length());
    if (!bResults) {
        cout << "HttpSendRequest failed: " << GetLastError() << endl;
        cleanup(hRequest, hConnect, hInternet);
        return 1;
    }

    // Read response
    char szBuffer[1024];
    DWORD dwRead = 0;
    while (InternetReadFile(hRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead > 0) {
        szBuffer[dwRead] = '\0';
        cout << szBuffer;
    }

    cleanup(hRequest, hConnect, hInternet);

    return 0;
}
