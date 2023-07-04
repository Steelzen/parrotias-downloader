#include <wx/wx.h>
#include <wx/filename.h>
#include <windows.h>
#include <shlobj.h>
#include <curl/curl.h>
#include <zip.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "logo_data.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Downloader : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(Downloader);

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSetStatusInstallText(wxCommandEvent& event);

    wxBoxSizer* CreateMainSizer();
};

enum
{
};

bool Downloader::OnInit()
{
    //// check libcurl info 
    //curl_version_info_data* vinfo = curl_version_info(CURLVERSION_NOW);
    //if (vinfo->features & CURL_VERSION_SSL) {
    //    wxLogMessage("SSL support is enabled.");
    //}
    //else {
    //    wxLogMessage("SSL support is NOT enabled.");
    //}

    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Parrotias setup", wxDefaultPosition, wxSize(1000, 300), wxDEFAULT_FRAME_STYLE)
{
    SetBackgroundColour(wxColour(240, 240, 240));
    
    SetSizer(CreateMainSizer());

    Fit();

    Centre();

    SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a Parrotias Windows app downloader ",
        "About Parrotias Downloader", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnSetStatusInstallText(wxCommandEvent& event)
{
    std::cout << "Install Status Changed";
}

// Function to write downloaded data to a file
size_t WriteData(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

// Function to download a file from an HTTP source
bool DownloadFile(const char* url, const char* destination)
{
    CURLcode global_init_res = curl_global_init(CURL_GLOBAL_ALL);
    if (global_init_res != CURLE_OK) {
        wxMessageBox(wxString::Format("curl_global_inint() failed: %s", curl_easy_strerror(global_init_res)), "Error", wxOK | wxICON_ERROR);
        return false;
    }

    CURL* curl = curl_easy_init();
    if (curl)
    {
        FILE* file = fopen(destination, "wb");
        if (file)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // for github source need to design a location like this
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            CURLcode result = curl_easy_perform(curl);

            fclose(file);

            if (result != CURLE_OK)
            {
                wxMessageBox(wxString::Format("Failed to download file: %s", curl_easy_strerror(result)), "Error", wxOK | wxICON_ERROR);
                curl_global_cleanup();
                return false;
            }
        }
        else
        {
            wxMessageBox(wxString::Format("Failed to create file: %s", destination), "Error", wxOK | wxICON_ERROR);
            curl_global_cleanup();
            return false;
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return true;
    }

    wxMessageBox("Failed to initialize libcurl", "Error", wxOK | wxICON_ERROR);
    curl_global_cleanup();
    return false;
}

// Find Users folder in computer and download the file
void DownloadToUserFolder(const char* url, const char* filename) {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path))) {
        std::string destination = std::string(path) + "\\" + std::string(filename);
        DownloadFile(url, destination.c_str());
    }
    else {
        // Replace printf with appropriate method for showing an error message
        wxMessageBox("Failed to get user directory", "Error", wxOK | wxICON_ERROR);
    }
}

// Download and unzip usage
void DownloadAndUnzipFile()
{
    DownloadToUserFolder("https://github.com/Steelzen/parrotias-windows/archive/refs/tags/release.zip", "parrotias-windows-release.zip");

    //TODO: Unzip downloaded file

    //TODO: Delete downloaded file
}

wxBoxSizer* MyFrame::CreateMainSizer()
{
    // Create the main sizer that will contain the logo, and subsizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Create sub sizer to contain messages and progress bar
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    // Load the image data using stb_image
    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory(logo_data, logo_data_size, &width, &height, &channels, 0);

    if (image_data)
    {
        wxImage image(width, height, image_data, true);
        wxBitmap logoBitmap(image);
        wxStaticBitmap* logo = new wxStaticBitmap(this, wxID_ANY, logoBitmap);
        mainSizer->Add(logo, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);

        stbi_image_free(image_data);
    }
    else
    {
        // Failed to load image data
        wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
    }
    
    // Create title message
    wxStaticText* titleMessage = new wxStaticText(this, wxID_ANY, "Parrotias");
    wxFont font(wxFontInfo(14).Bold());
    titleMessage->SetFont(font);
    rightSizer->Add(titleMessage, 0, wxALL | wxALIGN_LEFT, 10);

    // Under message
    wxStaticText* customMessage = new wxStaticText(this, wxID_ANY, "Can't wait you will use it!", wxDefaultPosition, wxSize(400, 100), wxGA_HORIZONTAL);
    rightSizer->Add(customMessage, 0, wxALL | wxALIGN_LEFT, 10);

    // Create the progress bar
    wxGauge* progressBar = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(400, 10), wxGA_HORIZONTAL);
    rightSizer->Add(progressBar, 0, wxALL | wxEXPAND, 10);

    // Create downloading message
    wxStaticText* downloadMessage = new wxStaticText(this, wxID_ANY, "Installing Parrotias...");
    rightSizer->Add(downloadMessage, 0, wxALL | wxALIGN_LEFT, 10);

    // Place Subsizer to MainSizer
    mainSizer->Add(rightSizer, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);

    // Set the main sizer as the sizer for the frame
    SetSizer(mainSizer);

    DownloadAndUnzipFile();


    return mainSizer; 
}